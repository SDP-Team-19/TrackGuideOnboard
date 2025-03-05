#ifndef BOUNDARYLOGIC_H
#define BOUNDARYLOGIC_H

#include <vector>
#include <tuple>
#include <string>

class RecordBoundaryLogic {
private:
    float max_distance;
    std::vector<std::tuple<float, float>> recorded_path;

public:
    // Constructor that initializes max_distance
    RecordBoundaryLogic(float max_distance);

    // Default constructor
    RecordBoundaryLogic();

    /*  Function to calculate distance given latitude and longitude, 
        using KD-tree and calculating the distance perpendicular to the heading
    */
    float calculate_distance(float latitude, float longitude);

    // Function to load track from a saved CSV file
    void load_track(const std::string& file_path);
};

#endif // BOUNDARYLOGIC_H