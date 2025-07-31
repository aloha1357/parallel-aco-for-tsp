/**
 * @file ThreadLocalPheromoneModel.hpp
 * @brief Thread-local pheromone delta buffer for parallel ACO
 * 
 * This class provides a thread-local buffer for accumulating pheromone
 * deltas before merging to the global pheromone matrix. This enables
 * safe parallel pheromone updates without race conditions.
 */

#pragma once

#include <vector>

class ThreadLocalPheromoneModel {
private:
    std::vector<std::vector<double>> delta_pheromone_;
    int size_;

public:
    /**
     * @brief Construct thread-local pheromone buffer for given number of cities
     * @param size Number of cities
     */
    explicit ThreadLocalPheromoneModel(int size);
    
    /**
     * @brief Get delta pheromone level between two cities
     * @param from Source city
     * @param to Destination city
     * @return Delta pheromone level Δτ[from][to]
     */
    double getDelta(int from, int to) const;
    
    /**
     * @brief Set delta pheromone level between two cities
     * @param from Source city
     * @param to Destination city
     * @param value Delta pheromone level to set
     */
    void setDelta(int from, int to, double value);
    
    /**
     * @brief Add delta pheromone to an edge
     * @param from Source city
     * @param to Destination city
     * @param delta Delta pheromone to add
     */
    void addDelta(int from, int to, double delta);
    
    /**
     * @brief Accumulate pheromone delta along a tour path
     * @param tour_path Vector of city indices representing the tour path
     * @param tour_length Total length of the tour
     * @param quality Quality factor Q (typically 1/tour_length or constant)
     * @throws std::invalid_argument if tour is invalid or parameters are negative
     * @throws std::out_of_range if tour contains invalid city indices
     */
    void accumulateDelta(const std::vector<int>& tour_path, double tour_length, double quality);
    
    /**
     * @brief Reset all delta values to zero
     */
    void reset();
    
    /**
     * @brief Get number of cities
     * @return Size of the delta matrix
     */
    int size() const;
};
