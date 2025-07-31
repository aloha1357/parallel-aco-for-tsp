#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <map>
#include <cmath>
#include <iostream>
#include "aco/Graph.hpp"
#include "aco/PheromoneModel.hpp"
#include "aco/Ant.hpp"

// For now, create simplified tests without cucumber-cpp
// We'll implement the same logic using GoogleTest

// Global test state
namespace {
    std::shared_ptr<Graph> test_graph;
    std::shared_ptr<PheromoneModel> test_pheromone;
    std::unique_ptr<Ant> test_ant;
    double alpha = 1.0;
    double beta = 2.0;
}

class ProbabilisticChoiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a 3x3 test graph
        test_graph = std::make_shared<Graph>(3);
        test_pheromone = std::make_shared<PheromoneModel>(3);
        
        // Set specific distance values for testing
        // We need to modify Graph to allow setting custom distances
        // For now, we'll work with the random distances
    }
    
    void TearDown() override {
        test_graph.reset();
        test_pheromone.reset();
        test_ant.reset();
    }
};

TEST_F(ProbabilisticChoiceTest, HigherPheromoneIncreasesSelectionProbability) {
    // Set equal distances to eliminate distance bias
    test_graph->setDistance(0, 1, 10.0);
    test_graph->setDistance(0, 2, 10.0);
    
    // Set pheromone values: city 1 has higher pheromone than city 2
    test_pheromone->setPheromone(0, 1, 2.0);
    test_pheromone->setPheromone(0, 2, 1.0);
    
    // Create ant at city 0
    test_ant = std::make_unique<Ant>(test_graph, test_pheromone, alpha, beta, 42);
    test_ant->setCurrentCity(0);
    
    // Count selections over many trials
    std::map<int, int> city_counts;
    const int trials = 1000;
    
    for (int trial = 0; trial < trials; ++trial) {
        // Reset ant state for each trial
        test_ant->setCurrentCity(0);
        
        int chosen_city = test_ant->chooseNextCity();
        if (chosen_city == 1 || chosen_city == 2) {
            city_counts[chosen_city]++;
        }
    }
    
    // City 1 (higher pheromone) should be chosen more often than city 2
    EXPECT_GT(city_counts[1], city_counts[2]) 
        << "City 1 (pheromone=2.0) should be chosen more than city 2 (pheromone=1.0)"
        << " - City 1: " << city_counts[1] << ", City 2: " << city_counts[2];
}

TEST_F(ProbabilisticChoiceTest, ProbabilityCalculationFollowsFormula) {
    // Set equal distances for controlled testing
    test_graph->setDistance(0, 1, 10.0);
    test_graph->setDistance(0, 2, 10.0);
    
    // Set specific pheromone values
    test_pheromone->setPheromone(0, 1, 2.0);
    test_pheromone->setPheromone(0, 2, 1.0);
    
    // Create ant at city 0
    test_ant = std::make_unique<Ant>(test_graph, test_pheromone, alpha, beta, 42);
    test_ant->setCurrentCity(0);
    
    // Calculate probabilities
    std::vector<double> probabilities = test_ant->calculateSelectionProbabilities();
    
    // Check that probabilities sum to 1.0 (for unvisited cities)
    double total_prob = 0.0;
    for (int i = 1; i < 3; ++i) { // Cities 1 and 2 are unvisited
        total_prob += probabilities[i];
    }
    
    EXPECT_NEAR(total_prob, 1.0, 1e-6) 
        << "Probabilities for unvisited cities should sum to 1.0";
    
    // Probability for visited city (0) should be 0
    EXPECT_DOUBLE_EQ(probabilities[0], 0.0) 
        << "Probability for current/visited city should be 0";
    
    // Manual calculation check with equal distances
    double d01 = 10.0, d02 = 10.0;
    double tau01 = 2.0, tau02 = 1.0;
    
    double attractiveness1 = std::pow(tau01, alpha) * std::pow(1.0/d01, beta);
    double attractiveness2 = std::pow(tau02, alpha) * std::pow(1.0/d02, beta);
    double total_attractiveness = attractiveness1 + attractiveness2;
    
    double expected_prob1 = attractiveness1 / total_attractiveness;
    double expected_prob2 = attractiveness2 / total_attractiveness;
    
    EXPECT_NEAR(probabilities[1], expected_prob1, 1e-6)
        << "Probability for city 1 should match τ^α·η^β formula";
    EXPECT_NEAR(probabilities[2], expected_prob2, 1e-6)
        << "Probability for city 2 should match τ^α·η^β formula";
}
