#include "aco/Graph.hpp"
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <memory>
#include <functional>
#include <iostream>

// TSPLIB distance calculation functions
namespace {
    // EUC_2D: Euclidean distance rounded to nearest integer
    inline int dist_euc_2d(double x1, double y1, double x2, double y2) {
        double d = std::hypot(x2 - x1, y2 - y1);
        return static_cast<int>(std::lround(d));
    }
    
    // CEIL_2D: Euclidean distance rounded up
    inline int dist_ceil_2d(double x1, double y1, double x2, double y2) {
        double d = std::hypot(x2 - x1, y2 - y1);
        return static_cast<int>(std::ceil(d));
    }
    
    // ATT: Pseudo-Euclidean distance (for kroA series)
    inline int dist_att(double x1, double y1, double x2, double y2) {
        double rij = std::sqrt(((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)) / 10.0);
        int tij = static_cast<int>(std::lround(rij));
        return (tij < rij) ? (tij + 1) : tij;
    }
    
    // GEO: Geographical distance (for some instances)
    inline int dist_geo(double x1, double y1, double x2, double y2) {
        // Convert to radians and calculate geographical distance
        // This is more complex, implement if needed for specific instances
        double d = std::hypot(x2 - x1, y2 - y1);
        return static_cast<int>(std::lround(d));
    }
}

Graph::Graph(int size) : size_(size) {
    if (size <= 0) {
        throw std::invalid_argument("Graph size must be positive");
    }
    
    // Initialize distance matrix
    distances_.resize(size_);
    for (int i = 0; i < size_; ++i) {
        distances_[i].resize(size_);
    }
    
    // Initialize with random symmetric distances
    initializeRandomSymmetric();
}

double Graph::getDistance(int from, int to) const {
    if (from < 0 || from >= size_ || to < 0 || to >= size_) {
        throw std::out_of_range("City index out of range");
    }
    return distances_[from][to];
}

int Graph::size() const {
    return size_;
}

void Graph::setDistance(int from, int to, double distance) {
    if (from < 0 || from >= size_ || to < 0 || to >= size_) {
        throw std::out_of_range("City index out of range");
    }
    distances_[from][to] = distance;
    distances_[to][from] = distance; // Maintain symmetry
}

void Graph::initializeRandomSymmetric(unsigned int seed) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<double> dist(MIN_DISTANCE, MAX_DISTANCE);
    
    // Initialize diagonal to 0 (distance from city to itself)
    for (int i = 0; i < size_; ++i) {
        distances_[i][i] = 0.0;
    }
    
    // Initialize upper triangle with random values and copy to lower triangle for symmetry
    for (int i = 0; i < size_; ++i) {
        for (int j = i + 1; j < size_; ++j) {
            double distance = dist(rng);
            distances_[i][j] = distance;
            distances_[j][i] = distance; // Ensure symmetry
        }
    }
}

std::shared_ptr<Graph> Graph::fromTSPFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open TSP file: " + filename);
    }
    
    std::string line;
    int dimension = 0;
    std::string edge_weight_type;
    std::vector<std::pair<double, double>> coordinates;
    
    // Parse header
    while (std::getline(file, line)) {
        if (line.find("DIMENSION") != std::string::npos) {
            std::istringstream iss(line);
            std::string key, colon;
            iss >> key >> colon >> dimension;
        } else if (line.find("EDGE_WEIGHT_TYPE") != std::string::npos) {
            std::istringstream iss(line);
            std::string key, colon;
            iss >> key >> colon >> edge_weight_type;
        } else if (line.find("NODE_COORD_SECTION") != std::string::npos) {
            break;
        }
    }
    
    if (dimension <= 0) {
        throw std::runtime_error("Invalid or missing DIMENSION in TSP file");
    }
    
    // 支持多種 TSPLIB 距離類型
    std::function<int(double,double,double,double)> distance_func;
    
    if (edge_weight_type == "EUC_2D") {
        distance_func = dist_euc_2d;
        std::cout << "Loading TSP with EUC_2D distance (rounded Euclidean)" << std::endl;
    } else if (edge_weight_type == "CEIL_2D") {
        distance_func = dist_ceil_2d;
        std::cout << "Loading TSP with CEIL_2D distance (ceiling Euclidean)" << std::endl;
    } else if (edge_weight_type == "ATT") {
        distance_func = dist_att;
        std::cout << "Loading TSP with ATT distance (pseudo-Euclidean)" << std::endl;
    } else if (edge_weight_type == "GEO") {
        distance_func = dist_geo;
        std::cout << "Loading TSP with GEO distance (geographical)" << std::endl;
    } else {
        throw std::runtime_error("Unsupported EDGE_WEIGHT_TYPE: " + edge_weight_type + 
                                 " (supported: EUC_2D, CEIL_2D, ATT, GEO)");
    }
    
    // Parse coordinates
    coordinates.resize(dimension);
    for (int i = 0; i < dimension; ++i) {
        if (!std::getline(file, line) || line == "EOF") {
            throw std::runtime_error("Unexpected end of coordinate data");
        }
        
        std::istringstream iss(line);
        int city_id;
        double x, y;
        if (!(iss >> city_id >> x >> y)) {
            throw std::runtime_error("Invalid coordinate format at line: " + line);
        }
        
        // Store coordinates (city_id is 1-based, convert to 0-based)
        if (city_id > 0 && city_id <= dimension) {
            coordinates[city_id - 1] = {x, y};
        }
    }
    
    // Create graph and calculate distances
    auto graph = std::make_shared<Graph>(dimension);
    
    // 使用正確的 TSPLIB 距離計算函數
    std::cout << "Calculating distances using " << edge_weight_type << " metric..." << std::endl;
    
    for (int i = 0; i < dimension; ++i) {
        graph->setDistance(i, i, 0.0); // Distance to itself is 0
        for (int j = i + 1; j < dimension; ++j) {
            // 使用選定的距離函數計算整數距離，然後轉為 double
            int distance = distance_func(
                coordinates[i].first, coordinates[i].second,
                coordinates[j].first, coordinates[j].second
            );
            graph->setDistance(i, j, static_cast<double>(distance));
            graph->setDistance(j, i, static_cast<double>(distance)); // 對稱
        }
    }
    
    return graph;
}

double Graph::calculateEuclideanDistance(double x1, double y1, double x2, double y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
}
