#include <iostream>
#include <memory>
#include "aco/Graph.hpp"
#include "aco/AcoEngine.hpp"
#include "aco/Ant.hpp"
#include "aco/PheromoneModel.hpp"

int main() {
    try {
        // Test with a simple 4x4 graph first
        auto graph = std::make_shared<Graph>(4);
        graph->setDistance(0, 1, 10.0);
        graph->setDistance(0, 2, 15.0);
        graph->setDistance(0, 3, 20.0);
        graph->setDistance(1, 2, 35.0);
        graph->setDistance(1, 3, 25.0);
        graph->setDistance(2, 3, 30.0);
        
        // Test individual ant construction
        auto pheromones = std::make_shared<PheromoneModel>(4);
        pheromones->initialize(1.0);
        
        std::cout << "Testing individual ant construction:" << std::endl;
        for (int i = 0; i < 3; ++i) {
            Ant ant(graph, pheromones, 1.0, 2.0, 42 + i);
            auto tour = ant.constructTour();
            std::cout << "Ant " << i << " tour length: " << tour->getLength() << std::endl;
            std::cout << "Path: ";
            for (int city : tour->getPath()) {
                std::cout << city << " ";
            }
            std::cout << std::endl;
        }
        
        std::cout << "\nTesting ACO Engine:" << std::endl;
        AcoParameters params;
        params.alpha = 1.0;
        params.beta = 2.0;
        params.rho = 0.1;
        params.num_ants = 4;
        params.max_iterations = 10;
        params.num_threads = 1;
        params.random_seed = 42;
        
        AcoEngine engine(graph, params);
        auto results = engine.run();
        
        std::cout << "Best tour length: " << results.best_tour_length << std::endl;
        std::cout << "Convergence iteration: " << results.convergence_iteration << std::endl;
        std::cout << "Actual iterations: " << results.actual_iterations << std::endl;
        
        std::cout << "\nIteration best lengths:" << std::endl;
        for (size_t i = 0; i < results.iteration_best_lengths.size(); ++i) {
            std::cout << "Iteration " << i << ": best=" << results.iteration_best_lengths[i] 
                      << ", avg=" << results.iteration_avg_lengths[i] << std::endl;
        }
        
        std::cout << "\nBest tour path: ";
        for (int city : results.best_tour_path) {
            std::cout << city << " ";
        }
        std::cout << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }
}
