/**
 * @file AcoEngine.hpp
 * @brief Main ACO algorithm engine with OpenMP parallel execution
 * 
 * This class implements the complete Ant Colony Optimization algorithm
 * with support for parallel execution using OpenMP.
 */

#pragma once

#include <vector>
#include <memory>
#include <random>
#include "Graph.hpp"
#include "PheromoneModel.hpp"
#include "ThreadLocalPheromoneModel.hpp"
#include "Ant.hpp"
#include "Tour.hpp"

/**
 * @brief ACO algorithm parameters
 */
struct AcoParameters {
    double alpha = 1.0;        // Pheromone importance factor
    double beta = 2.0;         // Distance importance factor  
    double rho = 0.1;          // Evaporation rate
    int num_ants = 50;         // Number of ants per iteration
    int max_iterations = 100;  // Maximum number of iterations
    int num_threads = 1;       // Number of OpenMP threads
    int random_seed = 42;      // Random seed for reproducibility
    
    // Convergence parameters
    bool enable_early_stopping = false;  // Enable early stopping
    int stagnation_limit = 100;          // Iterations without improvement to trigger early stop
    double target_quality = 0.0;         // Stop if this quality is reached (0 = disabled)
};

/**
 * @brief ACO algorithm execution results
 */
struct AcoResults {
    std::vector<int> best_tour_path;  // Best tour found
    double best_tour_length = 0.0;   // Length of best tour
    int convergence_iteration = -1;   // Iteration where best was found
    std::vector<double> iteration_best_lengths; // Best length per iteration
    std::vector<double> iteration_avg_lengths;  // Average length per iteration
    double execution_time_ms = 0.0;  // Total execution time
    int actual_iterations = 0;       // Actual iterations completed (may be less due to early stopping)
    bool converged = false;          // Whether algorithm converged to target quality
    bool early_stopped = false;      // Whether early stopping was triggered
    int stagnation_count = 0;        // Final stagnation count
};

class AcoEngine {
private:
    std::shared_ptr<Graph> graph_;
    std::shared_ptr<PheromoneModel> pheromones_;
    AcoParameters params_;
    std::mt19937 rng_;
    
    // Best solution tracking
    std::vector<int> global_best_tour_;
    double global_best_length_;
    
    // Current iteration statistics
    std::vector<double> current_iteration_lengths_;
    
    /**
     * @brief Execute one iteration of the ACO algorithm
     * @return Best tour length found in this iteration
     */
    double executeIteration(int iteration);
    
    /**
     * @brief Calculate average tour length for current iteration
     * @return Average tour length among all ants this iteration
     */
    double calculateIterationAverage() const;
    
    /**
     * @brief Construct tours for all ants in parallel
     * @param thread_local_deltas Vector to store thread-local pheromone deltas
     * @return Best tour length found among all ants this iteration
     */
    double constructToursParallel(std::vector<ThreadLocalPheromoneModel>& thread_local_deltas, int iteration);
    
    /**
     * @brief Update global best solution if a better one is found
     * @param tour_path Tour path to check
     * @param tour_length Length of the tour
     * @return True if this is a new global best
     */
    bool updateGlobalBest(const std::vector<int>& tour_path, double tour_length);

public:
    /**
     * @brief Construct ACO engine with given graph
     * @param graph Shared pointer to the TSP graph
     * @param params ACO algorithm parameters
     */
    explicit AcoEngine(std::shared_ptr<Graph> graph, const AcoParameters& params = AcoParameters{});
    
    /**
     * @brief Run the complete ACO algorithm
     * @return Results containing best tour and execution statistics
     */
    AcoResults run();
    
    /**
     * @brief Get current best tour
     * @return Vector of city indices representing the best tour
     */
    const std::vector<int>& getBestTour() const { return global_best_tour_; }
    
    /**
     * @brief Get current best tour length
     * @return Length of the best tour found so far
     */
    double getBestTourLength() const { return global_best_length_; }
    
    /**
     * @brief Get algorithm parameters
     * @return Current ACO parameters
     */
    const AcoParameters& getParameters() const { return params_; }
    
    /**
     * @brief Set algorithm parameters
     * @param params New ACO parameters
     */
    void setParameters(const AcoParameters& params);
    
    /**
     * @brief Reset the engine state for a new run
     */
    void reset();
};
