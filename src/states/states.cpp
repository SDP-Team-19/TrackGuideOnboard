// filepath: /Users/mitchellsylvia/TrackGuideOnboard/src/states/states.cpp
#include "states.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <mutex>

States::States(LEDControl ledController, BoundaryLogic& boundaryLogic)
    : ledController_(ledController), boundaryLogic_(boundaryLogic), track_loaded_(false) {
}

void States::run_record_function(const char* content) {
    track_loaded_ = false;
    // Implement the function you want to run in a new process
    std::cout << "Running record function in a new process." << std::endl;

    // Example content received
    // Extract the latitude and longitude values
    std::istringstream iss(content);
    std::string date, time;
    double latitude, longitude;

    iss >> date >> time >> latitude >> longitude;

    // Use a mutex to avoid race conditions when writing to the file
    static std::mutex file_mutex;
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
    std::istringstream iss(content);
    std::string date, time;
    double latitude, longitude;

    iss >> date >> time >> latitude >> longitude;

    if (!track_loaded_) {
        std::cout << "Loading track from coordinates.csv" << std::endl;
        boundaryLogic_.load_track("coordinates.csv");
        track_loaded_ = true;
    }

    // Calculate the distance
    try {
        double distance = boundaryLogic_.calculate_distance(latitude, longitude);
        std::cout << "Distance from track: " << distance << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error calculating distance: " << e.what() << std::endl;
    }
}

void States::run_reset_function() {
    // Implementation for run_record_function without parameters
    std::cout << "Running record function without parameters." << std::endl;
}