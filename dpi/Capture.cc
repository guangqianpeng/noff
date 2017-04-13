//
// Created by frabk on 17-4-10.
//

#include <assert.h>
#include <stdio.h>
#include <string>
#include <muduo/base/Logging.h>

#include "Capture.h"

using std::placeholders::_1;
using std::placeholders::_2;

Capture::Capture(const char *device, int snaplen, int promisc, int msTimeout)
        :pcap_(NULL),
         device_(device),
         filter_(),
         linkType(PCAP_ERROR_NOT_ACTIVATED),
         linkTypeStr(NULL),
         linkOffset(0),
         errBuf_{'\0'},
         running_(false)
{
    pcap_ = pcap_open_live(device, snaplen, promisc, msTimeout, errBuf_);

    // may warning
    if (pcap_ != NULL && errBuf_[0] != '\0') {
        LOG_WARN << "capture " << errBuf_;
    }
    if (pcap_ == NULL) {
        LOG_ERROR << "capture " << errBuf_;
        exit(1);
    }

    linkType = pcap_datalink(pcap_);

    linkTypeStr = pcap_datalink_val_to_name(linkType);
    assert(linkTypeStr != NULL);

    switch (linkType) {
        // ethernet packet
        case DLT_EN10MB:
            linkOffset = 14;
            break;
        case DLT_LINUX_SLL:
            linkOffset = 16;
            break;
        case PCAP_ERROR_NOT_ACTIVATED:
            // can never happen
            LOG_FATAL << "capture " << device_
                      << ": not activated";
            break;
        default:
            LOG_ERROR << "capture " << device_
                     <<": unsupported link type "
                     << linkTypeStr;
            exit(1);
    }

    LOG_INFO << "capture " << device_
             << ": open, link type " << linkTypeStr;

    addPacketCallBack(std::bind(
            &Capture::onRecvPacket, this, _1, _2));
}

Capture::~Capture()
{
    if (running_) {
        breakLoop();
    }

    pcap_close(pcap_);

    LOG_INFO << "capture " << device_
             << ": stopped";
}

void Capture::startLoop(int packetCount)
{
    assert(!running_);
    running_ = true;

    LOG_INFO << "capture " << device_ << ": started";

    int err = pcap_loop(pcap_, packetCount, internalCallBack, reinterpret_cast<u_char*>(this));
    switch (err) {
        case 0:
            LOG_INFO << "capture " << device_
                     << ": break loop on " << packetCount
                     << " packet";
            break;
        case -1:
            LOG_ERROR << "capture " << pcap_geterr(pcap_);
            break;
        case -2:
            LOG_INFO << "capture "<< device_
                     << ": break loop by user";
            break;
        default:
            LOG_FATAL << "capture " << device_
                      <<": break loop by unknown error";
            break;
    }

    logCaptureStats();

    running_ = false;
}

void Capture::breakLoop()
{
    assert(running_);
    running_ = false;

    pcap_breakloop(pcap_);
}

void Capture::setFilter(const char *str)
{
    if (pcap_compile(pcap_, &filter_, str, 1, 0) != 0) {
        LOG_ERROR << "pcap compile: " << pcap_geterr(pcap_);
        exit(1);
    }

    if (pcap_setfilter(pcap_, &filter_) != 0) {
        LOG_ERROR << "pcap setfilter: " << pcap_geterr(pcap_);
        exit(1);
    }
}

void Capture::logCaptureStats()
{
    pcap_stat stat;
    pcap_stats(pcap_, &stat);
    LOG_INFO << "capture " << device_
             << ": receive packet " << stat.ps_recv
             << ", drop by kernel " << stat.ps_drop
             << ", drop by filter " << stat.ps_ifdrop;
}

void Capture::onRecvPacket(const pcap_pkthdr *hdr, const u_char *data)
{
    if (hdr->caplen < linkOffset) {
        LOG_WARN << "capture " << device_
                 << ": packet length too short";
        return;
    }

    switch (linkType) {

        case DLT_EN10MB:
            if (data[12] != 0x08 || data[13] != 0x00) {
                LOG_DEBUG << "capture " << device_
                          << ": receive none IP packet";
                return;
            }
            break;

        case DLT_LINUX_SLL:
            if (data[14] != 0x08 || data[15] != 0x00) {
                LOG_DEBUG << "capture " << device_
                          << ": receive none IP packet";
                return;
            }
            break;

        default:
            // never happened
            LOG_FATAL << "capture " << device_
                      << ": receive unsupported packet";
            return;
    }

    for (auto& func : ipFragmentCallbacks_) {
        func(reinterpret_cast<const ip*>(data),
             hdr->caplen - linkOffset);
    }
}

void Capture::internalCallBack(u_char *user, const struct pcap_pkthdr *hdr, const u_char *data)
{
    Capture *cap = reinterpret_cast<Capture*>(user);

    for (auto& func : cap->packetCallbacks_) {
        func(hdr, data);
    }
}