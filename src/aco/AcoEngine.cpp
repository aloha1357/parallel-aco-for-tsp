#include "aco/AcoEngine.hpp"
#include <omp.h>
#include <algorithm>
#include <chrono>
#include <stdexcept>
#include <limits>

AcoEngine::AcoEngine(std::shared_ptr<Graph> graph, const AcoParameters& params)
    : graph_(graph), params_(params), rng_(params.random_seed), global_best_length_(std::numeric_limits<double>::max()) {
    
    if (!graph_) {
        throw std::invalid_argument("Graph cannot be null");
    }
    
    if (params_.num_ants <= 0) {
        throw std::invalid_argument("Number of ants must be positive");
    }
    
    if (params_.max_iterations <= 0) {
        throw std::invalid_argument("Maximum iterations must be positive");
    }
    
    if (params_.num_threads <= 0) {
        throw std::invalid_argument("Number of threads must be positive");
    }
    
    // Initialize pheromone matrix
    pheromones_ = std::make_unique<PheromoneModel>(graph_->size());
    
    // Set OpenMP thread count
    omp_set_num_threads(params_.num_threads);
    
    reset();
}

void AcoEngine::setParameters(const AcoParameters& params) {
    params_ = params;
    rng_.seed(params_.random_seed);
    omp_set_num_threads(params_.num_threads);
    reset();
}

void AcoEngine::reset() {
    global_best_tour_.clear();
    global_best_length_ = std::numeric_limits<double>::max();
    
    if (pheromones_) {
        pheromones_->initialize(); // Reset to default pheromone levels
    }
}

AcoResults AcoEngine::run() {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    AcoResults results;
    results.iteration_best_lengths.reserve(params_.max_iterations);
    
    reset();
    
    for (int iteration = 0; iteration < params_.max_iterations; ++iteration) {
        double iteration_best = executeIteration();
        results.iteration_best_lengths.push_back(iteration_best);
        
        // Check if we found a new global best
        if (iteration_best < global_best_length_) {
            results.convergence_iteration = iteration;
        }
    }
    
    // Copy final results
    results.best_tour_path = global_best_tour_;
    results.best_tour_length = global_best_length_;
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    results.execution_time_ms = duration.count() / 1000.0;
    
    return results;
}

double AcoEngine::executeIteration() {
    // Create thread-local pheromone delta buffers
    std::vector<ThreadLocalPheromoneModel> thread_local_deltas;
    for (int i = 0; i < params_.num_threads; ++i) {
        thread_local_deltas.emplace_back(graph_->size());
    }
    
    // Construct tours in parallel and collect deltas
    double iteration_best = constructToursParallel(thread_local_deltas);
    
    // Evaporate pheromones
    pheromones_->evaporate(params_.rho);
    
    // Merge all thread-local deltas to global pheromone matrix
    pheromones_->mergeDeltas(thread_local_deltas);
    
    return iteration_best;
}

double AcoEngine::constructToursParallel(std::vector<ThreadLocalPheromoneModel>& thread_local_deltas) {
    double iteration_best_length = std::numeric_limits<double>::max();
    std::vector<int> iteration_best_tour;
    
    // Parallel ant construction using OpenMP
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        double thread_best_length = std::numeric_limits<double>::max();
        std::vector<int> thread_best_tour;
        
        // Each thread processes a subset of ants
        #pragma omp for schedule(static)
        for (int ant_id = 0; ant_id < params_.num_ants; ++ant_id) {
            // Create ant with thread-specific random seed
            std::mt19937 ant_rng(params_.random_seed + ant_id * 1000 + thread_id);
            Ant ant(graph_, &ant_rng);
            
            // Construct tour for this ant
            auto tour = ant.constructTour();
            double tour_length = tour->getLength();
            
            // Track thread-local best
            if (tour_length < thread_best_length) {
                thread_best_length = tour_length;
                thread_best_tour = tour->getPath();
            }
            
            // Accumulate pheromone delta for this tour
            if (tour_length > 0) {
                double quality = 1.0 / tour_length; // Q/L_k formula
                thread_local_deltas[thread_id].accumulateDelta(tour->getPath(), tour_length, quality);
            }
        }
        
        // Update global best in critical section
        #pragma omp critical(update_best)
        {
            if (thread_best_length < iteration_best_length) {
                iteration_best_length = thread_best_length;
                iteration_best_tour = thread_best_tour;
            }
        }
    }
    
    // Update global best solution
    if (!iteration_best_tour.empty()) {
        updateGlobalBest(iteration_best_tour, iteration_best_length);
    }
    
    return iteration_best_length;
}

bool AcoEngine::updateGlobalBest(const std::vector<int>& tour_path, double tour_length) {
    if (tour_length < global_best_length_) {
        global_best_tour_ = tour_path;
        global_best_length_ = tour_length;
        return true;
    }
    return false;
}
