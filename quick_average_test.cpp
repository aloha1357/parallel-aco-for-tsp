#include "aco/BenchmarkAnalyzer.hpp"
#include "aco/TSPLibReader.hpp"
#include <iostream>
#include <vector>

using namespace aco;

int main() {
    std::cout << "=== Quick Average Test Verification ===" << std::endl;
    std::cout << "Quick verification of average testing functionality (reduced repetitions)" << std::endl;
    std::cout << std::endl;

    BenchmarkAnalyzer analyzer;
    
    // Select a small test problem for quick verification
    std::vector<TSPBenchmark> test_benchmarks = {
        {"berlin52", "../data/berlin52.tsp", 7542, 52}  // Test only one medium-scale problem with relative path
    };
    
    // Configure test parameters - reduce repetitions for quick testing
    int runs_per_configuration = 3;  // Quick test only runs 3 times
    
    std::cout << "=== Quick Test Configuration ===" << std::endl;
    std::cout << "Repetitions: " << runs_per_configuration << " times/config (quick verification)" << std::endl;
    std::cout << "Test problems: " << test_benchmarks.size() << " (berlin52)" << std::endl;
    std::cout << "Expected thread configs: 1, 2, 4, 8" << std::endl;
    std::cout << "Expected strategies: 5 ACO strategies" << std::endl;
    std::cout << "Expected total time: about 3-5 minutes" << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== Starting Quick Average Test ===" << std::endl;
    
    try {
        analyzer.runDetailedAverageAnalysis(
            test_benchmarks, 
            runs_per_configuration, 
            "quick_average_test"
        );
        
        std::cout << "\n=== Quick Test Complete! ===" << std::endl;
        std::cout << "\n📊 Generated files:" << std::endl;
        std::cout << "• quick_average_test_scalability_detailed.csv" << std::endl;
        std::cout << "• quick_average_test_strategy_detailed.csv" << std::endl;
        std::cout << "• quick_average_test_statistical_analysis.md" << std::endl;
        
        std::cout << "\n✅ Average testing functionality verified successfully!" << std::endl;
        std::cout << "\n💡 For complete testing, please run:" << std::endl;
        std::cout << "   ./enhanced_average_test.exe" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Quick test failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
