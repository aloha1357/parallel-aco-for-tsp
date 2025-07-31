/**
 * @file PheromoneModel.hpp
 * @brief Pheromone matrix management for ACO algorithm
 * 
 * This class manages the pheromone matrix τ[i][j] which represents
 * the pheromone level on the edge between cities i and j.
 */

#pragma once

#include <vector>

class PheromoneModel {
private:
    std::vector<std::vector<double>> pheromone_;
    int size_;
    
    static constexpr double DEFAULT_PHEROMONE = 1.0;
    static constexpr double MIN_PHEROMONE = 0.01;

public:
    /**
     * @brief Construct pheromone model for given number of cities
     * @param size Number of cities
     */
    explicit PheromoneModel(int size);
    
    /**
     * @brief Get pheromone level between two cities
     * @param from Source city
     * @param to Destination city
     * @return Pheromone level τ[from][to]
     */
    double getPheromone(int from, int to) const;
    
    /**
     * @brief Set pheromone level between two cities
     * @param from Source city
     * @param to Destination city
     * @param value Pheromone level to set
     */
    void setPheromone(int from, int to, double value);
    
    /**
     * @brief Initialize all pheromone levels to default value
     * @param value Initial pheromone level (default: 1.0)
     */
    void initialize(double value = DEFAULT_PHEROMONE);
    
    /**
     * @brief Get number of cities
     * @return Size of the pheromone matrix
     */
    int size() const;
};
