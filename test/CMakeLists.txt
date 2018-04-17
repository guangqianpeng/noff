if(NOT MUDUO_PATH)
    if(CMAKE_BUILD_TYPE EQUAL Debug)
        set(MUDUO_PATH "${CMAKE_HOME_DIRECTORY}/build/debug-install-cpp11")
    else()
        set(MUDUO_PATH "${CMAKE_HOME_DIRECTORY}/build/release-install-cpp11")
    endif()
endif()
set(Muduo_INCLUDE_DIR ${MUDUO_PATH}/include)

include_directories(../dpi ../app/http)

add_executable(capture TestCapture.cc)
target_link_libraries(capture Dpi ${muduo_base})
target_link_libraries(capture pcap pthread rt)

add_executable(dispatcher TestDispatcher.cc)
target_link_libraries(dispatcher Dpi ${muduo_base})
target_link_libraries(dispatcher pcap pthread rt)

add_executable(ipFragment TestIpFragment.cc)
target_link_libraries(ipFragment Dpi ${muduo_base})
target_link_libraries(ipFragment pcap pthread rt tcmalloc)

add_executable(tcpFragment TestTcpFragment.cc)
target_link_libraries(tcpFragment Dpi ${muduo_base})
target_link_libraries(tcpFragment pcap pthread rt tcmalloc)

add_executable(http TestHttp.cc)

target_link_libraries(http Http ${muduo_base})
target_link_libraries(http pcap pthread rt tcmalloc pfring)