#include <iostream>
#include "aco/Graph.hpp"
#include "aco/AcoEngine.hpp"
#include "aco/StrategyComparator.hpp"
#include "aco/SyntheticTSPGenerator.hpp"

int main() {
    try {
        std::cout << "=== ACO Strategy Comparison Demo ===" << std::endl;
        
        // Generate test instance
        std::cout << "Generating 50-city synthetic TSP instance..." << std::endl;
        auto graph = SyntheticTSPGenerator::generateRandomInstance(50, 500.0, 12345);
        
        StrategyComparator comparator(graph);
        
        // Set performance budget
        auto budget = PerformanceMonitor::createTimeBudget(10000.0); // 10 seconds per strategy
        comparator.setPerformanceBudget(budget);
        
        std::cout << "\nRunning strategy comparison (this may take a few minutes)..." << std::endl;
        
        // Compare all predefined strategies
        auto results = comparator.compareAllStrategies(3); // 3 runs per strategy
        
        // Print detailed comparison report
        comparator.printComparisonReport(results);
        
        // Find and highlight best performers
        std::cout << "\n=== Detailed Analysis ===" << std::endl;
        
        auto best_quality = comparator.findBestStrategy(results, "tour_length");
        std::cout << "\n🏆 Best Quality Strategy: " << best_quality.strategy_name << std::endl;
        std::cout << "   Tour Length: " << best_quality.best_tour_length << std::endl;
        std::cout << "   Convergence Iteration: " << best_quality.convergence_iteration << std::endl;
        std::cout << "   Solution Stability: " << best_quality.solution_stability << std::endl;
        
        auto fastest = comparator.findBestStrategy(results, "time");
        std::cout << "\n⚡ Fastest Strategy: " << fastest.strategy_name << std::endl;
        std::cout << "   Execution Time: " << fastest.execution_time_ms << " ms" << std::endl;
        std::cout << "   Tour Length: " << fastest.best_tour_length << std::endl;
        
        // Test reproducibility
        std::cout << "\n=== Reproducibility Testing ===" << std::endl;
        auto standard_config = StrategyComparator::createStandardStrategy();
        standard_config.parameters.max_iterations = 50;
        
        bool reproducible = comparator.testReproducibility(standard_config, 77777, 3);
        std::cout << "Standard strategy reproducibility: " << (reproducible ? "✅ PASSED" : "❌ FAILED") << std::endl;
        
        // Test seed consistency
        std::vector<int> test_seeds = {12345, 54321, 98765};
        auto seed_results = comparator.testSeedConsistency(standard_config, test_seeds);
        
        std::cout << "\nSeed consistency test:" << std::endl;
        for (size_t i = 0; i < test_seeds.size(); ++i) {
            std::cout << "  Seed " << test_seeds[i] << ": Tour length " << seed_results[i] << std::endl;
        }
        
        // Strategy insights
        std::cout << "\n=== Strategy Insights ===" << std::endl;
        for (const auto& result : results) {
            std::cout << "\n" << result.strategy_name << ":" << std::endl;
            std::cout << "  - Average convergence at iteration " << result.convergence_iteration << std::endl;
            std::cout << "  - Convergence speed: " << result.convergence_speed << " iterations" << std::endl;
            std::cout << "  - Solution stability: " << result.solution_stability << std::endl;
            std::cout << "  - Exploration diversity: " << result.exploration_diversity << std::endl;
        }
        
        std::cout << "\n=== Demo Complete ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
