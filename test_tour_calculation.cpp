#include <iostream>
#include <vector>
#include "include/aco/Graph.hpp"
#include "include/aco/Tour.hpp"

int main() {
    try {
        // Load the kroA100 instance
        auto graph = Graph::fromTSPFile("data/kroA100.tsp");
        
        // Read the optimal tour (manually input first few cities for testing)
        std::vector<int> optimal_tour = {0, 46, 92, 27, 66, 57, 60, 50, 86, 24, 80, 68, 63, 39}; // First part of optimal
        
        // Test 1: Tour with explicit return to start
        std::vector<int> path_with_return = optimal_tour;
        path_with_return.push_back(0); // Add return to start
        
        Tour tour1(graph);
        tour1.setPath(path_with_return);
        
        std::cout << "Tour with explicit return to start:" << std::endl;
        std::cout << "Path size: " << path_with_return.size() << std::endl;
        std::cout << "Calculated length: " << tour1.getLength() << std::endl;
        
        // Test 2: Tour without explicit return to start
        Tour tour2(graph);
        tour2.setPath(optimal_tour);
        
        std::cout << "\nTour without explicit return to start:" << std::endl;
        std::cout << "Path size: " << optimal_tour.size() << std::endl;
        std::cout << "Calculated length: " << tour2.getLength() << std::endl;
        
        // Manual calculation to verify
        double manual_length = 0.0;
        for (size_t i = 0; i + 1 < optimal_tour.size(); ++i) {
            manual_length += graph->getDistance(optimal_tour[i], optimal_tour[i + 1]);
        }
        manual_length += graph->getDistance(optimal_tour.back(), optimal_tour.front());
        
        std::cout << "\nManual calculation (without explicit return): " << manual_length << std::endl;
        
        // Check distance from last city back to first
        double return_distance = graph->getDistance(optimal_tour.back(), optimal_tour.front());
        std::cout << "Distance from last to first city: " << return_distance << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
