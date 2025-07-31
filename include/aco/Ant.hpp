/**
 * @file Ant.hpp
 * @brief Ant agent for ACO algorithm with probabilistic city selection
 * 
 * This class represents an ant that constructs tours using ACO probabilistic rules.
 * The ant selects the next city based on pheromone levels and heuristic information.
 */

#pragma once

#include <memory>
#include <random>
#include <vector>

// Forward declarations
class Graph;
class Tour;
class PheromoneModel;

class Ant {
private:
    std::shared_ptr<Graph> graph_;
    std::shared_ptr<PheromoneModel> pheromone_;
    std::mt19937 rng_;
    std::mt19937* external_rng_;  // Pointer to external RNG (optional)
    
    // ACO parameters
    double alpha_;  // Pheromone importance factor
    double beta_;   // Heuristic importance factor
    
    // Current tour state
    std::vector<bool> visited_;
    int current_city_;

public:
    /**
     * @brief Constructor for Ant with Graph and PheromoneModel
     * @param graph Shared pointer to the graph
     * @param pheromone Shared pointer to pheromone model
     * @param alpha Pheromone importance (default: 1.0)
     * @param beta Heuristic importance (default: 2.0)
     * @param seed Random seed for reproducible results
     */
    Ant(std::shared_ptr<Graph> graph, 
        std::shared_ptr<PheromoneModel> pheromone = nullptr,
        double alpha = 1.0, 
        double beta = 2.0,
        unsigned int seed = 42);
    
    /**
     * @brief Constructor for Ant with external random number generator
     * @param graph Shared pointer to the graph
     * @param external_rng Pointer to external random number generator
     * @param alpha Pheromone importance (default: 1.0)
     * @param beta Heuristic importance (default: 2.0)
     */
    Ant(std::shared_ptr<Graph> graph, 
        std::mt19937* external_rng,
        double alpha = 1.0, 
        double beta = 2.0);
    
    /**
     * @brief Construct a tour visiting all cities exactly once
     * @return Unique pointer to constructed tour
     */
    std::unique_ptr<Tour> constructTour();
    
    /**
     * @brief Set current position and reset visited cities
     * @param city Starting city
     */
    void setCurrentCity(int city);
    
    /**
     * @brief Get current city position
     * @return Current city index
     */
    int getCurrentCity() const;
    
    /**
     * @brief Choose next city using ACO probabilistic rule
     * @return Selected city index (-1 if no cities available)
     */
    int chooseNextCity();
    
    /**
     * @brief Calculate selection probabilities for all unvisited cities
     * @return Vector of probabilities for each city
     */
    std::vector<double> calculateSelectionProbabilities() const;
    
    /**
     * @brief Mark a city as visited
     * @param city City to mark as visited
     */
    void markVisited(int city);
    
    /**
     * @brief Reset ant state for new tour construction
     */
    void reset();

private:
    /**
     * @brief Get reference to the random number generator in use
     * @return Reference to either internal or external RNG
     */
    std::mt19937& getRNG();
    /**
     * @brief Simple greedy tour construction (fallback when no pheromone model)
     * @return Vector representing the tour path
     */
    std::vector<int> constructGreedyTour();
    
    /**
     * @brief ACO-based tour construction using probabilistic selection
     * @return Vector representing the tour path
     */
    std::vector<int> constructACOTour();
};
