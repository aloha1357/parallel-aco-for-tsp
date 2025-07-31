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
    pheromones_ = std::make_shared<PheromoneModel>(graph_->size());
    
    // Initialize performance monitoring
    performance_monitor_ = std::make_unique<PerformanceMonitor>();
    
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
    results.iteration_avg_lengths.reserve(params_.max_iterations);
    
    reset();
    
    int stagnation_count = 0;
    double last_best_length = std::numeric_limits<double>::max();
    
    for (int iteration = 0; iteration < params_.max_iterations; ++iteration) {
        double iteration_best = executeIteration(iteration);
        double iteration_avg = calculateIterationAverage();
        
        results.iteration_best_lengths.push_back(global_best_length_);
        results.iteration_avg_lengths.push_back(iteration_avg);
        results.actual_iterations = iteration + 1;
        
        // Check if we found a new global best
        if (iteration_best < last_best_length) {
            global_best_length_ = iteration_best;
            results.convergence_iteration = iteration;
            stagnation_count = 0; // Reset stagnation counter
            last_best_length = iteration_best;
        } else {
            stagnation_count++;
        }
        
        // Check for convergence to target quality
        if (params_.target_quality > 0.0 && global_best_length_ <= params_.target_quality) {
            results.converged = true;
            break;
        }
        
        // Check for early stopping due to stagnation
        if (params_.enable_early_stopping && stagnation_count >= params_.stagnation_limit) {
            results.early_stopped = true;
            break;
        }
    }
    
    // Copy final results
    results.best_tour_path = global_best_tour_;
    results.best_tour_length = global_best_length_;
    results.stagnation_count = stagnation_count;
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    results.execution_time_ms = duration.count() / 1000.0;

    return results;
}

AcoResults AcoEngine::runWithBudget(const PerformanceBudget& budget) {
    setPerformanceBudget(budget);
    performance_monitor_->setBudget(budget);
    
    // Start performance monitoring
    performance_monitor_->startMonitoring();
    performance_monitor_->recordCheckpoint("algorithm_start");
    
    // Run the standard algorithm
    AcoResults results = run();
    
    // Stop monitoring and collect metrics
    performance_monitor_->recordCheckpoint("algorithm_end");
    performance_monitor_->stopMonitoring();
    
    // Add performance metrics to results
    results.performance_metrics = performance_monitor_->getMetrics();
    
    // Validate budget compliance
    bool budget_met = performance_monitor_->validateBudget();
    if (!budget_met) {
        auto violations = performance_monitor_->getBudgetViolations();
        // Note: Budget violations are already recorded in performance_metrics
    }
    
    return results;
}

void AcoEngine::setPerformanceBudget(const PerformanceBudget& budget) {
    performance_budget_ = budget;
    if (performance_monitor_) {
        performance_monitor_->setBudget(budget);
    }
}

void AcoEngine::enablePerformanceMonitoring(bool enable) {
    if (enable && !performance_monitor_) {
        performance_monitor_ = std::make_unique<PerformanceMonitor>();
        performance_monitor_->setBudget(performance_budget_);
    } else if (!enable) {
        performance_monitor_.reset();
    }
}double AcoEngine::executeIteration(int iteration) {
    // Create thread-local pheromone delta buffers
    std::vector<ThreadLocalPheromoneModel> thread_local_deltas;
    for (int i = 0; i < params_.num_threads; ++i) {
        thread_local_deltas.emplace_back(graph_->size());
    }
    
    // Clear current iteration statistics
    current_iteration_lengths_.clear();
    current_iteration_lengths_.reserve(params_.num_ants);
    
    // Construct tours in parallel and collect deltas
    double iteration_best = constructToursParallel(thread_local_deltas, iteration);
    
    // Evaporate pheromones
    pheromones_->evaporate(params_.rho);
    
    // Merge all thread-local deltas to global pheromone matrix
    pheromones_->mergeDeltas(thread_local_deltas);
    
    return iteration_best;
}

double AcoEngine::constructToursParallel(std::vector<ThreadLocalPheromoneModel>& thread_local_deltas, int iteration) {
    double iteration_best_length = std::numeric_limits<double>::max();
    std::vector<int> iteration_best_tour;
    
    // Thread-safe collection of all tour lengths
    std::vector<std::vector<double>> thread_tour_lengths(params_.num_threads);
    
    // Parallel ant construction using OpenMP
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        double thread_best_length = std::numeric_limits<double>::max();
        std::vector<int> thread_best_tour;
        
        // Each thread processes a subset of ants
        #pragma omp for schedule(static)
        for (int ant_id = 0; ant_id < params_.num_ants; ++ant_id) {
            // Create ant with thread-specific random seed and pheromone model
            std::mt19937 ant_rng(params_.random_seed + iteration * 10000 + ant_id * 1000 + thread_id);
            Ant ant(graph_, pheromones_, &ant_rng, params_.alpha, params_.beta);
            
            // Construct tour for this ant
            auto tour = ant.constructTour();
            double tour_length = tour->getLength();
            
            // Collect tour length for statistics
            thread_tour_lengths[thread_id].push_back(tour_length);
            
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
    
    // Collect all tour lengths from all threads
    for (const auto& thread_lengths : thread_tour_lengths) {
        current_iteration_lengths_.insert(current_iteration_lengths_.end(), 
                                         thread_lengths.begin(), thread_lengths.end());
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

double AcoEngine::calculateIterationAverage() const {
    if (current_iteration_lengths_.empty()) {
        return 0.0;
    }
    
    double sum = 0.0;
    for (double length : current_iteration_lengths_) {
        sum += length;
    }
    return sum / current_iteration_lengths_.size();
}
