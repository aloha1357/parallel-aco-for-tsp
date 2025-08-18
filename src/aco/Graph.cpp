#include "aco/Graph.hpp"
#include "aco/DistanceCalculator.hpp"
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <memory>
#include <functional>
#include <iostream>

// TDD-based distance calculation system is now in DistanceCalculator.cpp

Graph::Graph(int size) : size_(size), distance_type_("RANDOM"), distance_calculator_(nullptr) {
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
            // Find the colon and extract the number after it
            size_t colon_pos = line.find(':');
            if (colon_pos != std::string::npos) {
                std::string value_str = line.substr(colon_pos + 1);
                std::istringstream iss(value_str);
                iss >> dimension;
            }
        } else if (line.find("EDGE_WEIGHT_TYPE") != std::string::npos) {
            // Find the colon and extract the type after it
            size_t colon_pos = line.find(':');
            if (colon_pos != std::string::npos) {
                std::string value_str = line.substr(colon_pos + 1);
                std::istringstream iss(value_str);
                iss >> edge_weight_type;
            }
        } else if (line.find("NODE_COORD_SECTION") != std::string::npos) {
            break;
        }
    }
    
    if (dimension <= 0) {
        throw std::runtime_error("Invalid or missing DIMENSION in TSP file");
    }
    
    // Create TDD-based distance calculator
    auto distance_calculator = DistanceCalculatorFactory::create(edge_weight_type);
    if (!distance_calculator) {
        throw std::runtime_error("Unsupported EDGE_WEIGHT_TYPE: " + edge_weight_type + 
                                 " (supported: " + []{
                                     std::string types;
                                     auto supported = DistanceCalculatorFactory::getSupportedTypes();
                                     for (size_t i = 0; i < supported.size(); ++i) {
                                         if (i > 0) types += ", ";
                                         types += supported[i];
                                     }
                                     return types;
                                 }() + ")");
    }
    
    std::cout << "Loading TSP with " << edge_weight_type << " distance calculator" << std::endl;
    
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
    
    // Create graph and configure distance calculator
    auto graph = std::make_shared<Graph>(dimension);
    graph->distance_type_ = edge_weight_type;
    graph->distance_calculator_ = distance_calculator;
    
    // Calculate distances using TDD-based distance calculator
    std::cout << "Calculating distances using TDD-based " << edge_weight_type << " calculator..." << std::endl;
    
    for (int i = 0; i < dimension; ++i) {
        graph->setDistance(i, i, 0.0); // Distance to itself is 0
        for (int j = i + 1; j < dimension; ++j) {
            // Use TDD-based distance calculator
            double distance = distance_calculator->calculateDistance(
                coordinates[i].first, coordinates[i].second,
                coordinates[j].first, coordinates[j].second
            );
            graph->setDistance(i, j, distance);
            graph->setDistance(j, i, distance); // Symmetry
        }
    }
    
    std::cout << "Graph created successfully with " << dimension << " cities using " 
              << edge_weight_type << " distance type" << std::endl;
    
    return graph;
}

std::string Graph::getDistanceType() const {
    return distance_type_;
}

std::shared_ptr<DistanceCalculator> Graph::getDistanceCalculator() const {
    return distance_calculator_;
}
