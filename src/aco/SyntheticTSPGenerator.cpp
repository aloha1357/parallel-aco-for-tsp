#include "aco/SyntheticTSPGenerator.hpp"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <stdexcept>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

std::shared_ptr<Graph> SyntheticTSPGenerator::generateRandomInstance(int num_cities, double area_size, int seed) {
    if (num_cities <= 0) {
        throw std::invalid_argument("Number of cities must be positive");
    }
    
    std::mt19937 rng(seed);
    std::uniform_real_distribution<double> coord_dist(0.0, area_size);
    
    std::vector<std::pair<double, double>> coordinates;
    coordinates.reserve(num_cities);
    
    // Generate random coordinates
    for (int i = 0; i < num_cities; ++i) {
        double x = coord_dist(rng);
        double y = coord_dist(rng);
        coordinates.emplace_back(x, y);
    }
    
    // Create graph and populate distance matrix
    auto graph = std::make_shared<Graph>(num_cities);
    populateDistanceMatrix(graph, coordinates);
    
    return graph;
}

std::shared_ptr<Graph> SyntheticTSPGenerator::generateClusteredInstance(int num_cities, int num_clusters,
                                                                       double cluster_radius, double area_size, int seed) {
    if (num_cities <= 0 || num_clusters <= 0) {
        throw std::invalid_argument("Number of cities and clusters must be positive");
    }
    
    std::mt19937 rng(seed);
    std::uniform_real_distribution<double> coord_dist(cluster_radius, area_size - cluster_radius);
    std::uniform_real_distribution<double> cluster_dist(-cluster_radius, cluster_radius);
    std::uniform_int_distribution<int> cluster_assignment(0, num_clusters - 1);
    
    // Generate cluster centers
    std::vector<std::pair<double, double>> cluster_centers;
    for (int i = 0; i < num_clusters; ++i) {
        double center_x = coord_dist(rng);
        double center_y = coord_dist(rng);
        cluster_centers.emplace_back(center_x, center_y);
    }
    
    std::vector<std::pair<double, double>> coordinates;
    coordinates.reserve(num_cities);
    
    // Assign cities to clusters and generate coordinates
    for (int i = 0; i < num_cities; ++i) {
        int cluster_id = cluster_assignment(rng);
        double offset_x = cluster_dist(rng);
        double offset_y = cluster_dist(rng);
        
        double x = cluster_centers[cluster_id].first + offset_x;
        double y = cluster_centers[cluster_id].second + offset_y;
        
        // Clamp to area bounds
        x = std::max(0.0, std::min(area_size, x));
        y = std::max(0.0, std::min(area_size, y));
        
        coordinates.emplace_back(x, y);
    }
    
    auto graph = std::make_shared<Graph>(num_cities);
    populateDistanceMatrix(graph, coordinates);
    
    return graph;
}

std::shared_ptr<Graph> SyntheticTSPGenerator::generateCircularInstance(int num_cities, int num_rings,
                                                                      double max_radius, int seed) {
    if (num_cities <= 0 || num_rings <= 0) {
        throw std::invalid_argument("Number of cities and rings must be positive");
    }
    
    std::mt19937 rng(seed);
    std::uniform_real_distribution<double> angle_dist(0.0, 2.0 * M_PI);
    std::uniform_int_distribution<int> ring_assignment(0, num_rings - 1);
    
    std::vector<std::pair<double, double>> coordinates;
    coordinates.reserve(num_cities);
    
    double center_x = max_radius;
    double center_y = max_radius;
    
    // Generate cities on concentric rings
    for (int i = 0; i < num_cities; ++i) {
        int ring = ring_assignment(rng);
        double radius = max_radius * (ring + 1.0) / num_rings;
        double angle = angle_dist(rng);
        
        double x = center_x + radius * std::cos(angle);
        double y = center_y + radius * std::sin(angle);
        
        coordinates.emplace_back(x, y);
    }
    
    auto graph = std::make_shared<Graph>(num_cities);
    populateDistanceMatrix(graph, coordinates);
    
    return graph;
}

