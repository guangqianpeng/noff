if(NOT MUDUO_PATH)
    if(CMAKE_BUILD_TYPE EQUAL Debug)
        set(MUDUO_PATH "${CMAKE_HOME_DIRECTORY}/build/debug-install-cpp11")
    else()
        set(MUDUO_PATH "${CMAKE_HOME_DIRECTORY}/build/release-install-cpp11")
    endif()
endif()
set(Muduo_INCLUDE_DIR ${MUDUO_PATH}/include)

aux_source_directory(. SRC)

include_directories(${Muduo_INCLUDE_DIR})
add_library(Dns STATIC ${SRC})