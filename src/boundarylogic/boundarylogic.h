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
#include <memory>
#include <mutex>

class BoundaryLogic {
public:
    // Constructors
    BoundaryLogic(double threshold);
    BoundaryLogic();

    // Delete copy constructor and copy assignment operator
    BoundaryLogic(const BoundaryLogic&) = delete;
    BoundaryLogic& operator=(const BoundaryLogic&) = delete;

    // Default move constructor and move assignment operator
    BoundaryLogic(BoundaryLogic&&) = default;
    BoundaryLogic& operator=(BoundaryLogic&&) = default;

    // Member functions
    double calculate_distance(double latitude, double longitude);
    void load_track(const std::string& file_path);

private:
    double computeSignedPerpendicularDistance(const Point2D& userPos, const Point2D& closest, const Point2D& next);

    double _threshold;
    std::vector<Point2D> recorded_path;
    std::unique_ptr<KDTree> _kdtree_ptr;
    std::unique_ptr<PointCloud> _point_cloud_ptr;
    std::mutex file_mutex;
    std::mutex kdtree_mutex;  // Mutex to protect access to the KD-tree
};

#endif // BOUNDARYLOGIC_H