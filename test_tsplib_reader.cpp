#include "aco/TSPLibReader.hpp"
#include <iostream>
#include <fstream>

using namespace aco;

int main() {
    std::cout << "=== TSPLibReader Test ===" << std::endl;
    
    // Test loading berlin52 (using relative path from build directory)
    std::string filename = "../data/berlin52.tsp";
    std::cout << "Attempting to load: " << filename << std::endl;
    
    auto graph = TSPLibReader::loadGraphFromTSPLib(filename);
    
    if (graph) {
        std::cout << "✅ Successfully loaded graph" << std::endl;
        std::cout << "Number of cities: " << graph->size() << std::endl;
        
        // Show distances between first few cities
        if (graph->size() >= 2) {
            std::cout << "Distance (0->1): " << graph->getDistance(0, 1) << std::endl;
            std::cout << "Distance (1->2): " << graph->getDistance(1, 2) << std::endl;
        }
        
        return 0;
    } else {
        std::cout << "❌ Loading failed" << std::endl;
        
        // Check if file exists
        std::ifstream file(filename);
        if (file.good()) {
            std::cout << "File exists, but loading failed" << std::endl;
            // Show first few lines of file
            std::string line;
            int lineCount = 0;
            std::cout << "File content preview:" << std::endl;
            while (std::getline(file, line) && lineCount < 10) {
                std::cout << "  " << line << std::endl;
                lineCount++;
            }
        } else {
            std::cout << "File does not exist: " << filename << std::endl;
        }
        
        return 1;
    }
}
