cmake_minimum_required(VERSION 3.20)

project(TrackGuideOnboard)

# Set paths for RTKLIB
set(RTKLIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(RTKLIB_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/libs)

# Find RTKLIB library
find_library(RTKLIB_LIB rtklib PATHS ${RTKLIB_LIBRARY_DIR})

# Include RTKLIB headers
include_directories(${RTKLIB_INCLUDE_DIR})

# Include directory for tcpserver.h
include_directories(${CMAKE_SOURCE_DIR}/src/rtkservice)

# Create executable for main
add_executable(main src/main.cpp)

# Create macors library
add_library(rtkservice src/rtkservice/rtkservice.cpp)
add_library(tcpserver src/rtkservice/tcpserver.cpp)

# Find CURL package
find_package(CURL REQUIRED)

# Link macors library to the main executable
target_link_libraries(main PRIVATE rtkservice ${RTKLIB_LIB} ${CURL_LIBRARIES})
target_link_libraries(main PRIVATE tcpserver ${RTKLIB_LIB} ${CURL_LIBRARIES})
