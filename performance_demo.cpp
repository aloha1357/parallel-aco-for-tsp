#include <iostream>
#include "aco/Graph.hpp"
#include "aco/AcoEngine.hpp"
#include "aco/PerformanceMonitor.hpp"
#include "aco/SyntheticTSPGenerator.hpp"

int main() {
    try {
        std::cout << "=== Scenario 9: Performance Budget Demo ===" << std::endl;
        
        // Generate a 100-city synthetic TSP instance
        std::cout << "Generating 100-city synthetic TSP instance..." << std::endl;
        auto graph = SyntheticTSPGenerator::generateRandomInstance(100, 1000.0, 12345);
        
        // Test 1: Time budget demo
        std::cout << "\n--- Test 1: Time Budget Validation ---" << std::endl;
        AcoParameters params;
        params.alpha = 1.0;
        params.beta = 2.0;
        params.rho = 0.1;
        params.num_ants = 100;
        params.max_iterations = 30;
        params.num_threads = 4;
        params.random_seed = 42;
        
        auto time_budget = PerformanceMonitor::createTimeBudget(5000.0); // 5 seconds
        
        AcoEngine engine(graph, params);
        auto results = engine.runWithBudget(time_budget);
        
        std::cout << "Execution time: " << results.execution_time_ms << " ms" << std::endl;
        std::cout << "Time budget: 5000 ms" << std::endl;
        std::cout << "Budget compliance: " << (results.performance_metrics.budget_violated ? "VIOLATED" : "PASSED") << std::endl;
        std::cout << "Best tour length: " << results.best_tour_length << std::endl;
        
        // Test 2: Speedup validation
        std::cout << "\n--- Test 2: Speedup Validation ---" << std::endl;
        
        // Single-thread baseline
        AcoParameters single_params = params;
        single_params.num_threads = 1;
        single_params.max_iterations = 20; // Reduce for faster test
        
        AcoEngine single_engine(graph, single_params);
        auto single_results = single_engine.run();
        
        // Multi-thread test
        AcoParameters multi_params = single_params;
        multi_params.num_threads = 4;
        
        auto speedup_budget = PerformanceMonitor::createSpeedupBudget(2.0, 50.0); // 2x speedup, 50% efficiency
        
        AcoEngine multi_engine(graph, multi_params);
        auto multi_results = multi_engine.runWithBudget(speedup_budget);
        
        double actual_speedup = single_results.execution_time_ms / multi_results.execution_time_ms;
        double efficiency = (actual_speedup / 4.0) * 100.0;
        
        std::cout << "Single-thread time: " << single_results.execution_time_ms << " ms" << std::endl;
        std::cout << "Multi-thread time: " << multi_results.execution_time_ms << " ms" << std::endl;
        std::cout << "Actual speedup: " << actual_speedup << "x" << std::endl;
        std::cout << "Efficiency: " << efficiency << "%" << std::endl;
        std::cout << "Speedup budget compliance: " << (multi_results.performance_metrics.budget_violated ? "VIOLATED" : "PASSED") << std::endl;
        
        // Test 3: Memory scaling demonstration
        std::cout << "\n--- Test 3: Memory Scaling Test ---" << std::endl;
        std::vector<int> sizes = {50, 100, 150};
        
        for (int size : sizes) {
            auto test_graph = SyntheticTSPGenerator::generateRandomInstance(size, 500.0, 54321 + size);
            
            AcoParameters test_params;
            test_params.alpha = 1.0;
            test_params.beta = 2.0;
            test_params.rho = 0.1;
            test_params.num_ants = size;
            test_params.max_iterations = 10;
            test_params.num_threads = 2;
            test_params.random_seed = 42;
            
            auto memory_budget = PerformanceMonitor::createMemoryBudget(500); // 500MB
            
            AcoEngine test_engine(test_graph, test_params);
            auto test_results = test_engine.runWithBudget(memory_budget);
            
            std::cout << "Size " << size << ": Peak memory " 
                      << test_results.performance_metrics.peak_memory_usage_mb << " MB, "
                      << "Time " << test_results.execution_time_ms << " ms" << std::endl;
        }
        
        std::cout << "\n=== Performance Budget Demo Complete ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
