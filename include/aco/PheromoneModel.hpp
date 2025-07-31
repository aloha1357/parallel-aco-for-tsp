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
     * @brief Apply pheromone evaporation: τ(i,j) ← (1-ρ) · τ(i,j)
     * @param rho Evaporation rate (0.0 ≤ ρ ≤ 1.0)
     * @throws std::invalid_argument if rho is out of valid range
     */
    void evaporate(double rho);
    
    /**
     * @brief Deposit pheromone along a tour path: τ(i,j) ← τ(i,j) + Δτ
     * @param tour_path Vector of city indices representing the tour path
     * @param tour_length Total length of the tour
     * @param quality Quality factor Q (typically 1/tour_length or constant)
     * @throws std::invalid_argument if tour is invalid or parameters are negative
     * @throws std::out_of_range if tour contains invalid city indices
     */
    void deposit(const std::vector<int>& tour_path, double tour_length, double quality);
    
    /**
     * @brief Get number of cities
     * @return Size of the pheromone matrix
     */
    int size() const;
};
