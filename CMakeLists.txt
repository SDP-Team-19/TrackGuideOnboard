cmake_minimum_required(VERSION 3.10)
project(TrackGuideOnboard)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set include paths
include_directories(
    ${CMAKE_SOURCE_DIR}/src/states
    ${CMAKE_SOURCE_DIR}/src/boundarylogic
    ${CMAKE_SOURCE_DIR}/src/inputoutput
    ${CMAKE_SOURCE_DIR}/src/rtkservice
    ${CMAKE_SOURCE_DIR}/include
)

# Manually specify AWS SDK installation path
set(AWSSDK_DIR "~/Desktop/TrackGuide-App-trackguide-map/aws-sdk-install/lib/cmake/AWSSDK")

find_library(WS2811_LIBRARY ws2811 REQUIRED)
find_library(PIGPIO_LIBRARY pigpio REQUIRED)

find_package(AWSSDK REQUIRED COMPONENTS kinesis core)

# Add source files
set(SOURCES
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/states/states.cpp
    ${CMAKE_SOURCE_DIR}/src/boundarylogic/boundarylogic.cpp
    ${CMAKE_SOURCE_DIR}/src/inputoutput/buttons.cpp
    ${CMAKE_SOURCE_DIR}/src/inputoutput/ledcontrol.cpp
    ${CMAKE_SOURCE_DIR}/src/inputoutput/semaphoreguard.cpp
    ${CMAKE_SOURCE_DIR}/src/rtkservice/rtkservice.cpp
    ${CMAKE_SOURCE_DIR}/src/rtkservice/tcpserver.cpp
)

add_executable(main ${SOURCES})
target_link_libraries(main PRIVATE ${WS2811_LIBRARY})
target_link_libraries(main PRIVATE ${PIGPIO_LIBRARY})
target_link_libraries(main PRIVATE aws-cpp-sdk-kinesis aws-cpp-sdk-core)
