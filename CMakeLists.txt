cmake_minimum_required(VERSION 3.10)
project(TrackGuideOnboard)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")

# Manually specify AWS SDK installation path
set(AWSSDK_DIR "~/Desktop/TrackGuide-App-trackguide-map/aws-sdk-install/lib/cmake/AWSSDK")

find_library(WS2811_LIBRARY ws2811 REQUIRED)
find_library(PIGPIO_LIBRARY pigpio REQUIRED)

find_package(AWSSDK REQUIRED COMPONENTS kinesis core)

# Create the executable target
add_executable(main
    src/main.cpp
    src/states/states.cpp
    src/boundarylogic/boundarylogic.cpp
    src/inputoutput/buttons.cpp
    src/inputoutput/ledcontrol.cpp
    src/inputoutput/semaphoreguard.cpp
    src/rtkservice/rtkservice.cpp
    src/rtkservice/tcpserver.cpp
)

# Set include directories *only* for this target
target_include_directories(main PRIVATE
    ${CMAKE_SOURCE_DIR}/src/states
    ${CMAKE_SOURCE_DIR}/src/boundarylogic
    ${CMAKE_SOURCE_DIR}/src/inputoutput
    ${CMAKE_SOURCE_DIR}/src/rtkservice
    ${CMAKE_SOURCE_DIR}/include
)

# Link required libraries
target_link_libraries(main PRIVATE
    ${WS2811_LIBRARY}
    ${PIGPIO_LIBRARY}
    aws-cpp-sdk-kinesis
    aws-cpp-sdk-core
)