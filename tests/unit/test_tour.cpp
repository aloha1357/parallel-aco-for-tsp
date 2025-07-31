#include <gtest/gtest.h>
#include "aco/Tour.hpp"
#include "aco/Graph.hpp"
#include <memory>

class TourTest : public ::testing::Test {
protected:
    void SetUp() override {
        graph = std::make_shared<Graph>(4);
    }
    
    std::shared_ptr<Graph> graph;
};

TEST_F(TourTest, BasicConstruction) {
    Tour tour(graph);
    EXPECT_EQ(tour.getLength(), 0.0);
    EXPECT_TRUE(tour.getPath().empty());
}

TEST_F(TourTest, SetValidPath) {
    Tour tour(graph);
    std::vector<int> path = {0, 1, 2, 3, 0};
    
    tour.setPath(path);
    
    EXPECT_EQ(tour.getPath(), path);
    EXPECT_GT(tour.getLength(), 0.0); // Should calculate positive length
}

TEST_F(TourTest, EmptyPathHasZeroLength) {
    Tour tour(graph);
    std::vector<int> empty_path;
    
    tour.setPath(empty_path);
    
    EXPECT_EQ(tour.getLength(), 0.0);
}

TEST_F(TourTest, SingleCityPathHasZeroLength) {
    Tour tour(graph);
    std::vector<int> single_city = {0};
    
    tour.setPath(single_city);
    
    EXPECT_EQ(tour.getLength(), 0.0);
}

TEST_F(TourTest, NullGraphThrows) {
    EXPECT_THROW(Tour(nullptr), std::invalid_argument);
}
