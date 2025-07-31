#include <gtest/gtest.h>
#include "aco/ThreadLocalPheromoneModel.hpp"

class ThreadLocalPheromoneModelTest : public ::testing::Test {
protected:
    void SetUp() override {
        model = std::make_unique<ThreadLocalPheromoneModel>(4);
    }
    
    std::unique_ptr<ThreadLocalPheromoneModel> model;
};

TEST_F(ThreadLocalPheromoneModelTest, BasicConstruction) {
    EXPECT_EQ(model->size(), 4);
    
    // Check all deltas are initially zero
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_DOUBLE_EQ(model->getDelta(i, j), 0.0) << "Initial delta should be zero";
        }
    }
}

TEST_F(ThreadLocalPheromoneModelTest, SetAndGetDelta) {
    model->setDelta(1, 2, 5.5);
    EXPECT_DOUBLE_EQ(model->getDelta(1, 2), 5.5);
}

TEST_F(ThreadLocalPheromoneModelTest, AddDelta) {
    model->setDelta(1, 2, 3.0);
    model->addDelta(1, 2, 2.5);
    EXPECT_DOUBLE_EQ(model->getDelta(1, 2), 5.5) << "AddDelta should accumulate values";
}

TEST_F(ThreadLocalPheromoneModelTest, AccumulateDeltaBasicFunction) {
    // Create a simple tour: 0 -> 1 -> 2 -> 0 (path length 3)
    std::vector<int> tour_path = {0, 1, 2, 0};
    double tour_length = 10.0;
    double quality = 100.0; // Q = 100, so Δτ = Q/L = 100/10 = 10.0
    
    // Apply accumulation
    model->accumulateDelta(tour_path, tour_length, quality);
    
    // Check edges used in tour get delta pheromone
    EXPECT_DOUBLE_EQ(model->getDelta(0, 1), 10.0) << "Edge (0,1) should have delta pheromone";
    EXPECT_DOUBLE_EQ(model->getDelta(1, 2), 10.0) << "Edge (1,2) should have delta pheromone";
    EXPECT_DOUBLE_EQ(model->getDelta(2, 0), 10.0) << "Edge (2,0) should have delta pheromone";
    
    // Check unused edges remain zero
    EXPECT_DOUBLE_EQ(model->getDelta(0, 2), 0.0) << "Unused edge (0,2) should remain zero";
    EXPECT_DOUBLE_EQ(model->getDelta(1, 3), 0.0) << "Unused edge (1,3) should remain zero";
}

TEST_F(ThreadLocalPheromoneModelTest, AccumulateMultipleTours) {
    // First tour: 0 -> 1 -> 0 (shares edge 0-1)
    std::vector<int> tour1 = {0, 1, 0};
    model->accumulateDelta(tour1, 5.0, 50.0); // Δτ = 50/5 = 10.0
    
    // Second tour: 0 -> 1 -> 2 -> 0 (also uses edge 0-1)
    std::vector<int> tour2 = {0, 1, 2, 0};
    model->accumulateDelta(tour2, 10.0, 80.0); // Δτ = 80/10 = 8.0
    
    // Edge (0,1) is used by both tours: 10.0 + 8.0 = 18.0
    EXPECT_DOUBLE_EQ(model->getDelta(0, 1), 18.0) 
        << "Edge (0,1) should accumulate from both tours";
    
    // Edge (1,2) used only by second tour: 8.0
    EXPECT_DOUBLE_EQ(model->getDelta(1, 2), 8.0)
        << "Edge (1,2) should have contribution from tour2 only";
    
    // Edge (2,0) used only by second tour: 8.0
    EXPECT_DOUBLE_EQ(model->getDelta(2, 0), 8.0)
        << "Edge (2,0) should have contribution from tour2 only";
}

TEST_F(ThreadLocalPheromoneModelTest, Reset) {
    // Set some delta values
    model->setDelta(0, 1, 5.0);
    model->setDelta(1, 2, 3.0);
    model->setDelta(2, 3, 7.0);
    
    // Reset
    model->reset();
    
    // Check all values are zero
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_DOUBLE_EQ(model->getDelta(i, j), 0.0) 
                << "All deltas should be zero after reset";
        }
    }
}

TEST_F(ThreadLocalPheromoneModelTest, OutOfRangeThrows) {
    EXPECT_THROW(model->getDelta(-1, 0), std::out_of_range);
    EXPECT_THROW(model->getDelta(0, 4), std::out_of_range);
    EXPECT_THROW(model->setDelta(4, 0, 1.0), std::out_of_range);
    EXPECT_THROW(model->addDelta(-1, 0, 1.0), std::out_of_range);
}

TEST_F(ThreadLocalPheromoneModelTest, AccumulateInvalidInputThrows) {
    std::vector<int> valid_tour = {0, 1, 2, 0};
    std::vector<int> empty_tour = {};
    std::vector<int> single_city = {0};
    std::vector<int> out_of_range = {0, 1, 5, 0}; // city 5 doesn't exist
    
    EXPECT_THROW(model->accumulateDelta(empty_tour, 10.0, 100.0), std::invalid_argument) 
        << "Empty tour should throw";
    EXPECT_THROW(model->accumulateDelta(single_city, 10.0, 100.0), std::invalid_argument)
        << "Single city tour should throw";
    EXPECT_THROW(model->accumulateDelta(out_of_range, 10.0, 100.0), std::out_of_range)
        << "Out of range city should throw";
    EXPECT_THROW(model->accumulateDelta(valid_tour, -1.0, 100.0), std::invalid_argument)
        << "Negative tour length should throw";
    EXPECT_THROW(model->accumulateDelta(valid_tour, 0.0, 100.0), std::invalid_argument)
        << "Zero tour length should throw";
    EXPECT_THROW(model->accumulateDelta(valid_tour, 10.0, -50.0), std::invalid_argument)
        << "Negative quality should throw";
}

TEST_F(ThreadLocalPheromoneModelTest, InvalidSizeThrows) {
    EXPECT_THROW(ThreadLocalPheromoneModel(-1), std::invalid_argument);
    EXPECT_THROW(ThreadLocalPheromoneModel(0), std::invalid_argument);
}
