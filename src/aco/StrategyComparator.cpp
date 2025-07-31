#include "aco/StrategyComparator.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>

StrategyComparator::StrategyComparator(std::shared_ptr<Graph> graph) 
    : graph_(graph) {
    
    // Initialize with predefined strategies
    addStrategy(AcoStrategy::STANDARD, createStandardStrategy());
    addStrategy(AcoStrategy::EXPLOITATION, createExploitationStrategy());
    addStrategy(AcoStrategy::EXPLORATION, createExplorationStrategy());
    addStrategy(AcoStrategy::AGGRESSIVE, createAggressiveStrategy());
    addStrategy(AcoStrategy::CONSERVATIVE, createConservativeStrategy());
}

void StrategyComparator::addStrategy(AcoStrategy strategy, const StrategyConfig& config) {
    strategies_[strategy] = config;
}

void StrategyComparator::addCustomStrategy(const std::string& name, const AcoParameters& params,
                                          const std::string& description) {
    custom_strategies_.emplace_back(name, description, params);
}

std::vector<StrategyComparisonResult> StrategyComparator::compareAllStrategies(int num_runs) {
    std::vector<StrategyComparisonResult> results;
    
    // Test predefined strategies
    for (const auto& [strategy, config] : strategies_) {
        results.push_back(runStrategy(config, num_runs));
    }
    
    // Test custom strategies
    for (const auto& config : custom_strategies_) {
        results.push_back(runStrategy(config, num_runs));
    }
    
    return results;
}

StrategyComparisonResult StrategyComparator::runStrategy(const StrategyConfig& config, int num_runs) {
    std::vector<double> tour_lengths;
    std::vector<double> execution_times;
    std::vector<int> convergence_iterations;
    std::vector<double> average_lengths;
    std::vector<std::vector<double>> all_iteration_lengths;
    
    PerformanceMetrics combined_metrics;
    
    for (int run = 0; run < num_runs; ++run) {
        AcoEngine engine(graph_, config.parameters);
        
        // Enable performance monitoring if budget is set
        if (performance_budget_.max_execution_time_ms > 0.0 || performance_budget_.max_memory_usage_mb > 0) {
            auto results = engine.runWithBudget(performance_budget_);
            tour_lengths.push_back(results.best_tour_length);
            execution_times.push_back(results.execution_time_ms);
            convergence_iterations.push_back(results.convergence_iteration);
            
            // Calculate average length
            if (!results.iteration_avg_lengths.empty()) {
                double avg = std::accumulate(results.iteration_avg_lengths.begin(), 
                                           results.iteration_avg_lengths.end(), 0.0) / 
                           results.iteration_avg_lengths.size();
                average_lengths.push_back(avg);
            }
            
            all_iteration_lengths.push_back(results.iteration_best_lengths);
            
            // Accumulate performance metrics
            if (run == 0) {
                combined_metrics = results.performance_metrics;
            } else {
                combined_metrics.execution_time_ms += results.performance_metrics.execution_time_ms;
                combined_metrics.peak_memory_usage_mb = std::max(combined_metrics.peak_memory_usage_mb,
                                                                results.performance_metrics.peak_memory_usage_mb);
            }
        } else {
            auto results = engine.run();
            tour_lengths.push_back(results.best_tour_length);
            execution_times.push_back(results.execution_time_ms);
            convergence_iterations.push_back(results.convergence_iteration);
            
            if (!results.iteration_avg_lengths.empty()) {
                double avg = std::accumulate(results.iteration_avg_lengths.begin(), 
                                           results.iteration_avg_lengths.end(), 0.0) / 
                           results.iteration_avg_lengths.size();
                average_lengths.push_back(avg);
            }
            
            all_iteration_lengths.push_back(results.iteration_best_lengths);
        }
    }
    
    // Calculate statistics
    StrategyComparisonResult result;
    result.strategy_name = config.name;
    result.best_tour_length = *std::min_element(tour_lengths.begin(), tour_lengths.end());
    result.execution_time_ms = std::accumulate(execution_times.begin(), execution_times.end(), 0.0) / num_runs;
    result.convergence_iteration = std::accumulate(convergence_iterations.begin(), convergence_iterations.end(), 0) / num_runs;
    result.average_tour_length = std::accumulate(average_lengths.begin(), average_lengths.end(), 0.0) / average_lengths.size();
    
    // Calculate convergence characteristics
    if (!all_iteration_lengths.empty()) {
        result.convergence_speed = calculateConvergenceSpeed(all_iteration_lengths[0]);
        result.solution_stability = calculateSolutionStability(all_iteration_lengths[0]);
        result.exploration_diversity = calculateExplorationDiversity(all_iteration_lengths[0]);
    }
    
    // Average performance metrics
    combined_metrics.execution_time_ms /= num_runs;
    result.performance_metrics = combined_metrics;
    
    return result;
}

