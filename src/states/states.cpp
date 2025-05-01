// filepath: /Users/mitchellsylvia/TrackGuideOnboard/src/states/states.cpp
#include "states.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <mutex>

States::States(LEDControl& ledController, BoundaryLogic& boundaryLogic, ApiClient& apiClient)
    : ledController_(ledController), boundaryLogic_(boundaryLogic), apiClient_(apiClient), track_loaded_(false), is_recording_(false), is_playing_(false), previous_time_(0.0), previous_latitude_(0.0), previous_longitude_(0.0), previous_color_(0x00002000){
}

void States::run_record_function(const char* content) {
    track_loaded_ = false;
    is_playing_ = false;
    std::cout << "Running record function in a new process." << std::endl;

    if (!is_recording_) {
        previous_latitude_ = 0.0;
        previous_longitude_ = 0.0;
        previous_time_ = 0.0;
        ledController_.indicate_record_startup();
        is_recording_ = true;
    }

    std::istringstream iss(content);
    std::vector<std::string> tokens;
    std::string token;
    double latitude, longitude;

    while (iss >> token) {
        tokens.push_back(token);
    }
    if (!(tokens.size() > 13)) {
        std::cerr << "Invalid data format" << std::endl;
        return;
    }

    std::string time = tokens[1];
    float minutes = 0;
    float seconds = 0;
    float milliseconds = 0;
    try {
        if (time.length() >= 12) {
            minutes = std::stof(time.substr(3, 2));
            seconds = std::stof(time.substr(6, 2));
            milliseconds = std::stof(time.substr(9, 3));
        } else {
            std::cerr << "Time string too short: " << time << std::endl;
            return;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing time string: " << time << std::endl;
        return;
    }

    double total_seconds = (minutes * 60) + seconds + (milliseconds / 1000.0);
    latitude = std::stod(tokens[2]);
    longitude = std::stod(tokens[3]);

    // Check distance from previous point if we have a previous point
    if (previous_latitude_ != 0.0 && previous_longitude_ != 0.0) {
        double distance_lat = latitude - previous_latitude_;
        double distance_lon = longitude - previous_longitude_;
        double distance_cm = std::sqrt(distance_lat * distance_lat + distance_lon * distance_lon) * 10000000; // Convert to cm
        if (distance_cm > 600) {
            std::cout << "Point ignored - too far from previous point (" << distance_cm << "cm)" << std::endl;
            return;
        }
    }

    double speed = calculate_speed(latitude, longitude, total_seconds);

    std::string mode = "record";
    double threshold = ledController_.get_max_distance_as_lat_long_deg();
    std::thread([this, mode, threshold, latitude, longitude](){ apiClient_.send_post_request(apiClient_.create_request(latitude, longitude, threshold, mode)); }).detach();

    std::lock_guard<std::mutex> lock(file_mutex);
    std::ofstream outfile("coordinates.csv", std::ios_base::app);
    if (!outfile.is_open()) {
        std::cerr << "Failed to open coordinates.csv" << std::endl;
        return;
    }
    outfile.precision(8);
    outfile << std::fixed;
    outfile << latitude << ", " << longitude  << ", " << speed << std::endl;
    outfile.close();

    std::cout << "Latitude: " << latitude << ", Longitude: " << longitude << " saved to coordinates.csv" << std::endl;
    previous_latitude_ = latitude;
    previous_longitude_ = longitude;
    previous_time_ = total_seconds;
}

void States::run_play_function(const char* content) {
    // Extract the latitude and longitude values
    float speed_scale_factor = 1.0;
    is_recording_ = false;
    std::istringstream iss(content);
    std::vector<std::string> tokens;
    std::string token;
    int stats, fix;
    double latitude, longitude;

    while (iss >> token) {
        tokens.push_back(token);
    }
    if (!(tokens.size() > 13)) {
        std::cerr << "Invalid data format" << std::endl;
        return;
    }

    if (!is_playing_)
    {
        previous_latitude_ = 0.0;
        previous_longitude_ = 0.0;
        previous_time_ = 0.0;
        is_playing_ = true;
    }

    std::string time = tokens[1];
    float minutes = 0;
    float seconds = 0;
    float milliseconds = 0;
    try {
        if (time.length() >= 12) {  // Ensure string is long enough
            minutes = std::stof(time.substr(3, 2));  // extract "33" from "03:33:48.700"
            seconds = std::stof(time.substr(6, 2));  // extract "48" from "03:33:48.700"
            milliseconds = std::stof(time.substr(9, 3));  // extract "700" from "03:33:48.700"
        } else {
            std::cerr << "Time string too short: " << time << std::endl;
            return;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing time string: " << time << std::endl;
        return;
    }
    double total_seconds = (minutes * 60) + seconds + (milliseconds / 1000.0);
    std::cout << "Total seconds: " << total_seconds << std::endl;
    latitude = std::stod(tokens[2]);
    longitude = std::stod(tokens[3]);
    double speed = calculate_speed(latitude, longitude, total_seconds);

    if (!track_loaded_) {
        std::cout << "Loading track from coordinates.csv" << std::endl;
        track_loaded_ = boundaryLogic_.load_track("coordinates.csv");
    }

    // Calculate the distance
    try {
        if(track_loaded_){
            // Calculate next predicted position based on current velocity vector
            double lat_velocity = latitude - previous_latitude_;
            double lon_velocity = longitude - previous_longitude_;
            double vector_magnitude = sqrt(lat_velocity * lat_velocity + lon_velocity * lon_velocity);
            if (vector_magnitude <= 0)
            {
                std::cout << "no movement since last vector, skipping" << std::endl;
                return;
            }
            double normalized_lat = lat_velocity / vector_magnitude;
            double normalized_lon = lon_velocity / vector_magnitude;
            // Convert speed from m/s to degrees/s (approximately 1 degree = 111000 meters)
            double speed_in_degrees = speed / 111000.0;
            double next_lat = latitude + (normalized_lat * speed_in_degrees * speed_scale_factor);
            double next_lon = longitude + (normalized_lon * speed_in_degrees * speed_scale_factor);
            std::cout << "Next predicted position: " << next_lat << ", " << next_lon << std::endl;
            double distance = boundaryLogic_.calculate_distance(next_lat, next_lon);
            double previous_speed = boundaryLogic_.get_speed_at_nearest_point(next_lat, next_lon);
            std::cout << "Speed at nearest point (m/s): " << previous_speed << std::endl;
            std::cout << "Distance from track (cm): " << distance << std::endl;
            ws2811_led_t color = previous_color_;
            if (previous_speed > 0) {
                color = ledController_.get_interpolated_breaking_color(speed, previous_speed, Color::GREEN, Color::YELLOW, Color::RED);
                previous_color_ = color;
            }
            ledController_.set_led_location(-distance, color, 3);
            std::string mode = "play";
            double threshold = ledController_.get_max_distance_as_lat_long_deg();
            std::thread([this, mode, threshold, latitude, longitude](){ apiClient_.send_post_request(apiClient_.create_request(latitude, longitude, threshold, mode)); }).detach();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error calculating distance: " << e.what() << std::endl;
    }
    previous_latitude_ = latitude;
    previous_longitude_ = longitude;
    previous_time_ = total_seconds;
}

void States::run_reset_function() {
    std::lock_guard<std::mutex> lock(file_mutex);
    is_recording_ = false;
    is_recording_ = false;
    if (remove("coordinates.csv") != 0) {
        std::cout << "Could not delete coordinates.csv" << std::endl;
    } else {
        std::cout << "coordinates.csv successfully deleted" << std::endl;
    }
}

void States::run_standby_function() {
    is_recording_ = false;
    is_playing_ = false;
    std::cout << "Running standby function." << std::endl;
    std::string mode = "standby";
    double threshold = ledController_.get_max_distance_as_lat_long_deg();
    std::thread([this, mode, threshold](){ apiClient_.send_post_request(apiClient_.create_request(0.0, 0.0, threshold, mode)); }).detach();
    previous_latitude_ = 0.0;
    previous_longitude_ = 0.0;
    previous_time_ = 0.0;
    ledController_.clear();
}

double States::calculate_speed(double latitude, double longitude, double current_time) {
    // Calculate the speed based on the previous and current positions
    if (previous_time_ != 0.0 && previous_latitude_ != 0.0 && previous_longitude_ != 0.0) {
        double distance_lat = latitude - previous_latitude_;
        double distance_lon = longitude - previous_longitude_;
        double distance = std::sqrt(distance_lat * distance_lat + distance_lon * distance_lon) * 100000; // Convert to meters
        double time_diff = current_time - previous_time_;
        if (time_diff > 0) {
            double speed = distance / time_diff;
            double speed_mph = speed * 2.23694;
            std::cout << "Speed (m/s): " << speed << ", Speed (mph): " << speed_mph << std::endl;
            return speed; // speed in m/s
        }
    }
    std::cout << "No previous data to calculate speed." << std::endl;
    return 0.0;
}