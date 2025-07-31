#include <iostream>
#include <memory>
#include "include/aco/Graph.hpp"
#include "include/aco/AcoEngine.hpp"

int main() {
    try {
        // Load eil51.tsp
        auto graph = Graph::fromTSPFile("data/eil51.tsp");
        std::cout << "Loaded eil51.tsp with " << graph->size() << " cities" << std::endl;
        
        // Test different parameter configurations
        struct TestConfig {
            std::string name;
            double alpha, beta, rho;
            int num_ants, max_iterations;
        };
        
        std::vector<TestConfig> configs = {
            {"Current", 1.0, 2.0, 0.1, 51, 50},
            {"More Exploration", 0.5, 2.0, 0.1, 51, 50},
            {"More Exploitation", 2.0, 1.0, 0.1, 51, 50},
            {"Higher Evaporation", 1.0, 2.0, 0.3, 51, 50},
            {"More Ants", 1.0, 2.0, 0.1, 100, 50},
            {"Best Practice", 1.0, 3.0, 0.5, 51, 100}
        };
        
        for (const auto& config : configs) {
            std::cout << "\n=== Testing: " << config.name << " ===" << std::endl;
            std::cout << "Alpha=" << config.alpha << ", Beta=" << config.beta 
                      << ", Rho=" << config.rho << ", Ants=" << config.num_ants << std::endl;
            
            AcoParameters params;
            params.alpha = config.alpha;
            params.beta = config.beta;
            params.rho = config.rho;
            params.num_ants = config.num_ants;
            params.max_iterations = config.max_iterations;
            params.num_threads = 1;
            params.random_seed = 42;
            
            AcoEngine engine(graph, params);
            auto results = engine.run();
            
            std::cout << "Best length: " << results.best_tour_length << std::endl;
            std::cout << "Convergence iteration: " << results.convergence_iteration << std::endl;
            
            // Show improvement trend
            if (results.iteration_best_lengths.size() >= 10) {
                std::cout << "Progress: ";
                for (int i = 0; i < std::min(10, (int)results.iteration_best_lengths.size()); i += 2) {
                    std::cout << (int)results.iteration_best_lengths[i] << " ";
                }
                std::cout << "... " << (int)results.best_tour_length << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
