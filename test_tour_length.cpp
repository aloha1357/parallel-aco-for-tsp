#include "aco/Graph.hpp"
#include "aco/Tour.hpp"
#include <iostream>
#include <vector>
#include <memory>

int main() {
    // Create a simple 3-city graph for testing
    auto graph = std::make_shared<Graph>(3);
    
    // Set up a simple triangle with known distances
    // City 0 -> City 1: distance 10
    // City 1 -> City 2: distance 20  
    // City 2 -> City 0: distance 30
    graph->setDistance(0, 1, 10.0);
    graph->setDistance(1, 2, 20.0);
    graph->setDistance(2, 0, 30.0);
    
    // Test Tour with complete cycle path (including return to start)
    std::vector<int> complete_path = {0, 1, 2, 0};  // Complete cycle
    Tour tour_complete(graph);
    tour_complete.setPath(complete_path);
    
    std::cout << "=== Tour Length Calculation Test ===" << std::endl;
    std::cout << "Graph distances:" << std::endl;
    std::cout << "  0->1: " << graph->getDistance(0, 1) << std::endl;
    std::cout << "  1->2: " << graph->getDistance(1, 2) << std::endl;
    std::cout << "  2->0: " << graph->getDistance(2, 0) << std::endl;
    std::cout << std::endl;
    
    std::cout << "Complete path: ";
    for (size_t i = 0; i < complete_path.size(); ++i) {
        std::cout << complete_path[i];
        if (i < complete_path.size() - 1) std::cout << " -> ";
    }
    std::cout << std::endl;
    std::cout << "Calculated length: " << tour_complete.getLength() << std::endl;
    std::cout << "Expected length: " << (10.0 + 20.0 + 30.0) << std::endl;
    
    // Test Tour with incomplete path (not returning to start)
    std::vector<int> incomplete_path = {0, 1, 2};  // Missing return edge
    Tour tour_incomplete(graph);
    tour_incomplete.setPath(incomplete_path);
    
    std::cout << std::endl;
    std::cout << "Incomplete path: ";
    for (size_t i = 0; i < incomplete_path.size(); ++i) {
        std::cout << incomplete_path[i];
        if (i < incomplete_path.size() - 1) std::cout << " -> ";
    }
    std::cout << std::endl;
    std::cout << "Calculated length: " << tour_incomplete.getLength() << std::endl;
    std::cout << "Expected length: " << (10.0 + 20.0 + 30.0) << std::endl;
    std::cout << "(Should automatically add return edge 2->0)" << std::endl;
    
    return 0;
}
