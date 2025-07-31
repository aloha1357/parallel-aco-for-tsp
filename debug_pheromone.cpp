#include <iostream>
#include <memory>
#include "aco/Graph.hpp"
#include "aco/PheromoneModel.hpp"
#include "aco/Ant.hpp"
#include "aco/Tour.hpp"

int main() {
    // Create a simple 4-city test case
    auto graph = std::make_shared<Graph>(4);
    graph->setDistance(0, 1, 10.0);
    graph->setDistance(0, 2, 15.0);
    graph->setDistance(0, 3, 20.0);
    graph->setDistance(1, 2, 35.0);
    graph->setDistance(1, 3, 25.0);
    graph->setDistance(2, 3, 30.0);
    
    auto pheromones = std::make_shared<PheromoneModel>(4);
    pheromones->initialize(1.0);
    
    std::cout << "Initial pheromone values:" << std::endl;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            std::cout << "τ(" << i << "," << j << ") = " << pheromones->getPheromone(i, j) << " ";
        }
        std::cout << std::endl;
    }
    
    // Create ant and test probability calculation
    Ant ant(graph, pheromones, 1.0, 2.0, 42);
    ant.setCurrentCity(0);
    
    std::cout << "\nTesting probability calculation from city 0:" << std::endl;
    
    // We need to access the private method somehow, let's test tour construction instead
    std::cout << "Constructing multiple tours to see variety:" << std::endl;
    
    for (int i = 0; i < 5; ++i) {
        Ant test_ant(graph, pheromones, 1.0, 2.0, 42 + i);
        auto tour = test_ant.constructTour();
        
        std::cout << "Tour " << i << ": ";
        for (int city : tour->getPath()) {
            std::cout << city << " ";
        }
        std::cout << " (length: " << tour->getLength() << ")" << std::endl;
    }
    
    // Test with different pheromone levels
    std::cout << "\nTesting with biased pheromones:" << std::endl;
    pheromones->setPheromone(0, 1, 5.0);  // Make 0->1 very attractive
    pheromones->setPheromone(0, 2, 0.1);  // Make 0->2 less attractive
    
    for (int i = 0; i < 5; ++i) {
        Ant test_ant(graph, pheromones, 1.0, 2.0, 42 + i);
        auto tour = test_ant.constructTour();
        
        std::cout << "Biased Tour " << i << ": ";
        for (int city : tour->getPath()) {
            std::cout << city << " ";
        }
        std::cout << " (length: " << tour->getLength() << ")" << std::endl;
    }
    
    return 0;
}
