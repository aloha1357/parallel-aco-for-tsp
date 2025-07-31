#pragma once

#include "Graph.hpp"
#include <memory>
#include <string>
#include <random>

/**
 * @brief Utility class for generating synthetic TSP instances for testing
 */
class SyntheticTSPGenerator {
public:
    /**
     * @brief Generate a random TSP instance with cities in a square area
     * @param num_cities Number of cities to generate
     * @param area_size Size of the square area (width and height)
     * @param seed Random seed for reproducibility
     * @return Shared pointer to generated graph
     */
    static std::shared_ptr<Graph> generateRandomInstance(int num_cities, double area_size = 1000.0, 
                                                        int seed = 42);
    
    /**
     * @brief Generate a clustered TSP instance with cities grouped in clusters
     * @param num_cities Number of cities to generate
     * @param num_clusters Number of clusters to create
     * @param cluster_radius Radius of each cluster
     * @param area_size Size of the overall area
     * @param seed Random seed for reproducibility
     * @return Shared pointer to generated graph
     */
    static std::shared_ptr<Graph> generateClusteredInstance(int num_cities, int num_clusters,
                                                           double cluster_radius = 50.0,
                                                           double area_size = 1000.0, int seed = 42);
    
    /**
     * @brief Generate a circular TSP instance with cities arranged on concentric circles
     * @param num_cities Number of cities to generate
     * @param num_rings Number of concentric circles
     * @param max_radius Radius of the outermost circle
     * @param seed Random seed for reproducibility
     * @return Shared pointer to generated graph
     */
    static std::shared_ptr<Graph> generateCircularInstance(int num_cities, int num_rings = 3,
                                                          double max_radius = 500.0, int seed = 42);
    
    /**
     * @brief Generate a grid-based TSP instance with cities on a rectangular grid
     * @param grid_width Number of cities in width direction
     * @param grid_height Number of cities in height direction
     * @param spacing Distance between adjacent grid points
     * @param noise_level Amount of random noise to add to positions (0.0 = perfect grid)
     * @param seed Random seed for reproducibility
     * @return Shared pointer to generated graph
     */
    static std::shared_ptr<Graph> generateGridInstance(int grid_width, int grid_height,
                                                      double spacing = 50.0, double noise_level = 0.0,
                                                      int seed = 42);
    
    /**
     * @brief Save a generated TSP instance to a TSPLIB format file
     * @param graph Graph to save
     * @param filename Output filename
     * @param instance_name Name for the TSP instance
     * @param coordinates Coordinate pairs for each city (x, y)
     */
    static void saveToTSPFile(std::shared_ptr<Graph> graph, const std::string& filename,
                             const std::string& instance_name,
                             const std::vector<std::pair<double, double>>& coordinates);
    
    /**
     * @brief Create a performance testing suite with instances of various sizes
     * @param base_filename Base name for generated files (will add size suffix)
     * @param sizes Vector of instance sizes to generate
     * @param seed Random seed for reproducibility
     */
    static void createPerformanceTestSuite(const std::string& base_filename,
                                          const std::vector<int>& sizes = {100, 200, 442, 1000},
                                          int seed = 42);

private:
    /**
     * @brief Calculate Euclidean distance between two points
     * @param x1, y1 Coordinates of first point
     * @param x2, y2 Coordinates of second point
     * @return Distance between the points
     */
    static double calculateDistance(double x1, double y1, double x2, double y2);
    
    /**
     * @brief Populate distance matrix from coordinate pairs
     * @param graph Graph to populate
     * @param coordinates City coordinates
     */
    static void populateDistanceMatrix(std::shared_ptr<Graph> graph, 
                                      const std::vector<std::pair<double, double>>& coordinates);
};
