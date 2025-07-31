#pragma once

#include "AcoEngine.hpp"
#include "PerformanceMonitor.hpp"
#include <string>
#include <vector>
#include <map>

/**
 * @brief Different ACO algorithm strategies for comparison
 */
enum class AcoStrategy {
    STANDARD,           // Standard ACO with balanced parameters
    EXPLOITATION,       // High pheromone influence (high alpha, low beta)
    EXPLORATION,        // High heuristic influence (low alpha, high beta)
    AGGRESSIVE,         // High evaporation rate for faster convergence
    CONSERVATIVE,       // Low evaporation rate for stable solutions
    ELITIST,           // Elitist strategy (only best ants deposit pheromone)
    RANK_BASED         // Rank-based ant system
};

/**
 * @brief Configuration for a specific ACO strategy
 */
struct StrategyConfig {
    std::string name;
    std::string description;
    AcoParameters parameters;
    
    // 默認構造函數
    StrategyConfig() = default;
    
    StrategyConfig(const std::string& n, const std::string& desc, const AcoParameters& params)
        : name(n), description(desc), parameters(params) {}
};

/**
 * @brief Results from comparing different strategies
 */
struct StrategyComparisonResult {
    std::string strategy_name;
    double best_tour_length;
    double execution_time_ms;
    int convergence_iteration;
    double average_tour_length;
    PerformanceMetrics performance_metrics;
    
    // Convergence characteristics
    double convergence_speed;      // Iterations to reach 95% of final quality
    double solution_stability;     // Variance in final iterations
    double exploration_diversity;  // Diversity in early iterations
};

/**
 * @brief Framework for comparing different ACO strategies
 */
class StrategyComparator {
public:
    StrategyComparator(std::shared_ptr<Graph> graph);
    
    // Strategy management
    void addStrategy(AcoStrategy strategy, const StrategyConfig& config);
    void addCustomStrategy(const std::string& name, const AcoParameters& params, 
                          const std::string& description = "");
    
    // Comparison execution
    std::vector<StrategyComparisonResult> compareAllStrategies(int num_runs = 5);
    StrategyComparisonResult runStrategy(const StrategyConfig& config, int num_runs = 5);
    
    // Analysis and reporting
    void printComparisonReport(const std::vector<StrategyComparisonResult>& results);
    StrategyComparisonResult findBestStrategy(const std::vector<StrategyComparisonResult>& results,
                                            const std::string& metric = "tour_length");
    
    // Reproducibility testing
    bool testReproducibility(const StrategyConfig& config, int seed, int num_tests = 3);
    std::vector<double> testSeedConsistency(const StrategyConfig& config, 
                                          const std::vector<int>& seeds);
    
    // Strategy access
    StrategyConfig getStrategyConfig(AcoStrategy strategy);
    
    // Predefined strategy factories
    static StrategyConfig createStandardStrategy();
    static StrategyConfig createExploitationStrategy();
    static StrategyConfig createExplorationStrategy();
    static StrategyConfig createAggressiveStrategy();
    static StrategyConfig createConservativeStrategy();
    
    // Performance budget integration
    void setPerformanceBudget(const PerformanceBudget& budget);

private:
    std::shared_ptr<Graph> graph_;
    std::map<AcoStrategy, StrategyConfig> strategies_;
    std::vector<StrategyConfig> custom_strategies_;
    PerformanceBudget performance_budget_;
    
    // Analysis helpers
    double calculateConvergenceSpeed(const std::vector<double>& iteration_lengths);
    double calculateSolutionStability(const std::vector<double>& iteration_lengths);
    double calculateExplorationDiversity(const std::vector<double>& iteration_lengths);
};
