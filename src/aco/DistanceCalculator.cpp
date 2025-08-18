#include "aco/DistanceCalculator.hpp"
#include <cmath>
#include <vector>

// EuclideanDistanceCalculator implementation
double EuclideanDistanceCalculator::calculateDistance(double x1, double y1, double x2, double y2) const {
    double d = std::hypot(x2 - x1, y2 - y1);
    return std::lround(d);
}

// CeilingDistanceCalculator implementation
double CeilingDistanceCalculator::calculateDistance(double x1, double y1, double x2, double y2) const {
    double d = std::hypot(x2 - x1, y2 - y1);
    return std::ceil(d);
}

// AttDistanceCalculator implementation
double AttDistanceCalculator::calculateDistance(double x1, double y1, double x2, double y2) const {
    double rij = std::sqrt(((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)) / 10.0);
    int tij = static_cast<int>(std::lround(rij));
    return (tij < rij) ? (tij + 1) : tij;
}

// GeographicalDistanceCalculator implementation
double GeographicalDistanceCalculator::calculateDistance(double x1, double y1, double x2, double y2) const {
    static const double PI = 3.141592653589793;
    static const double RRR = 6378.388; // Earth radius in kilometers
    
    // Convert to radians
    double deg = static_cast<int>(x1);
    double min = x1 - deg;
    double latitude_i = PI * (deg + 5.0 * min / 3.0) / 180.0;
    
    deg = static_cast<int>(y1);
    min = y1 - deg;
    double longitude_i = PI * (deg + 5.0 * min / 3.0) / 180.0;
    
    deg = static_cast<int>(x2);
    min = x2 - deg;
    double latitude_j = PI * (deg + 5.0 * min / 3.0) / 180.0;
    
    deg = static_cast<int>(y2);
    min = y2 - deg;
    double longitude_j = PI * (deg + 5.0 * min / 3.0) / 180.0;
    
    // Calculate geographical distance
    double q1 = std::cos(longitude_i - longitude_j);
    double q2 = std::cos(latitude_i - latitude_j);
    double q3 = std::cos(latitude_i + latitude_j);
    
    double distance = RRR * std::acos(0.5 * ((1.0 + q1) * q2 - (1.0 - q1) * q3)) + 1.0;
    
    return static_cast<int>(distance);
}

// DistanceCalculatorFactory implementation
std::shared_ptr<DistanceCalculator> DistanceCalculatorFactory::create(const std::string& type) {
    if (type == "EUC_2D") {
        return std::make_shared<EuclideanDistanceCalculator>();
    } else if (type == "CEIL_2D") {
        return std::make_shared<CeilingDistanceCalculator>();
    } else if (type == "ATT") {
        return std::make_shared<AttDistanceCalculator>();
    } else if (type == "GEO") {
        return std::make_shared<GeographicalDistanceCalculator>();
    }
    return nullptr;
}

std::vector<std::string> DistanceCalculatorFactory::getSupportedTypes() {
    return {"EUC_2D", "CEIL_2D", "ATT", "GEO"};
}
