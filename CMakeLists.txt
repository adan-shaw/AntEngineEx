cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)
project(AntEngine)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(APP_OS Linux)
    set(DEBUG_FLAGS "-g -O0 -Wall -DEBUG")
    set(RELEASE_FLAGS "-O3")
    # 娣