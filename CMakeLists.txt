cmake_minimum_required(VERSION 3.20)

project(TrackGuideOnboard)

# Set paths for RTKLIB
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libs)

# Find libraries
find_library(WS2811_LIBRARY ws2811 REQUIRED)
find_library(PIGPIO_LIBRARY pigpio REQUIRED)


# Include RTKLIB headers
include_directories(${INCLUDE_DIR})

find_package(AWSSDK REQUIRED COMPONENTS kinesis core)

# Include directory for tcpserver.h
include_directories(${CMAKE_SOURCE_DIR}/src/rtkservice)
include_directories(${CMAKE_SOURCE_DIR}/src/inputoutput)
include_directories(${CMAKE_SOURCE_DIR}/src/states)
include_directories(${CMAKE_SOURCE_DIR}/src/boundarylogic)

# Create executable for main
add_executable(main src/main.cpp)

# Create macors library
add_library(rtkservice src/rtkservice/rtkservice.cpp)
add_library(tcpserver src/rtkservice/tcpserver.cpp)
add_library(ledcontrol src/inputoutput/ledcontrol.cpp)
add_library(buttons src/inputoutput/buttons.cpp)
add_library(semaphoreguard src/inputoutput/semaphoreguard.cpp)
add_library(states src/states/states.cpp)
add_library(boundarylogic src/boundarylogic/boundarylogic.cpp)
add_library(kinesisStream src/inputoutput/kinesisStream.cpp)

# Link macors library to the main executable
target_link_libraries(main PRIVATE rtkservice)
target_link_libraries(main PRIVATE tcpserver)
target_link_libraries(main PRIVATE ledcontrol ${WS2811_LIBRARY})
target_link_libraries(main PRIVATE buttons ${PIGPIO_LIBRARY})
target_link_libraries(main PRIVATE states)
target_link_libraries(main PRIVATE semaphoreguard)
target_link_libraries(main PRIVATE boundarylogic)
target_link_libraries(main PRIVATE kinesisStream ${AWSSDK_LINK_LIBRARIES})
target_link_libraries(main PRIVATE ${AWSSDK_LINK_LIBRARIES})
target_link_libraries(main PRIVATE ${WS2811_LIBRARY})
target_link_libraries(main PRIVATE ${PIGPIO_LIBRARY})
target_link_libraries(main PRIVATE ${OPENSSL_LIBRARIES})
target_include_directories(main PRIVATE /usr/local/include)