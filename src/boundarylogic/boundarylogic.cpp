// filepath: /Users/mitchellsylvia/TrackGuideOnboard/src/boundarylogic/boundarylogic.cpp
#include "boundarylogic.h"
#include <fstream>
#include <sstream>
#include <cmath>
#include <stdexcept>

// Constructor that initializes max_distance
RecordBoundaryLogic::RecordBoundaryLogic(float max_distance) : max_distance(max_distance) {}

// Default constructor
RecordBoundaryLogic::RecordBoundaryLogic() : max_distance(0.0f) {}

// Function to calculate distance given latitude and longitude
float RecordBoundaryLogic::calculate_distance(float latitude, float longitude) {
    // Placeholder implementation for distance calculation
    // You can replace this with actual KD-tree and perpendicular distance calculation
    if (recorded_path.empty()) {
        throw std::runtime_error("Recorded path is empty.");
    }

    float min_distance = std::numeric_limits<float>::max();
    for (const auto& point : recorded_path) {
        float lat = std::get<0>(point);
        float lon = std::get<1>(point);
        float distance = std::sqrt(std::pow(lat - latitude, 2) + std::pow(lon - longitude, 2));
        if (distance < min_distance) {
            min_distance = distance;
        }
    }
    return min_distance;
}

// Function to load track from a saved CSV file
void RecordBoundaryLogic::load_track(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + file_path);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string lat_str, lon_str;
        if (std::getline(ss, lat_str, ',') && std::getline(ss, lon_str, ',')) {
            float latitude = std::stof(lat_str);
            float longitude = std::stof(lon_str);
            recorded_path.emplace_back(latitude, longitude);
        }
    }
    file.close();
}