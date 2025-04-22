// filepath: /Users/mitchellsylvia/TrackGuideOnboard/src/states/states.cpp
#include "states.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <mutex>

States::States(LEDControl& ledController, BoundaryLogic& boundaryLogic, ApiClient& apiClient)
    : ledController_(ledController), boundaryLogic_(boundaryLogic), apiClient_(apiClient), track_loaded_(false), is_recording_(false){
}

void States::run_record_function(const char* content) {
    track_loaded_ = false;
    // Implement the function you want to run in a new process
    std::cout << "Running record function in a new process." << std::endl;

    // Example content received
    // Extract the latitude and longitude values
    if (!is_recording_)
    {
        ledController_.indicate_record_startup();
        is_recording_ = true;
    }

    std::istringstream iss(content);
    std::string date, time;
    double latitude, longitude;

    std::cout << "Content stream: " << content << std::endl;

    iss >> date >> time >> latitude >> longitude;
    std::string mode = "record";
    double threshold = ledController_.get_max_distance_as_lat_long_deg();
    std::async(std::launch::async, [&]() {
        apiClient_.send_post_request(apiClient_.create_request(latitude, longitude, threshold, mode));
    });
    // kinesisStream_.sendPositionData(latitude, longitude);

    // Use a mutex to avoid race conditions when writing to the file
    std::lock_guard<std::mutex> lock(file_mutex);

    // Open the CSV file in append mode
    std::ofstream outfile("coordinates.csv", std::ios_base::app);
    if (!outfile.is_open()) {
        std::cerr << "Failed to open coordinates.csv" << std::endl;
        return;
    }
    outfile.precision(8);
    outfile << std::fixed;

    // Write the latitude and longitude to the CSV file
    outfile << latitude << ", " << longitude << std::endl;

    // Close the file
    outfile.close();

    std::cout << "Latitude: " << latitude << ", Longitude: " << longitude << " saved to coordinates.csv" << std::endl;
}

void States::run_play_function(const char* content) {
    // Extract the latitude and longitude values
    is_recording_ = false;
    std::istringstream iss(content);
    std::string date, time;
    double latitude, longitude;

    iss >> date >> time >> latitude >> longitude;
    // kinesisStream_.sendPositionData(latitude, longitude);

    if (!track_loaded_) {
        std::cout << "Loading track from coordinates.csv" << std::endl;
        track_loaded_ = boundaryLogic_.load_track("coordinates.csv");
    }

    // Calculate the distance
    try {
        if(track_loaded_){
            double distance = boundaryLogic_.calculate_distance(latitude, longitude);
            std::cout << "Distance from track (cm): " << distance << std::endl;
            ledController_.set_led_location(distance, ledController_.map_color(Color::RED), 3);
            std::string mode = "play";
            double threshold = ledController_.get_max_distance_as_lat_long_deg();
            std::async(std::launch::async, [&]() {
                apiClient_.send_post_request(apiClient_.create_request(latitude, longitude, threshold, mode));
            });
        }
    } catch (const std::exception& e) {
        std::cerr << "Error calculating distance: " << e.what() << std::endl;
    }
}

void States::run_reset_function() {
    std::lock_guard<std::mutex> lock(file_mutex);
    is_recording_ = false;
    if (remove("coordinates.csv") != 0) {
        std::cout << "Could not delete coordinates.csv" << std::endl;
    } else {
        std::cout << "coordinates.csv successfully deleted" << std::endl;
    }
}

void States::run_standby_function() {
    std::cout << "Running standby function." << std::endl;
    std::string mode = "standby";
    double threshold = ledController_.get_max_distance_as_lat_long_deg();
    std::async(std::launch::async, [&]() {
        apiClient_.send_post_request(apiClient_.create_request(0.0, 0.0, threshold, mode));
    });
    ledController_.clear();
}