cmake_minimum_required(VERSION 3.20)

project(TrackGuideOnboard)

add_executable(main src/main.cpp)
add_library(macors src/MaCORS/MaCORS.cpp)

find_package(CURL REQUIRED)
target_include_directories(macors PRIVATE ${CURL_INCLUDE_DIRS})
target_link_libraries(macors PRIVATE ${CURL_LIBRARIES})

target_link_libraries(main PRIVATE macors)