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

// ===== Evaporation Tests =====

TEST_F(PheromoneModelTest, EvaporateBasicFunction) {
    // Initialize with known values
    model->initialize(10.0);
    
    // Apply evaporation with ρ = 0.2 (should multiply by 0.8)
    model->evaporate(0.2);
    
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_DOUBLE_EQ(model->getPheromone(i, j), 8.0) 
                << "Evaporation should multiply by (1-ρ) = 0.8";
        }
    }
}

TEST_F(PheromoneModelTest, EvaporateMultipleValues) {
    // Set different values
    model->setPheromone(0, 1, 5.0);
    model->setPheromone(1, 2, 8.0);
    model->setPheromone(2, 3, 12.0);
    
    // Apply evaporation with ρ = 0.3 (should multiply by 0.7)
    model->evaporate(0.3);
    
    EXPECT_DOUBLE_EQ(model->getPheromone(0, 1), 3.5) << "5.0 * 0.7 = 3.5";
    EXPECT_DOUBLE_EQ(model->getPheromone(1, 2), 5.6) << "8.0 * 0.7 = 5.6";
    EXPECT_DOUBLE_EQ(model->getPheromone(2, 3), 8.4) << "12.0 * 0.7 = 8.4";
}

TEST_F(PheromoneModelTest, EvaporateZeroRatePreservesValues) {
    // Initialize with different values
    model->setPheromone(0, 1, 7.5);
    model->setPheromone(1, 2, 3.2);
    
    // Apply zero evaporation (ρ = 0.0, should multiply by 1.0)
    model->evaporate(0.0);
    
    EXPECT_DOUBLE_EQ(model->getPheromone(0, 1), 7.5) << "Zero evaporation should preserve values";
    EXPECT_DOUBLE_EQ(model->getPheromone(1, 2), 3.2) << "Zero evaporation should preserve values";
}

TEST_F(PheromoneModelTest, EvaporateEnforcesMinimumPheromone) {
    // Set a small value that would fall below minimum after evaporation
    model->setPheromone(0, 1, 0.05); // Above minimum (0.01)
    
    // Apply strong evaporation (ρ = 0.9, should multiply by 0.1)
    // 0.05 * 0.1 = 0.005, which is below MIN_PHEROMONE = 0.01
    model->evaporate(0.9);
    
    EXPECT_GE(model->getPheromone(0, 1), 0.01) << "Should enforce minimum pheromone level";
}

TEST_F(PheromoneModelTest, EvaporateInvalidRateThrows) {
    EXPECT_THROW(model->evaporate(-0.1), std::invalid_argument) << "Negative ρ should throw";
    EXPECT_THROW(model->evaporate(1.1), std::invalid_argument) << "ρ > 1.0 should throw";
}

// ===== Delta Accumulation (Deposition) Tests =====

TEST_F(PheromoneModelTest, DepositBasicFunction) {
    // Initialize with base values
    model->initialize(1.0);
    
    // Create a simple tour: 0 -> 1 -> 2 -> 0 (path length 3)
    std::vector<int> tour_path = {0, 1, 2, 0};
    double tour_length = 10.0;
    double quality = 100.0; // Q = 100, so Δτ = Q/L = 100/10 = 10.0
    
    // Apply deposition
    model->deposit(tour_path, tour_length, quality);
    
    // Check edges used in tour get increased pheromone
    EXPECT_DOUBLE_EQ(model->getPheromone(0, 1), 11.0) << "Edge (0,1) should have +10.0 pheromone";
    EXPECT_DOUBLE_EQ(model->getPheromone(1, 2), 11.0) << "Edge (1,2) should have +10.0 pheromone";
    EXPECT_DOUBLE_EQ(model->getPheromone(2, 0), 11.0) << "Edge (2,0) should have +10.0 pheromone";
    
    // Check unused edges remain unchanged
    EXPECT_DOUBLE_EQ(model->getPheromone(0, 2), 1.0) << "Unused edge (0,2) should remain unchanged";
    EXPECT_DOUBLE_EQ(model->getPheromone(1, 3), 1.0) << "Unused edge (1,3) should remain unchanged";
}

TEST_F(PheromoneModelTest, DepositMultipleTours) {
    // Initialize with base values
    model->initialize(2.0);
    
    // First tour: 0 -> 1 -> 0 (shares edge 0-1)
    std::vector<int> tour1 = {0, 1, 0};
    model->deposit(tour1, 5.0, 50.0); // Δτ = 50/5 = 10.0
    
    // Second tour: 0 -> 1 -> 2 -> 0 (also uses edge 0-1)
    std::vector<int> tour2 = {0, 1, 2, 0};
    model->deposit(tour2, 10.0, 80.0); // Δτ = 80/10 = 8.0
    
    // Edge (0,1) is used by both tours: 2.0 + 10.0 + 8.0 = 20.0
    EXPECT_DOUBLE_EQ(model->getPheromone(0, 1), 20.0) 
        << "Edge (0,1) should accumulate from both tours";
    
    // Edge (1,2) used only by second tour: 2.0 + 8.0 = 10.0
    EXPECT_DOUBLE_EQ(model->getPheromone(1, 2), 10.0)
        << "Edge (1,2) should have contribution from tour2 only";
    
    // Edge (2,0) used only by second tour: 2.0 + 8.0 = 10.0
    EXPECT_DOUBLE_EQ(model->getPheromone(2, 0), 10.0)
        << "Edge (2,0) should have contribution from tour2 only";
    
    // Unused edges remain at base value
    EXPECT_DOUBLE_EQ(model->getPheromone(0, 2), 2.0) << "Unused edge should remain unchanged";
}

TEST_F(PheromoneModelTest, DepositZeroQualityAddsNothing) {
    // Initialize with base values
    model->initialize(3.0);
    
    // Tour with zero quality
    std::vector<int> tour = {0, 1, 2, 0};
    model->deposit(tour, 10.0, 0.0); // Δτ = 0/10 = 0.0
    
    // All edges should remain unchanged
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_DOUBLE_EQ(model->getPheromone(i, j), 3.0)
                << "Zero quality should not change pheromone levels";
        }
    }
}

TEST_F(PheromoneModelTest, DepositInvalidInputThrows) {
    std::vector<int> valid_tour = {0, 1, 2, 0};
    std::vector<int> empty_tour = {};
    std::vector<int> single_city = {0};
    std::vector<int> out_of_range = {0, 1, 5, 0}; // city 5 doesn't exist
    
    EXPECT_THROW(model->deposit(empty_tour, 10.0, 100.0), std::invalid_argument) 
        << "Empty tour should throw";
    EXPECT_THROW(model->deposit(single_city, 10.0, 100.0), std::invalid_argument)
        << "Single city tour should throw";
    EXPECT_THROW(model->deposit(out_of_range, 10.0, 100.0), std::out_of_range)
        << "Out of range city should throw";
    EXPECT_THROW(model->deposit(valid_tour, -1.0, 100.0), std::invalid_argument)
        << "Negative tour length should throw";
    EXPECT_THROW(model->deposit(valid_tour, 0.0, 100.0), std::invalid_argument)
        << "Zero tour length should throw";
    EXPECT_THROW(model->deposit(valid_tour, 10.0, -50.0), std::invalid_argument)
        << "Negative quality should throw";
}
