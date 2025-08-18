/**
 * @file Graph.hpp
 * @brief Graph representation for TSP with symmetric distance matrix
 * 
 * This class represents a complete undirected graph for the Traveling Salesman Problem.
 * All distances are symmetric (distance(i,j) == distance(j,i)) and non-negative.
 * Supports accurate TSPLIB distance calculations through TDD-based distance calculators.
 */

#pragma once

#include <vector>
#include <random>
#include <memory>
#include <string>
#include "DistanceCalculator.hpp"

class Graph {
private:
    std::vector<std::vector<double>> distances_;
    int size_;
    std::shared_ptr<DistanceCalculator> distance_calculator_;
    std::string distance_type_;
    
    static constexpr double MIN_DISTANCE = 1.0;
    static constexpr double MAX_DISTANCE = 100.0;

public:
    /**
     * @brief Construct a new Graph with the specified number of cities
     * @param size Number of cities (must be positive)
     * @throws std::invalid_argument if size <= 0
     */
    explicit Graph(int size);
    
    /**
     * @brief Get distance between two cities
     * @param from Source city index
     * @param to Destination city index  
     * @return Distance between the cities
     * @throws std::out_of_range if city indices are invalid
     */
    double getDistance(int from, int to) const;
    
    /**
     * @brief Get number of cities in the graph
     * @return Number of cities
     */
    int size() const;
    
    /**
     * @brief Initialize graph with random symmetric distances
     * @param seed Random seed for reproducible results (default: 42)
     */
    void initializeRandomSymmetric(unsigned int seed = 42);
    
    /**
     * @brief Set distance between two cities (for testing purposes)
     * @param from Source city
     * @param to Destination city
     * @param distance Distance value to set
     */
    void setDistance(int from, int to, double distance);
    
    /**
     * @brief Create a Graph from a TSP file with TDD-based distance calculation
     * @param filename Path to the TSP file
     * @return Shared pointer to the created Graph
     * @throws std::runtime_error if file cannot be read or parsed
     */
    static std::shared_ptr<Graph> fromTSPFile(const std::string& filename);
    
    /**
     * @brief Get the distance calculation type used by this graph
     * @return Distance type string (EUC_2D, CEIL_2D, ATT, GEO)
     */
    std::string getDistanceType() const;
    
    /**
     * @brief Get the distance calculator used by this graph
     * @return Shared pointer to distance calculator
     */
    std::shared_ptr<DistanceCalculator> getDistanceCalculator() const;
    
private:
    /**
     * @brief Calculate Euclidean distance between two points
     * @param x1, y1 Coordinates of first point
     * @param x2, y2 Coordinates of second point
     * @return Euclidean distance
     */
    static double calculateEuclideanDistance(double x1, double y1, double x2, double y2);
};
