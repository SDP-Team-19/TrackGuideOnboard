cmake_minimum_required(VERSION 3.20)

project(TrackGuideOnboard)

# Set paths for RTKLIB
set(RTKLIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(RTKLIB_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/libs)

# Find RTKLIB library
find_library(RTKLIB_LIB rtklib PATHS ${RTKLIB_LIBRARY_DIR})

# Include RTKLIB headers
include_directories(${RTKLIB_INCLUDE_DIR})

# Create executable for main
add_executable(main src/main.cpp)

# Create macors library
add_library(macors src/MaCORS/MaCORS.cpp)

# Find CURL package
find_package(CURL REQUIRED)

# Include CURL headers for macors
target_include_directories(macors PRIVATE ${CURL_INCLUDE_DIRS})

# Link libraries to macors
target_link_libraries(macors PRIVATE ${RTKLIB_LIB} ${CURL_LIBRARIES})

# Link macors library to the main executable
target_link_libraries(main PRIVATE macors ${RTKLIB_LIB} ${CURL_LIBRARIES})