std::shared_ptr<Graph> SyntheticTSPGenerator::generateGridInstance(int grid_width, int grid_height,
                                                                  double spacing, double noise_level, int seed) {
    if (grid_width <= 0 || grid_height <= 0) {
        throw std::invalid_argument("Grid dimensions must be positive");
    }
    
    int num_cities = grid_width * grid_height;
    std::mt19937 rng(seed);
    std::uniform_real_distribution<double> noise_dist(-noise_level * spacing, noise_level * spacing);
    
    std::vector<std::pair<double, double>> coordinates;
    coordinates.reserve(num_cities);
    
    // Generate grid coordinates with optional noise
    for (int row = 0; row < grid_height; ++row) {
        for (int col = 0; col < grid_width; ++col) {
            double x = col * spacing + (noise_level > 0.0 ? noise_dist(rng) : 0.0);
            double y = row * spacing + (noise_level > 0.0 ? noise_dist(rng) : 0.0);
            coordinates.emplace_back(x, y);
        }
    }
    
    auto graph = std::make_shared<Graph>(num_cities);
    populateDistanceMatrix(graph, coordinates);
    
    return graph;
}

void SyntheticTSPGenerator::saveToTSPFile(std::shared_ptr<Graph> graph, const std::string& filename,
                                         const std::string& instance_name,
                                         const std::vector<std::pair<double, double>>& coordinates) {
    if (!graph) {
        throw std::invalid_argument("Graph cannot be null");
    }
    
    if (coordinates.size() != static_cast<size_t>(graph->size())) {
        throw std::invalid_argument("Coordinates size must match graph size");
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing: " + filename);
    }
    
    // Write TSPLIB header
    file << "NAME: " << instance_name << "\n";
    file << "TYPE: TSP\n";
    file << "COMMENT: Synthetic TSP instance\n";
    file << "DIMENSION: " << graph->size() << "\n";
    file << "EDGE_WEIGHT_TYPE: EUC_2D\n";
    file << "NODE_COORD_SECTION\n";
    
    // Write coordinates
    for (size_t i = 0; i < coordinates.size(); ++i) {
        file << (i + 1) << " " << std::fixed << std::setprecision(2)
             << coordinates[i].first << " " << coordinates[i].second << "\n";
    }
    
    file << "EOF\n";
    file.close();
}

void SyntheticTSPGenerator::createPerformanceTestSuite(const std::string& base_filename,
                                                      const std::vector<int>& sizes, int seed) {
    for (int size : sizes) {
        // Generate random instance
        auto graph = generateRandomInstance(size, 1000.0, seed + size);
        
        // Create coordinates for saving
        std::mt19937 rng(seed + size);
        std::uniform_real_distribution<double> coord_dist(0.0, 1000.0);
        
        std::vector<std::pair<double, double>> coordinates;
        coordinates.reserve(size);
        
        for (int i = 0; i < size; ++i) {
            coordinates.emplace_back(coord_dist(rng), coord_dist(rng));
        }
        
        // Save to file
        std::string filename = base_filename + "_" + std::to_string(size) + ".tsp";
        std::string instance_name = "synthetic_" + std::to_string(size);
        
        saveToTSPFile(graph, filename, instance_name, coordinates);
    }
}

// Private helper methods
double SyntheticTSPGenerator::calculateDistance(double x1, double y1, double x2, double y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
}

void SyntheticTSPGenerator::populateDistanceMatrix(std::shared_ptr<Graph> graph,
                                                  const std::vector<std::pair<double, double>>& coordinates) {
    int size = graph->size();
    
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (i == j) {
                graph->setDistance(i, j, 0.0);
            } else {
                double distance = calculateDistance(
                    coordinates[i].first, coordinates[i].second,
                    coordinates[j].first, coordinates[j].second
                );
                graph->setDistance(i, j, distance);
            }
        }
    }
}