void StrategyComparator::printComparisonReport(const std::vector<StrategyComparisonResult>& results) {
    std::cout << "\n=== ACO Strategy Comparison Report ===" << std::endl;
    std::cout << std::setw(15) << "Strategy" 
              << std::setw(12) << "Best Length"
              << std::setw(12) << "Avg Time(ms)"
              << std::setw(12) << "Conv. Iter"
              << std::setw(12) << "Stability"
              << std::setw(12) << "Conv. Speed" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    
    for (const auto& result : results) {
        std::cout << std::setw(15) << result.strategy_name
                  << std::setw(12) << std::fixed << std::setprecision(1) << result.best_tour_length
                  << std::setw(12) << std::fixed << std::setprecision(1) << result.execution_time_ms
                  << std::setw(12) << result.convergence_iteration
                  << std::setw(12) << std::fixed << std::setprecision(3) << result.solution_stability
                  << std::setw(12) << std::fixed << std::setprecision(1) << result.convergence_speed
                  << std::endl;
    }
    
    // Find best performers
    auto best_quality = std::min_element(results.begin(), results.end(),
        [](const auto& a, const auto& b) { return a.best_tour_length < b.best_tour_length; });
    
    auto fastest = std::min_element(results.begin(), results.end(),
        [](const auto& a, const auto& b) { return a.execution_time_ms < b.execution_time_ms; });
    
    std::cout << "\n=== Best Performers ===" << std::endl;
    std::cout << "Best Quality: " << best_quality->strategy_name 
              << " (Length: " << best_quality->best_tour_length << ")" << std::endl;
    std::cout << "Fastest: " << fastest->strategy_name 
              << " (Time: " << fastest->execution_time_ms << " ms)" << std::endl;
}

StrategyComparisonResult StrategyComparator::findBestStrategy(
    const std::vector<StrategyComparisonResult>& results, const std::string& metric) {
    
    if (results.empty()) {
        throw std::invalid_argument("No results to compare");
    }
    
    if (metric == "tour_length") {
        return *std::min_element(results.begin(), results.end(),
            [](const auto& a, const auto& b) { return a.best_tour_length < b.best_tour_length; });
    } else if (metric == "time") {
        return *std::min_element(results.begin(), results.end(),
            [](const auto& a, const auto& b) { return a.execution_time_ms < b.execution_time_ms; });
    } else if (metric == "convergence") {
        return *std::min_element(results.begin(), results.end(),
            [](const auto& a, const auto& b) { return a.convergence_speed < b.convergence_speed; });
    }
    
    return results[0]; // Default to first
}

bool StrategyComparator::testReproducibility(const StrategyConfig& config, int seed, int num_tests) {
    std::vector<double> tour_lengths;
    std::vector<std::vector<int>> tour_paths;
    
    AcoParameters test_params = config.parameters;
    test_params.random_seed = seed;
    
    for (int test = 0; test < num_tests; ++test) {
        AcoEngine engine(graph_, test_params);
        auto results = engine.run();
        
        tour_lengths.push_back(results.best_tour_length);
        tour_paths.push_back(results.best_tour_path);
    }
    
    // Check if all results are identical
    double first_length = tour_lengths[0];
    for (double length : tour_lengths) {
        if (std::abs(length - first_length) > 1e-6) {
            return false;
        }
    }
    
    return true;
}

StrategyConfig StrategyComparator::getStrategyConfig(AcoStrategy strategy) {
    auto it = strategies_.find(strategy);
    if (it != strategies_.end()) {
        return it->second;
    }
    
    // If strategy not found, return standard strategy as default
    return createStandardStrategy();
}

std::vector<double> StrategyComparator::testSeedConsistency(const StrategyConfig& config,
                                                          const std::vector<int>& seeds) {
    std::vector<double> results;
    
    for (int seed : seeds) {
        AcoParameters test_params = config.parameters;
        test_params.random_seed = seed;
        
        AcoEngine engine(graph_, test_params);
        auto result = engine.run();
        results.push_back(result.best_tour_length);
    }
    
    return results;
}

