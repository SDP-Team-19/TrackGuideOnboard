// filepath: /Users/mitchellsylvia/TrackGuideOnboard/src/boundarylogic/boundarylogic.cpp
#include "boundarylogic.h"

using namespace std;
using namespace nanoflann;

// Define a simple 2D point structure
struct Point2D {
    double x, y;
};

// Create a point cloud structure for nanoflann
struct PointCloud {
    vector<Point2D> points;

    // Must provide a way to access the point data for nanoflann
    inline size_t kdtree_get_point_count() const { return points.size(); }

    inline double kdtree_get_pt(const size_t idx, int dim) const {
        return (dim == 0) ? points[idx].x : points[idx].y;
    }

    // Optional bounding box (not needed for most cases)
    template <class BBOX>
    bool kdtree_get_bbox(BBOX&) const { return false; }
};

typedef KDTreeSingleIndexAdaptor<
    L2_Simple_Adaptor<double, PointCloud>,
    PointCloud, 
    2  // 2D points
> KDTree;

// Constructor that initializes max_distance
BoundaryLogic::BoundaryLogic(double threshold) : _threshold(threshold) {}

// Default constructor
BoundaryLogic::BoundaryLogic() : _threshold(0.0f) {}

// Function to calculate distance given latitude and longitude
double BoundaryLogic::calculate_distance(double latitude, double longitude) {
    KNNResultSet<double> resultSet(1);
    Point2D userPos = {latitude, longitude};  // User's position
    double queryPt[2] = {userPos.x, userPos.y};
    size_t nearestIdx;
    double outDistSqr;

    if (recorded_path.empty()) {
        throw std::runtime_error("Recorded path is empty.");
    }

    KNNResultSet<double> resultSet(1);
    resultSet.init(&nearestIdx, &outDistSqr);
    SearchParameters params;
    params.sorted = false;
    params.eps = 0.0;
    _kdtree_ptr->findNeighbors(resultSet, queryPt, params);

    size_t nextIdx = (nearestIdx < _point_cloud_ptr->points.size() - 1) ? nearestIdx + 1 : nearestIdx - 1;

    double signedPerpDist = computeSignedPerpendicularDistance(userPos, _point_cloud_ptr->points[nearestIdx], _point_cloud_ptr->points[nextIdx]);

    return signedPerpDist;
}

// Function to load track from a saved CSV file
void BoundaryLogic::load_track(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + file_path);
    }

    PointCloud point_cloud;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string lat_str, lon_str;
        if (std::getline(ss, lat_str, ',') && std::getline(ss, lon_str, ',')) {
            double latitude = std::stof(lat_str);
            double longitude = std::stof(lon_str);
            recorded_path.emplace_back(latitude, longitude);
            _point_cloud_ptr->points.push_back({latitude, longitude});
        }
    }
    file.close();

    // Load the KDTree into the private variable kdtree
    _kdtree_ptr = std::make_unique<KDTree>(2, point_cloud, KDTreeSingleIndexAdaptorParams(10 /* max leaf */));
    _kdtree_ptr->buildIndex();
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