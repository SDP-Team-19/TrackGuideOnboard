// filepath: /Users/mitchellsylvia/TrackGuideOnboard/src/boundarylogic/boundarylogic.cpp
#include "boundarylogic.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;
using namespace nanoflann;

// Constructor that initializes max_distance
BoundaryLogic::BoundaryLogic(double threshold) : _threshold(threshold), _point_cloud_ptr(std::make_unique<PointCloud>()), 
_kdtree_ptr(std::make_unique<KDTree>(2, *_point_cloud_ptr)) {
    std::cout << "BoundaryLogic initialized with threshold: " << threshold << std::endl;
}

// Default constructor
BoundaryLogic::BoundaryLogic() : _threshold(0.0f) {}

// Function to calculate distance given latitude and longitude
double BoundaryLogic::calculate_distance(double latitude, double longitude) {
    // std::cout << "Locking mutex" << std::endl;
    // std::lock_guard<std::mutex> lock(kdtree_mutex);  // Lock the mutex to ensure thread safety

    KNNResultSet<double> resultSet(1);
    Point2D userPos = {latitude, longitude};  // User's position
    double queryPt[2] = {userPos.x, userPos.y};
    size_t nearestIdx;
    double outDistSqr;
    std::cout << "initialized calc variables" << std::endl;

    if (recorded_path.empty()) {
        throw std::runtime_error("Recorded path is empty.");
    }

    resultSet.init(&nearestIdx, &outDistSqr);
    std::cout << "initialized result set" << std::endl;
    SearchParameters params;
    params.sorted = false;
    params.eps = 0.0;
    std::cout << "finding neighbors" << std::endl;
    _kdtree_ptr->findNeighbors(resultSet, queryPt, params);
    std::cout << "neighbors found" << std::endl;

    size_t nextIdx = (nearestIdx < _point_cloud_ptr->points.size() - 1) ? nearestIdx + 1 : nearestIdx - 1;
    std::cout << "next index found" << std::endl;

    std::cout << "computing signed perp distance" << std::endl;
    double signedPerpDist = computeSignedPerpendicularDistance(userPos, _point_cloud_ptr->points[nearestIdx], _point_cloud_ptr->points[nextIdx]);

    std::cout << "distance found" << std::endl;
    return signedPerpDist;
}

// Function to load track from a saved CSV file
void BoundaryLogic::load_track(const std::string& file_path) {
    // std::cout << "Locking mutex for loading track" << std::endl;
    // std::lock_guard<std::mutex> lock(kdtree_mutex);  // Lock the mutex to ensure thread safety

    std::cout << "Loading track from file: " << file_path << std::endl;
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + file_path);
    }

    std::cout << "Clearning variables" << std::endl;

    // Ensure _kdtree_ptr is properly initialized
    if (!_kdtree_ptr) {
        std::cerr << "_kdtree_ptr is not initialized." << std::endl;
        // Additional handling or initialization if necessary
    }

    PointCloud point_cloud;
    std::string line;
    std::cout << "point cloud initi" << std::endl;
    bool loop_entered = false;
    while (std::getline(file, line)) {
        loop_entered = true;
        std::istringstream ss(line);
        std::string lat_str, lon_str;
        std::cout << "Reading line: " << line << std::endl;
        if (std::getline(ss, lat_str, ',') && std::getline(ss, lon_str, ',')) {
            std::cout << "Read Latitude: " << lat_str << ", Longitude: " << lon_str << std::endl;
            double latitude, longitude;
            try {
                double latitude = std::stod(lat_str);
                double longitude = std::stod(lon_str);
                // Additional validation if necessary
                recorded_path.emplace_back(latitude, longitude);
                point_cloud.points.push_back({latitude, longitude});
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid number format: " << e.what() << std::endl;
                continue; // Skip to the next line or handle the error as needed
            }
        }
    }

    if (!loop_entered) {
        std::cout << "No lines read from the file." << std::endl;
    }

    file.close();

    if (point_cloud.points.empty()) {
        std::cerr << "Point cloud is empty. Cannot build KD-Tree." << std::endl;
        return;
    }

    // Load the KDTree into the private variable kdtree
    _kdtree_ptr = std::make_unique<KDTree>(2, point_cloud, KDTreeSingleIndexAdaptorParams(10 /* max leaf */));
    _kdtree_ptr->buildIndex();
    std::cout << "Track loaded successfully" << std::endl;
}

double BoundaryLogic::computeSignedPerpendicularDistance(const Point2D& userPos, const Point2D& closest, const Point2D& next) {
    // 1. Compute the heading vector (direction of the track at this point)
    double dx = next.x - closest.x;
    double dy = next.y - closest.y;
    
    // 2. Compute the normal vector (perpendicular to the heading)
    double normX = -dy;
    double normY = dx;
    
    // Normalize the normal vector
    double normLength = sqrt(normX * normX + normY * normY);
    normX /= normLength;
    normY /= normLength;

    // 3. Compute the vector from the closest track point to the user
    double diffX = userPos.x - closest.x;
    double diffY = userPos.y - closest.y;

    // 4. Compute the signed perpendicular distance (dot product with normal)
    double signedPerpDist = (diffX * normX + diffY * normY);

    return signedPerpDist;
}