void StrategyComparator::setPerformanceBudget(const PerformanceBudget& budget) {
    performance_budget_ = budget;
}

// Strategy factory methods
StrategyConfig StrategyComparator::createStandardStrategy() {
    AcoParameters params;
    params.alpha = 1.0;
    params.beta = 2.0;
    params.rho = 0.1;
    params.num_ants = 50;
    params.max_iterations = 100;
    params.num_threads = 4;
    params.random_seed = 42;
    
    return StrategyConfig("Standard", "Balanced ACO with standard parameters", params);
}

StrategyConfig StrategyComparator::createExploitationStrategy() {
    AcoParameters params;
    params.alpha = 2.0;  // High pheromone influence
    params.beta = 1.0;   // Low heuristic influence
    params.rho = 0.05;   // Low evaporation
    params.num_ants = 50;
    params.max_iterations = 100;
    params.num_threads = 4;
    params.random_seed = 42;
    
    return StrategyConfig("Exploitation", "High pheromone influence for exploitation", params);
}

StrategyConfig StrategyComparator::createExplorationStrategy() {
    AcoParameters params;
    params.alpha = 0.5;  // Low pheromone influence
    params.beta = 3.0;   // High heuristic influence
    params.rho = 0.2;    // High evaporation
    params.num_ants = 75; // More ants for exploration
    params.max_iterations = 100;
    params.num_threads = 4;
    params.random_seed = 42;
    
    return StrategyConfig("Exploration", "High heuristic influence for exploration", params);
}

StrategyConfig StrategyComparator::createAggressiveStrategy() {
    AcoParameters params;
    params.alpha = 1.5;
    params.beta = 2.5;
    params.rho = 0.3;    // High evaporation for fast convergence
    params.num_ants = 30; // Fewer ants, faster iterations
    params.max_iterations = 150;
    params.num_threads = 4;
    params.random_seed = 42;
    
    return StrategyConfig("Aggressive", "High evaporation for fast convergence", params);
}

StrategyConfig StrategyComparator::createConservativeStrategy() {
    AcoParameters params;
    params.alpha = 1.0;
    params.beta = 2.0;
    params.rho = 0.05;   // Very low evaporation
    params.num_ants = 100; // More ants for stable solutions
    params.max_iterations = 80;
    params.num_threads = 4;
    params.random_seed = 42;
    
    return StrategyConfig("Conservative", "Low evaporation for stable solutions", params);
}

// Analysis helper methods
double StrategyComparator::calculateConvergenceSpeed(const std::vector<double>& iteration_lengths) {
    if (iteration_lengths.size() < 10) return -1.0;
    
    double final_quality = iteration_lengths.back();
    double target_quality = final_quality * 1.05; // 95% of final quality
    
    for (size_t i = 0; i < iteration_lengths.size(); ++i) {
        if (iteration_lengths[i] <= target_quality) {
            return static_cast<double>(i);
        }
    }
    
    return static_cast<double>(iteration_lengths.size());
}

double StrategyComparator::calculateSolutionStability(const std::vector<double>& iteration_lengths) {
    if (iteration_lengths.size() < 20) return 0.0;
    
    // Calculate variance in final 20% of iterations
    size_t start_idx = iteration_lengths.size() * 0.8;
    double mean = 0.0;
    size_t count = 0;
    
    for (size_t i = start_idx; i < iteration_lengths.size(); ++i) {
        mean += iteration_lengths[i];
        count++;
    }
    mean /= count;
    
    double variance = 0.0;
    for (size_t i = start_idx; i < iteration_lengths.size(); ++i) {
        double diff = iteration_lengths[i] - mean;
        variance += diff * diff;
    }
    variance /= count;
    
    return std::sqrt(variance);
}

double StrategyComparator::calculateExplorationDiversity(const std::vector<double>& iteration_lengths) {
    if (iteration_lengths.size() < 20) return 0.0;
    
    // Calculate variance in first 20% of iterations
    size_t end_idx = iteration_lengths.size() * 0.2;
    double mean = 0.0;
    
    for (size_t i = 0; i < end_idx; ++i) {
        mean += iteration_lengths[i];
    }
    mean /= end_idx;
    
    double variance = 0.0;
    for (size_t i = 0; i < end_idx; ++i) {
        double diff = iteration_lengths[i] - mean;
        variance += diff * diff;
    }
    variance /= end_idx;
    
    return std::sqrt(variance);
}
