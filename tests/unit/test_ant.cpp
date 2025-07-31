#include <gtest/gtest.h>
#include "aco/Ant.hpp"
#include "aco/Graph.hpp"
#include "aco/Tour.hpp"
#include <memory>
#include <set>

class AntTest : public ::testing::Test {
protected:
    void SetUp() override {
        graph = std::make_shared<Graph>(5);
    }
    
    std::shared_ptr<Graph> graph;
};

TEST_F(AntTest, BasicConstruction) {
    Ant ant(graph);
    // Just test that construction doesn't throw
    EXPECT_TRUE(true);
}

TEST_F(AntTest, ConstructValidTour) {
    Ant ant(graph);
    auto tour = ant.constructTour();
    
    EXPECT_TRUE(tour != nullptr);
    
    const auto& path = tour->getPath();
    EXPECT_EQ(path.size(), 6u); // 5 cities + return to start
    
    // Should start and end at the same city
    EXPECT_EQ(path.front(), path.back());
    
    // Should visit all cities exactly once (excluding return)
    std::vector<int> tour_without_return(path.begin(), path.end() - 1);
    std::set<int> unique_cities(tour_without_return.begin(), tour_without_return.end());
    EXPECT_EQ(unique_cities.size(), 5u);
    
    // All cities should be valid indices
    for (int city : path) {
        EXPECT_GE(city, 0);
        EXPECT_LT(city, 5);
    }
    
    // Tour length should be positive
    EXPECT_GT(tour->getLength(), 0.0);
}

TEST_F(AntTest, NullGraphThrows) {
    EXPECT_THROW(Ant(nullptr), std::invalid_argument);
}
