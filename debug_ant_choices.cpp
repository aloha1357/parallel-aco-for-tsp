#include <iostream>
#include <memory>
#include <iomanip>
#include "include/aco/Graph.hpp"
#include "include/aco/PheromoneModel.hpp"
#include "include/aco/Ant.hpp"
#include "include/aco/Tour.hpp"

int main() {
    // Create a simple 4-city graph
    auto graph = std::make_shared<Graph>(4);
    
    // Set distances
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
    
    std::cout << "=== Testing 10 ants with different seeds ===" << std::endl;
    
    // Test multiple ants with different seeds
    for (int ant_id = 0; ant_id < 10; ++ant_id) {
        std::mt19937 rng(42 + ant_id * 1000);  // Same as AcoEngine
        Ant ant(graph, pheromone, &rng, 1.0, 2.0);
        auto tour = ant.constructTour();
        
        std::cout << "Ant " << std::setw(2) << ant_id << " (seed=" << (42 + ant_id * 1000) << "): ";
        for (int city : tour->getPath()) {
            std::cout << city << " ";
        }
        std::cout << " Length=" << std::fixed << std::setprecision(1) << tour->getLength() << std::endl;
    }
    
    std::cout << "\n=== Testing single ant step-by-step ===" << std::endl;
    
    // Test one ant step by step to see internal behavior
    std::mt19937 debug_rng(42);
    Ant debug_ant(graph, pheromone, &debug_rng, 1.0, 2.0);
    
    // Manually trace chooseNextCity behavior
    debug_ant.reset();
    debug_ant.setCurrentCity(0);
    debug_ant.markVisited(0);
    
    std::cout << "Starting from city 0" << std::endl;
    std::cout << "Available cities: 1, 2, 3" << std::endl;
    
    // Check probabilities for the first choice
    auto probs = debug_ant.calculateSelectionProbabilities();
    std::cout << "Probabilities: P(0->1)=" << std::fixed << std::setprecision(3) << probs[1]
              << ", P(0->2)=" << probs[2] << ", P(0->3)=" << probs[3] << std::endl;
    
    // Make multiple choices to see if there's variety
    for (int test = 0; test < 5; ++test) {
        debug_ant.reset();
        debug_ant.setCurrentCity(0);
        debug_ant.markVisited(0);
        
        int next = debug_ant.chooseNextCity();
        std::cout << "Choice " << test << ": " << next << std::endl;
    }
    
    return 0;
}
