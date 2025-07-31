#include <iostream>
#include <memory>
#include <random>
#include "include/aco/Graph.hpp"
#include "include/aco/PheromoneModel.hpp"
#include "include/aco/Ant.hpp"
#include "include/aco/Tour.hpp"

int main() {
    // Create a simple 4-city graph
    auto graph = std::make_shared<Graph>(4);
    
    // Set distances (simplified)
    double distances[4][4] = {
        {0, 1, 2, 3},
        {1, 0, 1, 2},
        {2, 1, 0, 1},
        {3, 2, 1, 0}
    };
    
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            graph->setDistance(i, j, distances[i][j]);
        }
    }
    
    auto pheromone = std::make_shared<PheromoneModel>(4);
    
    // Test 1: Normal pheromones
    std::cout << "=== Test 1: Normal pheromones ===" << std::endl;
    std::mt19937 rng1(42);
    Ant ant1(graph, pheromone, &rng1, 1.0, 2.0);
    auto tour1 = ant1.constructTour();
    std::cout << "Tour 1: ";
    for (int city : tour1->getPath()) {
        std::cout << city << " ";
    }
    std::cout << std::endl;
    
    // Test 2: Bias pheromone from 0 to 1
    std::cout << "\n=== Test 2: Biased pheromones (0->1) ===" << std::endl;
    pheromone->setPheromone(0, 1, 10.0);  // Strong bias to city 1
    std::mt19937 rng2(42);  // Same seed
    Ant ant2(graph, pheromone, &rng2, 1.0, 2.0);
    auto tour2 = ant2.constructTour();
    std::cout << "Tour 2: ";
    for (int city : tour2->getPath()) {
        std::cout << city << " ";
    }
    std::cout << std::endl;
    
    // Test 3: Different seed, biased pheromones
    std::cout << "\n=== Test 3: Different seed, biased pheromones ===" << std::endl;
    std::mt19937 rng3(123);  // Different seed
    Ant ant3(graph, pheromone, &rng3, 1.0, 2.0);
    auto tour3 = ant3.constructTour();
    std::cout << "Tour 3: ";
    for (int city : tour3->getPath()) {
        std::cout << city << " ";
    }
    std::cout << std::endl;
    
    // Test 4: Multiple ants with different seeds
    std::cout << "\n=== Test 4: Multiple ants ===" << std::endl;
    for (int i = 0; i < 5; ++i) {
        std::mt19937 rng(100 + i);
        Ant ant(graph, pheromone, &rng, 1.0, 2.0);
        auto tour = ant.constructTour();
        std::cout << "Ant " << i << ": ";
        for (int city : tour->getPath()) {
            std::cout << city << " ";
        }
        std::cout << std::endl;
    }
    
    return 0;
}
