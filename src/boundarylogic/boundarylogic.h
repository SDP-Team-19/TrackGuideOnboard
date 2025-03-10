#ifndef BOUNDARYLOGIC_H
#define BOUNDARYLOGIC_H

#include <vector>
#include <memory>
#include <mutex>
#include <nanoflann.hpp>

// Define a simple 2D point structure
struct Point2D {
    double x, y;
};

// Create a point cloud structure for nanoflann
struct PointCloud {
    std::vector<Point2D> points;

    // Must provide a way to access the point data for nanoflann
    inline size_t kdtree_get_point_count() const { return points.size(); }

    inline double kdtree_get_pt(const size_t idx, int dim) const {
        return (dim == 0) ? points[idx].x : points[idx].y;
    }

    // Optional bounding box (not needed for most cases)
    template <class BBOX>
    bool kdtree_get_bbox(BBOX&) const { return false; }
};

typedef nanoflann::KDTreeSingleIndexAdaptor<
    nanoflann::L2_Simple_Adaptor<double, PointCloud>,
    PointCloud, 
    2  // 2D points
> KDTree;

class BoundaryLogic {
public:
    // Constructors
    BoundaryLogic(double threshold);
    BoundaryLogic();

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