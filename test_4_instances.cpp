#include <iostream>
#include <vector>
#include <memory>
#include "aco/TSPLibReader.hpp"

int main() {
    std::cout << "=== Testing New 4-Instance Configuration ===" << std::endl;
    
    std::vector<std::pair<std::string, std::string>> instances = {
        {"eil51", "../data/eil51.tsp"},
        {"kroA100", "../data/kroA100.tsp"}, 
        {"kroA150", "../data/kroA150.tsp"},
        {"gr202", "../data/gr202.tsp"}
    };
    
    for (const auto& [name, filename] : instances) {
        std::cout << "\nTesting " << name << "..." << std::endl;
        
        try {
            auto graph = aco::TSPLibReader::loadGraphFromTSPLib(filename);
            if (graph) {
                std::cout << "  ✓ Successfully loaded " << name 
                          << " with " << graph->size() << " cities" << std::endl;
            } else {
                std::cout << "  ✗ Failed to load " << name << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "  ✗ Error loading " << name << ": " << e.what() << std::endl;
        }
    }
    
    std::cout << "\n=== Test Complete ===" << std::endl;
    return 0;
}
