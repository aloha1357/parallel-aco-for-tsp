#include <iostream>
#include <memory>
#include <iomanip>
#include "aco/Graph.hpp"
#include "aco/PheromoneModel.hpp"

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
    
    std::cout << "Distance matrix:" << std::endl;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            std::cout << std::setw(6) << std::fixed << std::setprecision(1) 
                      << graph->getDistance(i, j) << " ";
        }
        std::cout << std::endl;
    }
    
    std::cout << "\nPheromone matrix:" << std::endl;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            std::cout << std::setw(6) << std::fixed << std::setprecision(2) 
                      << pheromones->getPheromone(i, j) << " ";
        }
        std::cout << std::endl;
    }
    
    // Manual probability calculation for city 0
    double alpha = 1.0, beta = 2.0;
    int current_city = 0;
    
    std::cout << "\nManual probability calculation from city " << current_city << ":" << std::endl;
    std::cout << "Alpha = " << alpha << ", Beta = " << beta << std::endl;
    
    double total_attractiveness = 0.0;
    std::vector<double> attractiveness(4);
    
    for (int j = 1; j < 4; ++j) {  // Skip city 0 (already visited)
        double tau = pheromones->getPheromone(current_city, j);
        double distance = graph->getDistance(current_city, j);
        double eta = 1.0 / distance;  // Heuristic information
        
        double attr = std::pow(tau, alpha) * std::pow(eta, beta);
        attractiveness[j] = attr;
        total_attractiveness += attr;
        
        std::cout << "  City " << j << ": τ=" << tau << ", d=" << distance 
                  << ", η=" << eta << ", τ^α·η^β=" << attr << std::endl;
    }
    
    std::cout << "\nProbabilities:" << std::endl;
    for (int j = 1; j < 4; ++j) {
        double prob = attractiveness[j] / total_attractiveness;
        std::cout << "  P(0→" << j << ") = " << std::fixed << std::setprecision(4) 
                  << prob << std::endl;
    }
    
    // Test with biased pheromones
    std::cout << "\n=== Testing with biased pheromones ===" << std::endl;
    pheromones->setPheromone(0, 1, 5.0);
    pheromones->setPheromone(0, 2, 0.1);
    
    std::cout << "New pheromone matrix:" << std::endl;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            std::cout << std::setw(6) << std::fixed << std::setprecision(2) 
                      << pheromones->getPheromone(i, j) << " ";
        }
        std::cout << std::endl;
    }
    
    total_attractiveness = 0.0;
    for (int j = 1; j < 4; ++j) {
        double tau = pheromones->getPheromone(current_city, j);
        double distance = graph->getDistance(current_city, j);
        double eta = 1.0 / distance;
        
        double attr = std::pow(tau, alpha) * std::pow(eta, beta);
        attractiveness[j] = attr;
        total_attractiveness += attr;
        
        std::cout << "  City " << j << ": τ=" << tau << ", d=" << distance 
                  << ", η=" << eta << ", τ^α·η^β=" << attr << std::endl;
    }
    
    std::cout << "\nNew probabilities:" << std::endl;
    for (int j = 1; j < 4; ++j) {
        double prob = attractiveness[j] / total_attractiveness;
        std::cout << "  P(0→" << j << ") = " << std::fixed << std::setprecision(4) 
                  << prob << std::endl;
    }
    
    return 0;
}
