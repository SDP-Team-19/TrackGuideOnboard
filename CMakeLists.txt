cmake_minimum_required(VERSION 3.10)
project(TrackGuideOnboard)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set include paths
include_directories(
    src/states
    src/boundarylogic
    src/inputoutput
    src/rtkservice
    /include
)

# Manually specify AWS SDK installation path
set(AWSSDK_DIR "~/Desktop/TrackGuide-App-trackguide-map/aws-sdk-install/lib/cmake/AWSSDK")

find_library(WS2811_LIBRARY ws2811 REQUIRED)
find_library(PIGPIO_LIBRARY pigpio REQUIRED)

find_package(AWSSDK REQUIRED COMPONENTS kinesis core)

# Add source files
set(SOURCES
    src/main.cpp
    src/states/states.cpp
    src/boundarylogic/boundarylogic.cpp
    src/inputoutput/buttons.cpp
    src/inputoutput/kinesisStream.cpp
    src/inputoutput/ledcontrol.cpp
    src/inputoutput/semaphoreguard.cpp
    src/rtkservice/rtkservice.cpp
    src/rtkservice/tcpserver.cpp
)

add_executable(main ${SOURCES})
target_link_libraries(main PRIVATE ${WS2811_LIBRARY})
target_link_libraries(main PRIVATE ${PIGPIO_LIBRARY})
target_link_libraries(main PRIVATE ${AWSSDK_LINK_LIBRARIES})
