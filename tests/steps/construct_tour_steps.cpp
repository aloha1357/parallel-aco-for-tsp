#include <gtest/gtest.h>
#include <cucumber-cpp/defs.hpp>
#include <memory>
#include <vector>
#include <set>
#include "aco/Graph.hpp"
#include "aco/Tour.hpp"
#include "aco/Ant.hpp"

// Global test state for step definitions
namespace {
    std::shared_ptr<Graph> test_graph;
    std::unique_ptr<Tour> constructed_tour;
    std::unique_ptr<Ant> test_ant;
}

GIVEN("^a symmetric (\\d+)×(\\d+) distance matrix with random values$") {
    REGEX_PARAM(int, rows);
    REGEX_PARAM(int, cols);
    
    EXPECT_EQ(rows, cols) << "Distance matrix must be square for TSP";
    
    // Create a 10x10 test graph - this will fail until we implement Graph properly
    test_graph = std::make_shared<Graph>(rows);
    
    // This should pass once we implement the Graph class
    EXPECT_EQ(test_graph->size(), rows);
}

WHEN("^one ant constructs a tour$") {
    EXPECT_TRUE(test_graph != nullptr) << "Graph must be initialized first";
    
    // Create an ant and let it construct a tour - this will fail until implemented
    test_ant = std::make_unique<Ant>(test_graph);
    constructed_tour = test_ant->constructTour();
    
    EXPECT_TRUE(constructed_tour != nullptr) << "Ant should construct a valid tour";
}

THEN("^the tour length should have (\\d+) unique cities$") {
    REGEX_PARAM(int, expected_cities);
    
    EXPECT_TRUE(constructed_tour != nullptr) << "Tour must be constructed first";
    
    // Get the tour path and check unique cities
    const auto& path = constructed_tour->getPath();
    std::set<int> unique_cities(path.begin(), path.end());
    
    EXPECT_EQ(static_cast<int>(unique_cities.size()), expected_cities) 
        << "Tour should visit exactly " << expected_cities << " unique cities";
}

THEN("^the tour should start and end at the same city$") {
    EXPECT_TRUE(constructed_tour != nullptr) << "Tour must be constructed first";
    
    const auto& path = constructed_tour->getPath();
    EXPECT_GE(path.size(), 2u) << "Tour should have at least 2 cities";
    
    EXPECT_EQ(path.front(), path.back()) 
        << "Tour should start and end at the same city (Hamiltonian cycle)";
}

THEN("^each city should be visited exactly once$") {
    EXPECT_TRUE(constructed_tour != nullptr) << "Tour must be constructed first";
    
    const auto& path = constructed_tour->getPath();
    
    // Exclude the last city (which should be same as first for cycle)
    std::vector<int> tour_without_return(path.begin(), path.end() - 1);
    std::set<int> unique_cities(tour_without_return.begin(), tour_without_return.end());
    
    EXPECT_EQ(tour_without_return.size(), unique_cities.size()) 
        << "Each city should be visited exactly once (excluding return to start)";
}

THEN("^the tour path length should be positive$") {
    EXPECT_TRUE(constructed_tour != nullptr) << "Tour must be constructed first";
    
    double length = constructed_tour->getLength();
    EXPECT_GT(length, 0.0) << "Tour length should be positive";
}

THEN("^the tour path length should equal the sum of edge distances$") {
    EXPECT_TRUE(constructed_tour != nullptr) << "Tour must be constructed first";
    EXPECT_TRUE(test_graph != nullptr) << "Graph must be available";
    
    const auto& path = constructed_tour->getPath();
    double calculated_length = 0.0;
    
    // Sum up distances between consecutive cities in the path
    for (size_t i = 0; i < path.size() - 1; ++i) {
        calculated_length += test_graph->getDistance(path[i], path[i + 1]);
    }
    
    double tour_length = constructed_tour->getLength();
    EXPECT_DOUBLE_EQ(tour_length, calculated_length)
        << "Tour length should equal sum of edge distances";
}
