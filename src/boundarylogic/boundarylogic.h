#ifndef BOUNDARYLOGIC_H
#define BOUNDARYLOGIC_H

#include <vector>
#include <tuple>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <stdexcept>
#include <vector>
#include <nanoflann.hpp>

class BoundaryLogic {
private:
    double _threshold;
    std::vector<std::tuple<double, double>> recorded_path;
    std::unique_ptr<KDTree> _kdtree_ptr;
    std::unique_ptr<PointCloud> _point_cloud_ptr;
    double computeSignedPerpendicularDistance(const Point2D& userPos, const Point2D& closest, const Point2D& next);

public:
    // Constructor that initializes max_distance
    BoundaryLogic(double threshold);

    // Default constructor
    BoundaryLogic();

    /*  Function to calculate distance given latitude and longitude, 
        using KD-tree and calculating the distance perpendicular to the heading
    */
    double calculate_distance(double latitude, double longitude);

    // Function to load track from a saved CSV file
    void load_track(const std::string& file_path);
};

#endif // BOUNDARYLOGIC_H