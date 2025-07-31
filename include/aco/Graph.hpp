/**
 * @file Graph.hpp
 * @brief Graph representation for TSP with symmetric distance matrix
 * 
 * This class represents a complete undirected graph for the Traveling Salesman Problem.
 * All distances are symmetric (distance(i,j) == distance(j,i)) and non-negative.
 */

#pragma once

#include <vector>
#include <random>

class Graph {
private:
    std::vector<std::vector<double>> distances_;
    int size_;
    
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
};
