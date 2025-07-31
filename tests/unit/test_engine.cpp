#include <gtest/gtest.h>
#include "aco/AcoEngine.hpp"
#include "aco/Graph.hpp"
#include <memory>
#include <limits>

// Basic construction test with required parameters
TEST(AcoEngineTest, BasicConstruction) {
    auto graph = std::make_shared<Graph>(5);  // 5-city graph
    AcoParameters params;
    params.num_threads = 1;
    params.max_iterations = 10;
    params.num_ants = 5;
    
    AcoEngine engine(graph, params);
    EXPECT_EQ(engine.getBestTourLength(), std::numeric_limits<double>::max());
    EXPECT_TRUE(engine.getBestTour().empty());
}

TEST(AcoEngineTest, ParameterSetAndGet) {
    auto graph = std::make_shared<Graph>(4);
    AcoParameters params;
    params.alpha = 1.5;
    params.beta = 2.5;
    params.rho = 0.2;
    params.num_ants = 20;
    params.max_iterations = 50;
    params.num_threads = 4;
    params.random_seed = 12345;
    
    AcoEngine engine(graph, params);
    
    const auto& retrieved_params = engine.getParameters();
    EXPECT_DOUBLE_EQ(retrieved_params.alpha, 1.5);
    EXPECT_DOUBLE_EQ(retrieved_params.beta, 2.5);
    EXPECT_DOUBLE_EQ(retrieved_params.rho, 0.2);
    EXPECT_EQ(retrieved_params.num_ants, 20);
    EXPECT_EQ(retrieved_params.max_iterations, 50);
    EXPECT_EQ(retrieved_params.num_threads, 4);
    EXPECT_EQ(retrieved_params.random_seed, 12345);
}

TEST(AcoEngineTest, InvalidParametersThrow) {
    auto graph = std::make_shared<Graph>(4);
    
    // Test invalid num_ants
    AcoParameters params1;
    params1.num_ants = 0;
    EXPECT_THROW(AcoEngine engine(graph, params1), std::invalid_argument);
    
    // Test invalid max_iterations
    AcoParameters params2;
    params2.max_iterations = -1;
    EXPECT_THROW(AcoEngine engine(graph, params2), std::invalid_argument);
    
    // Test invalid num_threads
    AcoParameters params3;
    params3.num_threads = 0;
    EXPECT_THROW(AcoEngine engine(graph, params3), std::invalid_argument);
    
    // Test null graph
    EXPECT_THROW(AcoEngine engine(nullptr), std::invalid_argument);
}
