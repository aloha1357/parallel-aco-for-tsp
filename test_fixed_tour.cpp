#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include "include/aco/Graph.hpp"
#include "include/aco/Tour.hpp"

int main() {
    try {
        // Load the kroA100 instance
        auto graph = Graph::fromTSPFile("data/kroA100.tsp");
        
        // Read the optimal tour from file
        std::ifstream file("data/kroA100.opt.tour");
        std::string line;
        std::vector<int> optimal_tour;
        
        // Skip header until TOUR_SECTION
        while (std::getline(file, line)) {
            if (line.find("TOUR_SECTION") != std::string::npos) {
                break;
            }
        }
        
        // Read tour
        while (std::getline(file, line)) {
            if (line == "-1" || line == "EOF") break;
            int city = std::stoi(line);
            optimal_tour.push_back(city - 1); // Convert to 0-based
        }
        
        std::cout << "Loaded optimal tour with " << optimal_tour.size() << " cities" << std::endl;
        
        // Test 1: Tour without explicit return to start
        Tour tour1(graph);
        tour1.setPath(optimal_tour);
        
        std::cout << "\nTest 1 - Tour without explicit return:" << std::endl;
        std::cout << "First city: " << optimal_tour.front() << ", Last city: " << optimal_tour.back() << std::endl;
        std::cout << "Calculated length: " << tour1.getLength() << std::endl;
        
        // Test 2: Tour with explicit return to start (what Ant currently produces)
        std::vector<int> tour_with_return = optimal_tour;
        tour_with_return.push_back(optimal_tour.front()); // Add return to start
        
        Tour tour2(graph);
        tour2.setPath(tour_with_return);
        
        std::cout << "\nTest 2 - Tour with explicit return:" << std::endl;
        std::cout << "First city: " << tour_with_return.front() << ", Last city: " << tour_with_return.back() << std::endl;
        std::cout << "Calculated length: " << tour2.getLength() << std::endl;
        
        std::cout << "\nComparison:" << std::endl;
        std::cout << "Both should be equal: " << (tour1.getLength() == tour2.getLength() ? "YES" : "NO") << std::endl;
        std::cout << "Expected optimal length: 21282" << std::endl;
        std::cout << "Difference from expected: " << (tour1.getLength() - 21282) << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
