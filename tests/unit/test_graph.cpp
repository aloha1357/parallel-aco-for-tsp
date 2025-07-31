#include <gtest/gtest.h>
#include "aco/Graph.hpp"

// Test Graph basic construction and properties
TEST(GraphTest, BasicConstruction) {
    Graph graph(5);
    EXPECT_EQ(graph.size(), 5);
}

TEST(GraphTest, SymmetricDistances) {
    Graph graph(3);
    
    // Test symmetry: distance(i,j) should equal distance(j,i)
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            EXPECT_DOUBLE_EQ(graph.getDistance(i, j), graph.getDistance(j, i))
                << "Distance matrix should be symmetric";
        }
    }
}

TEST(GraphTest, SelfDistanceIsZero) {
    Graph graph(4);
    
    // Test that distance from a city to itself is 0
    for (int i = 0; i < 4; ++i) {
        EXPECT_DOUBLE_EQ(graph.getDistance(i, i), 0.0)
            << "Distance from city to itself should be 0";
    }
}

TEST(GraphTest, InvalidSize) {
    EXPECT_THROW(Graph(-1), std::invalid_argument);
    EXPECT_THROW(Graph(0), std::invalid_argument);
}

TEST(GraphTest, OutOfRangeAccess) {
    Graph graph(3);
    
    EXPECT_THROW(graph.getDistance(-1, 0), std::out_of_range);
    EXPECT_THROW(graph.getDistance(0, 3), std::out_of_range);
    EXPECT_THROW(graph.getDistance(3, 0), std::out_of_range);
}
