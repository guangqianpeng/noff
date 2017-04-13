//
// Created by frabk on 17-4-10.
//

#include <functional>
#include <signal.h>

#include <muduo/base/Logging.h>

#include "Capture.h"

using std::placeholders::_1;
using std::placeholders::_2;

Capture *cap;

void sigHandler(int)
{
    cap->breakLoop();
}

class PacektCounter {

public:
    void onMessage(const pcap_pkthdr *, const u_char *data)
    {
        if (++counter_ % 10 == 0) {
            LOG_INFO << counter_ << " packets received";
        }
    }

private:
    int counter_ = 0;
};

class IpFragmentCounter {

public:
    void onMessage(const ip *, int len)
    {
        if (++counter_ % 10 == 0) {
            LOG_INFO << counter_ << " IP fragments received";
        }
    }

private:
    int counter_ = 0;
};

int main()
{
    muduo::Logger::setLogLevel(muduo::Logger::TRACE);

    signal(SIGINT, sigHandler);

    for (int i = 0; i < 5; ++i) {

        PacektCounter       packetCounter;
        IpFragmentCounter   ipCounter;

        cap = new Capture("any", 65536, 1, 1000);

        cap -> setFilter("ip");

        cap -> addPacketCallBack(std::bind(
                &PacektCounter::onMessage, &packetCounter, _1, _2));
        cap->addIpFragmentCallback(std::bind(
                &IpFragmentCounter::onMessage, &ipCounter, _1, _2));

        // block until enough packets received
        cap->startLoop(20);

        // break loop and
        delete cap;
    }
}