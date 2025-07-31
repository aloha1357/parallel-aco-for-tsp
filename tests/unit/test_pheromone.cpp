#include <gtest/gtest.h>
#include "aco/PheromoneModel.hpp"

class PheromoneModelTest : public ::testing::Test {
protected:
    void SetUp() override {
        model = std::make_unique<PheromoneModel>(4);
    }
    
    std::unique_ptr<PheromoneModel> model;
};

TEST_F(PheromoneModelTest, BasicConstruction) {
    EXPECT_EQ(model->size(), 4);
    
    // Check default initialization
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_GT(model->getPheromone(i, j), 0.0) << "Pheromone should be positive";
        }
    }
}

TEST_F(PheromoneModelTest, SetAndGetPheromone) {
    model->setPheromone(1, 2, 5.5);
    EXPECT_DOUBLE_EQ(model->getPheromone(1, 2), 5.5);
}

TEST_F(PheromoneModelTest, MinimumPheromoneEnforced) {
    model->setPheromone(1, 2, 0.005); // Below minimum
    EXPECT_GE(model->getPheromone(1, 2), 0.01) << "Should enforce minimum pheromone level";
}

TEST_F(PheromoneModelTest, OutOfRangeThrows) {
    EXPECT_THROW(model->getPheromone(-1, 0), std::out_of_range);
    EXPECT_THROW(model->getPheromone(0, 4), std::out_of_range);
    EXPECT_THROW(model->setPheromone(4, 0, 1.0), std::out_of_range);
}

TEST_F(PheromoneModelTest, CustomInitialization) {
    model->initialize(3.5);
    
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_DOUBLE_EQ(model->getPheromone(i, j), 3.5);
        }
    }
}

TEST_F(PheromoneModelTest, InvalidSizeThrows) {
    EXPECT_THROW(PheromoneModel(-1), std::invalid_argument);
    EXPECT_THROW(PheromoneModel(0), std::invalid_argument);
}
