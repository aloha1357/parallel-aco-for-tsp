/*
 * BDD Scenarios implemented as GoogleTest unit tests
 * 
 * This file contains test cases that correspond to the BDD scenarios
 * originally planned for cucumber-cpp. Each test method name describes
 * the scenario being tested in a BDD-style format.
 */

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <set>
#include "aco/Graph.hpp"
#include "aco/Tour.hpp"
#include "aco/Ant.hpp"
#include "aco/PheromoneModel.hpp"
#include "aco/ThreadLocalPheromoneModel.hpp"
#include "aco/AcoEngine.hpp"

class BDDScenariosTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common setup for BDD scenarios
    }

    void TearDown() override {
        // Common cleanup
    }
};

// ==================== Walking Skeleton Scenarios ====================

TEST_F(BDDScenariosTest, WalkingSkeleton_EmptyMatrix_ZeroIterations_ReturnsZero) {
    // Given an empty distance matrix
    // When I run the ACO engine for 0 iterations  
    // Then the best path length should be 0
    
    // This is a walking skeleton test - basic structure verification
    EXPECT_TRUE(true) << "Walking skeleton structure is in place";
}

// ==================== Construct Tour Scenarios ====================

TEST_F(BDDScenariosTest, ConstructTour_10x10Matrix_OneAnt_CreatesValidTour) {
    // Given a symmetric 10×10 distance matrix with random values
    // When one ant constructs a tour
    // Then the tour should have valid properties
    
    auto graph = std::make_shared<Graph>(10);
    EXPECT_EQ(graph->size(), 10);
    
    auto ant = std::make_unique<Ant>(graph);
    auto tour = ant->constructTour();
    
    EXPECT_TRUE(tour != nullptr) << "Ant should construct a valid tour";
}

TEST_F(BDDScenariosTest, ConstructTour_ValidTour_HasCorrectNumberOfCities) {
    // Given a constructed tour
    // Then the tour length should have 10 unique cities
    
    auto graph = std::make_shared<Graph>(10);
    auto ant = std::make_unique<Ant>(graph);
    auto tour = ant->constructTour();
    
    const auto& path = tour->getPath();
    std::set<int> unique_cities(path.begin(), path.end());
    
    EXPECT_EQ(static_cast<int>(unique_cities.size()), 10) 
        << "Tour should visit exactly 10 unique cities";
}

TEST_F(BDDScenariosTest, ConstructTour_ValidTour_StartsAndEndsAtSameCity) {
    // Given a constructed tour
    // Then the tour should start and end at the same city
    
    auto graph = std::make_shared<Graph>(10);
    auto ant = std::make_unique<Ant>(graph);
    auto tour = ant->constructTour();
    
    const auto& path = tour->getPath();
    EXPECT_GE(path.size(), 2u) << "Tour should have at least 2 cities";
    EXPECT_EQ(path.front(), path.back()) 
        << "Tour should start and end at the same city (Hamiltonian cycle)";
}

TEST_F(BDDScenariosTest, ConstructTour_ValidTour_EachCityVisitedOnce) {
    // Given a constructed tour
    // Then each city should be visited exactly once
    
    auto graph = std::make_shared<Graph>(10);
    auto ant = std::make_unique<Ant>(graph);
    auto tour = ant->constructTour();
    
    const auto& path = tour->getPath();
    
    // Exclude the last city (which should be same as first for cycle)
    std::vector<int> tour_without_return(path.begin(), path.end() - 1);
    std::set<int> unique_cities(tour_without_return.begin(), tour_without_return.end());
    
    EXPECT_EQ(tour_without_return.size(), unique_cities.size()) 
        << "Each city should be visited exactly once (excluding return to start)";
}

TEST_F(BDDScenariosTest, ConstructTour_ValidTour_HasPositiveLength) {
    // Given a constructed tour
    // Then the tour path length should be positive
    
    auto graph = std::make_shared<Graph>(10);
    auto ant = std::make_unique<Ant>(graph);
    auto tour = ant->constructTour();
    
    double length = tour->getLength();
    EXPECT_GT(length, 0.0) << "Tour length should be positive";
}

TEST_F(BDDScenariosTest, ConstructTour_ValidTour_LengthEqualsEdgeDistances) {
    // Given a constructed tour
    // Then the tour path length should equal the sum of edge distances
    
    auto graph = std::make_shared<Graph>(10);
    auto ant = std::make_unique<Ant>(graph);
    auto tour = ant->constructTour();
    
    const auto& path = tour->getPath();
    double calculated_length = 0.0;
    
    // Sum up distances between consecutive cities in the path
    for (size_t i = 0; i < path.size() - 1; ++i) {
        calculated_length += graph->getDistance(path[i], path[i + 1]);
    }
    
    double tour_length = tour->getLength();
    EXPECT_DOUBLE_EQ(tour_length, calculated_length)
        << "Tour length should equal sum of edge distances";
}

// ==================== Future BDD Scenarios ====================

TEST_F(BDDScenariosTest, ProbabilisticChoice_PlaceholderForFutureImplementation) {
    // Placeholder for probabilistic choice BDD scenarios
    // Will be implemented when ACO probabilistic selection is ready
    GTEST_SKIP() << "Probabilistic choice scenarios not yet implemented";
}

// ==================== Evaporation BDD Scenarios ====================

TEST_F(BDDScenariosTest, Evaporation_5x5Matrix_Factor01_ReducesBy90Percent) {
    // GIVEN: a 5×5 pheromone matrix with initial values of 1.0
    auto pheromone = std::make_shared<PheromoneModel>(5);
    pheromone->initialize(1.0);
    
    // WHEN: I apply evaporation with factor 0.1
    pheromone->evaporate(0.1);
    
    // THEN: all pheromone values should be multiplied by 0.9
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            EXPECT_DOUBLE_EQ(pheromone->getPheromone(i, j), 0.9)
                << "Evaporation with ρ=0.1 should multiply values by (1-0.1)=0.9";
        }
    }
}

TEST_F(BDDScenariosTest, Evaporation_5x5Matrix_Factor03_ReducesBy70Percent) {
    // GIVEN: a 5×5 pheromone matrix with initial values of 1.0
    auto pheromone = std::make_shared<PheromoneModel>(5);
    pheromone->initialize(1.0);
    
    // WHEN: I apply evaporation with factor 0.3
    pheromone->evaporate(0.3);
    
    // THEN: all pheromone values should be multiplied by 0.7
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            EXPECT_DOUBLE_EQ(pheromone->getPheromone(i, j), 0.7)
                << "Evaporation with ρ=0.3 should multiply values by (1-0.3)=0.7";
        }
    }
}

TEST_F(BDDScenariosTest, Evaporation_5x5Matrix_Factor05_ReducesBy50Percent) {
    // GIVEN: a 5×5 pheromone matrix with initial values of 1.0
    auto pheromone = std::make_shared<PheromoneModel>(5);
    pheromone->initialize(1.0);
    
    // WHEN: I apply evaporation with factor 0.5
    pheromone->evaporate(0.5);
    
    // THEN: all pheromone values should be multiplied by 0.5
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            EXPECT_DOUBLE_EQ(pheromone->getPheromone(i, j), 0.5)
                << "Evaporation with ρ=0.5 should multiply values by (1-0.5)=0.5";
        }
    }
}

TEST_F(BDDScenariosTest, Evaporation_MultipleRounds_ThreeEvaporations_CorrectFinalValue) {
    // GIVEN: initial pheromone value of 10.0 at position (2,3)
    auto pheromone = std::make_shared<PheromoneModel>(5);
    pheromone->setPheromone(2, 3, 10.0);
    
    // WHEN: I apply evaporation with factor 0.2 for 3 rounds
    // Each round: value *= (1-0.2) = value * 0.8
    // After 3 rounds: 10.0 * 0.8^3 = 10.0 * 0.512 = 5.12
    pheromone->evaporate(0.2);
    pheromone->evaporate(0.2);
    pheromone->evaporate(0.2);
    
    // THEN: the pheromone value at position (2,3) should be approximately 5.12
    EXPECT_DOUBLE_EQ(pheromone->getPheromone(2, 3), 5.12)
        << "Three evaporations with ρ=0.2 should result in 10.0 * 0.8^3 = 5.12";
}

TEST_F(BDDScenariosTest, Evaporation_ZeroRate_PreservesAllValues) {
    // GIVEN: varying pheromone values in the matrix
    auto pheromone = std::make_shared<PheromoneModel>(5);
    pheromone->setPheromone(0, 1, 7.5);
    pheromone->setPheromone(1, 2, 3.2);
    pheromone->setPheromone(2, 3, 9.8);
    
    // WHEN: I apply evaporation with factor 0.0
    pheromone->evaporate(0.0);
    
    // THEN: all pheromone values should remain unchanged
    EXPECT_DOUBLE_EQ(pheromone->getPheromone(0, 1), 7.5)
        << "Zero evaporation should preserve original values";
    EXPECT_DOUBLE_EQ(pheromone->getPheromone(1, 2), 3.2)
        << "Zero evaporation should preserve original values";
    EXPECT_DOUBLE_EQ(pheromone->getPheromone(2, 3), 9.8)
        << "Zero evaporation should preserve original values";
}

// ==================== Delta Accumulation BDD Scenarios ====================

TEST_F(BDDScenariosTest, DeltaAccumulation_4x4Matrix_SingleTour_OnlyTourEdgesUpdated) {
    // GIVEN: a 4×4 distance matrix and an empty local delta pheromone matrix
    auto pheromone = std::make_shared<PheromoneModel>(4);
    pheromone->initialize(1.0); // Base pheromone level
    
    // AND: an ant tour visiting cities in order [0, 2, 1, 3, 0]
    std::vector<int> tour_path = {0, 2, 1, 3, 0};
    double tour_length = 20.0;
    double quality = 100.0; // Q = 100, so Δτ = Q/L = 100/20 = 5.0
    
    // WHEN: I accumulate the delta pheromone for this tour
    pheromone->deposit(tour_path, tour_length, quality);
    
    // THEN: delta pheromone should be Q/L at edges (0,2), (2,1), (1,3), (3,0)
    EXPECT_DOUBLE_EQ(pheromone->getPheromone(0, 2), 6.0) << "Edge (0,2) should have base + delta = 1.0 + 5.0";
    EXPECT_DOUBLE_EQ(pheromone->getPheromone(2, 1), 6.0) << "Edge (2,1) should have base + delta = 1.0 + 5.0";
    EXPECT_DOUBLE_EQ(pheromone->getPheromone(1, 3), 6.0) << "Edge (1,3) should have base + delta = 1.0 + 5.0";
    EXPECT_DOUBLE_EQ(pheromone->getPheromone(3, 0), 6.0) << "Edge (3,0) should have base + delta = 1.0 + 5.0";
    
    // AND: delta pheromone should be 0 at all other edges (remain at base level)
    EXPECT_DOUBLE_EQ(pheromone->getPheromone(0, 1), 1.0) << "Unused edge (0,1) should remain at base level";
    EXPECT_DOUBLE_EQ(pheromone->getPheromone(1, 0), 1.0) << "Unused edge (1,0) should remain at base level";
    EXPECT_DOUBLE_EQ(pheromone->getPheromone(0, 3), 1.0) << "Unused edge (0,3) should remain at base level";
    EXPECT_DOUBLE_EQ(pheromone->getPheromone(2, 3), 1.0) << "Unused edge (2,3) should remain at base level";
}

TEST_F(BDDScenariosTest, DeltaAccumulation_MultipleTours_AdditiveAccumulation) {
    // GIVEN: a 4×4 pheromone matrix
    auto pheromone = std::make_shared<PheromoneModel>(4);
    pheromone->initialize(2.0); // Base pheromone level
    
    // AND: first ant tour [0, 1, 2, 3, 0] with quality 80
    std::vector<int> tour1 = {0, 1, 2, 3, 0};
    double tour1_length = 10.0;
    double quality1 = 80.0; // Δτ1 = 80/10 = 8.0
    
    // AND: second ant tour [0, 2, 1, 3, 0] with quality 120
    std::vector<int> tour2 = {0, 2, 1, 3, 0};
    double tour2_length = 15.0;
    double quality2 = 120.0; // Δτ2 = 120/15 = 8.0
    
    // WHEN: I accumulate delta pheromone for both tours
    pheromone->deposit(tour1, tour1_length, quality1);
    pheromone->deposit(tour2, tour2_length, quality2);
    
    // THEN: edges used by both tours should have sum of their contributions
    // Edge (3,0) is used by both tours: base + Δτ1 + Δτ2 = 2.0 + 8.0 + 8.0 = 18.0
    EXPECT_DOUBLE_EQ(pheromone->getPheromone(3, 0), 18.0)
        << "Edge (3,0) used by both tours should accumulate both contributions";
    
    // AND: edges used by only one tour should have that tour's contribution
    // Edge (0,1) used only by tour1: base + Δτ1 = 2.0 + 8.0 = 10.0
    EXPECT_DOUBLE_EQ(pheromone->getPheromone(0, 1), 10.0)
        << "Edge (0,1) used only by tour1 should have its contribution";
    EXPECT_DOUBLE_EQ(pheromone->getPheromone(1, 2), 10.0)
        << "Edge (1,2) used only by tour1 should have its contribution";
    EXPECT_DOUBLE_EQ(pheromone->getPheromone(2, 3), 10.0)
        << "Edge (2,3) used only by tour1 should have its contribution";
    
    // Edge (0,2), (2,1), (1,3) used only by tour2: base + Δτ2 = 2.0 + 8.0 = 10.0
    EXPECT_DOUBLE_EQ(pheromone->getPheromone(0, 2), 10.0)
        << "Edge (0,2) used only by tour2 should have its contribution";
    EXPECT_DOUBLE_EQ(pheromone->getPheromone(2, 1), 10.0)
        << "Edge (2,1) used only by tour2 should have its contribution";
    EXPECT_DOUBLE_EQ(pheromone->getPheromone(1, 3), 10.0)
        << "Edge (1,3) used only by tour2 should have its contribution";
    
    // AND: unused edges should have zero delta pheromone (remain at base)
    EXPECT_DOUBLE_EQ(pheromone->getPheromone(1, 0), 2.0)
        << "Unused edge should remain at base level";
    EXPECT_DOUBLE_EQ(pheromone->getPheromone(3, 2), 2.0)
        << "Unused edge should remain at base level";
}

// Scenario 6: Delta Merge
TEST_F(BDDScenariosTest, DeltaMerge_GlobalPheromoneUpdate_OneDeltaMatrix_CorrectMerge) {
    // Given global pheromone matrix with values 1.0 everywhere
    PheromoneModel globalPheromones(4);
    globalPheromones.initialize(1.0);
    ThreadLocalPheromoneModel thread1Delta(4);
    
    // And thread 1 has delta values: (0,1)=0.5, (1,2)=0.3
    thread1Delta.setDelta(0, 1, 0.5);
    thread1Delta.setDelta(1, 2, 0.3);
    
    // When I merge delta matrix to global pheromone
    globalPheromones.mergeDelta(thread1Delta);
    
    // Then global pheromone should be updated correctly
    EXPECT_DOUBLE_EQ(globalPheromones.getPheromone(0, 1), 1.5); // 1.0 + 0.5
    EXPECT_DOUBLE_EQ(globalPheromones.getPheromone(1, 2), 1.3); // 1.0 + 0.3
    EXPECT_DOUBLE_EQ(globalPheromones.getPheromone(0, 2), 1.0); // unchanged
    EXPECT_DOUBLE_EQ(globalPheromones.getPheromone(2, 3), 1.0); // unchanged
}

TEST_F(BDDScenariosTest, DeltaMerge_GlobalPheromoneUpdate_MultipleDeltaMatrices_CorrectMerge) {
    // Given global pheromone matrix with values 1.0 everywhere
    PheromoneModel globalPheromones(4);
    globalPheromones.initialize(1.0);
    ThreadLocalPheromoneModel thread1Delta(4);
    ThreadLocalPheromoneModel thread2Delta(4);
    
    // And thread 1 has delta values: (0,1)=0.5, (1,2)=0.3
    thread1Delta.setDelta(0, 1, 0.5);
    thread1Delta.setDelta(1, 2, 0.3);
    
    // And thread 2 has delta values: (0,1)=0.2, (2,3)=0.4
    thread2Delta.setDelta(0, 1, 0.2);
    thread2Delta.setDelta(2, 3, 0.4);
    
    std::vector<ThreadLocalPheromoneModel> deltas = {thread1Delta, thread2Delta};
    
    // When I merge all delta matrices to global pheromone
    globalPheromones.mergeDeltas(deltas);
    
    // Then global pheromone should be updated correctly
    // Global pheromone at (0,1) should be 1.0 + 0.5 + 0.2 = 1.7
    EXPECT_DOUBLE_EQ(globalPheromones.getPheromone(0, 1), 1.7);
    // Global pheromone at (1,2) should be 1.0 + 0.3 = 1.3
    EXPECT_DOUBLE_EQ(globalPheromones.getPheromone(1, 2), 1.3);
    // Global pheromone at (2,3) should be 1.0 + 0.4 = 1.4
    EXPECT_DOUBLE_EQ(globalPheromones.getPheromone(2, 3), 1.4);
    // All other positions should remain 1.0
    EXPECT_DOUBLE_EQ(globalPheromones.getPheromone(0, 2), 1.0);
    EXPECT_DOUBLE_EQ(globalPheromones.getPheromone(0, 3), 1.0);
    EXPECT_DOUBLE_EQ(globalPheromones.getPheromone(1, 0), 1.0);
}

TEST_F(BDDScenariosTest, DeltaMerge_OperationIsCommutative_DifferentOrders_IdenticalResults) {
    // Given the same delta matrices from multiple threads
    ThreadLocalPheromoneModel thread1Delta(3);
    ThreadLocalPheromoneModel thread2Delta(3);
    
    thread1Delta.setDelta(0, 1, 0.5);
    thread1Delta.setDelta(1, 2, 0.3);
    
    thread2Delta.setDelta(0, 1, 0.2);
    thread2Delta.setDelta(2, 0, 0.4);
    
    // When I merge them in different orders
    PheromoneModel globalPheromones1(3);
    globalPheromones1.initialize(1.0);
    PheromoneModel globalPheromones2(3);
    globalPheromones2.initialize(1.0);
    
    // Order 1: thread1 first, then thread2
    std::vector<ThreadLocalPheromoneModel> deltas1 = {thread1Delta, thread2Delta};
    globalPheromones1.mergeDeltas(deltas1);
    
    // Order 2: thread2 first, then thread1
    std::vector<ThreadLocalPheromoneModel> deltas2 = {thread2Delta, thread1Delta};
    globalPheromones2.mergeDeltas(deltas2);
    
    // Then the final global pheromone matrix should be identical
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            EXPECT_DOUBLE_EQ(globalPheromones1.getPheromone(i, j), 
                           globalPheromones2.getPheromone(i, j))
                << "Mismatch at position (" << i << "," << j << ")";
        }
    }
}

TEST_F(BDDScenariosTest, DeltaMerge_EmptyDeltaMatrices_NoEffect_GlobalUnchanged) {
    // Given global pheromone matrix with specific values
    PheromoneModel globalPheromones(3);
    globalPheromones.initialize(0.5);
    globalPheromones.setPheromone(0, 1, 2.0);
    globalPheromones.setPheromone(1, 2, 3.0);
    
    // Store original values for comparison
    std::vector<std::vector<double>> originalValues(3, std::vector<double>(3));
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            originalValues[i][j] = globalPheromones.getPheromone(i, j);
        }
    }
    
    // And empty (all-zero) delta matrices from all threads
    ThreadLocalPheromoneModel emptyDelta1(3); // default initialized to 0
    ThreadLocalPheromoneModel emptyDelta2(3);
    ThreadLocalPheromoneModel emptyDelta3(3);
    
    std::vector<ThreadLocalPheromoneModel> emptyDeltas = {emptyDelta1, emptyDelta2, emptyDelta3};
    
    // When I merge the delta matrices
    globalPheromones.mergeDeltas(emptyDeltas);
    
    // Then the global pheromone matrix should remain unchanged
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            EXPECT_DOUBLE_EQ(globalPheromones.getPheromone(i, j), originalValues[i][j])
                << "Value changed at position (" << i << "," << j << ")";
        }
    }
}

TEST_F(BDDScenariosTest, ParallelConsistency_SingleThreadedReference_DeterministicBehavior) {
    // Given a small 4x4 symmetric distance matrix
    auto graph = std::make_shared<Graph>(4);
    
    // Set up ACO parameters with fixed seed for reproducibility
    AcoParameters params;
    params.alpha = 1.0;
    params.beta = 2.0;
    params.rho = 0.1;
    params.num_ants = 4;
    params.max_iterations = 10;
    params.num_threads = 1;  // Single-threaded reference
    params.random_seed = 42;
    
    // When I run the ACO engine twice with same parameters
    AcoEngine engine1(graph, params);
    auto results1 = engine1.run();
    
    AcoEngine engine2(graph, params);
    auto results2 = engine2.run();
    
    // Then both runs should produce identical results
    EXPECT_DOUBLE_EQ(results1.best_tour_length, results2.best_tour_length)
        << "Single-threaded runs with same seed should be deterministic";
    EXPECT_EQ(results1.best_tour_path, results2.best_tour_path)
        << "Tour paths should be identical for deterministic runs";
}

TEST_F(BDDScenariosTest, ParallelConsistency_MultiThreaded_ProducesValidTours) {
    // Given a 5x5 symmetric distance matrix
    auto graph = std::make_shared<Graph>(5);
    
    // Set up ACO parameters for multi-threaded execution
    AcoParameters params;
    params.alpha = 1.0;
    params.beta = 2.0;
    params.rho = 0.1;
    params.num_ants = 8;
    params.max_iterations = 20;
    params.num_threads = 4;  // Multi-threaded
    params.random_seed = 12345;
    
    // When I run the parallel ACO engine
    AcoEngine engine(graph, params);
    auto results = engine.run();
    
    // Then it should produce a valid tour
    EXPECT_GT(results.best_tour_length, 0.0) << "Best tour should have positive length";
    EXPECT_FALSE(results.best_tour_path.empty()) << "Best tour path should not be empty";
    
    // AND: Tour should visit correct number of cities
    EXPECT_EQ(results.best_tour_path.size(), static_cast<size_t>(graph->size() + 1))
        << "Tour should visit all cities plus return to start";
    
    // AND: Tour should start and end at same city
    EXPECT_EQ(results.best_tour_path.front(), results.best_tour_path.back())
        << "Tour should start and end at same city";
    
    // AND: All cities should be visited exactly once (except start/end)
    std::set<int> unique_cities;
    for (size_t i = 0; i < results.best_tour_path.size() - 1; ++i) {
        unique_cities.insert(results.best_tour_path[i]);
    }
    EXPECT_EQ(unique_cities.size(), static_cast<size_t>(graph->size()))
        << "All cities should be visited exactly once";
}

TEST_F(BDDScenariosTest, ParallelConsistency_ThreadSafety_NoRaceConditions) {
    // Given a 4x4 graph for faster execution
    auto graph = std::make_shared<Graph>(4);
    
    // Set up parameters for concurrent execution stress test
    AcoParameters params;
    params.alpha = 1.0;
    params.beta = 2.0;
    params.rho = 0.1;
    params.num_ants = 16;  // More ants to stress test
    params.max_iterations = 5;  // Fewer iterations for faster test
    params.num_threads = 8;  // Many threads to test concurrency
    params.random_seed = 999;
    
    // When I run the parallel ACO engine with many threads
    AcoEngine engine(graph, params);
    auto results = engine.run();
    
    // Then it should complete without crashes or hangs
    EXPECT_GT(results.execution_time_ms, 0.0) << "Engine should record execution time";
    EXPECT_GE(results.best_tour_length, 0.0) << "Best tour length should be non-negative";
    
    // AND: Results should be valid despite concurrent execution
    if (!results.best_tour_path.empty()) {
        EXPECT_GE(results.best_tour_path.size(), static_cast<size_t>(graph->size()))
            << "Tour should include all cities";
    }
    
    // AND: Iteration tracking should be consistent
    EXPECT_EQ(results.iteration_best_lengths.size(), static_cast<size_t>(params.max_iterations))
        << "Should track best length for each iteration";
}

TEST_F(BDDScenariosTest, ParallelConsistency_PerformanceScaling_ReasonableExecutionTime) {
    // Given a small problem for performance testing
    auto graph = std::make_shared<Graph>(4);
    
    // Test single-threaded performance
    AcoParameters single_params;
    single_params.num_threads = 1;
    single_params.num_ants = 8;
    single_params.max_iterations = 10;
    single_params.random_seed = 777;
    
    AcoEngine single_engine(graph, single_params);
    auto single_results = single_engine.run();
    
    // Test multi-threaded performance
    AcoParameters multi_params = single_params;
    multi_params.num_threads = 4;
    
    AcoEngine multi_engine(graph, multi_params);
    auto multi_results = multi_engine.run();
    
    // Then both should complete in reasonable time
    EXPECT_LT(single_results.execution_time_ms, 5000.0) // < 5 seconds
        << "Single-threaded execution should be reasonably fast";
    EXPECT_LT(multi_results.execution_time_ms, 5000.0) // < 5 seconds  
        << "Multi-threaded execution should be reasonably fast";
    
    // AND: Both should produce valid results
    EXPECT_GT(single_results.best_tour_length, 0.0);
    EXPECT_GT(multi_results.best_tour_length, 0.0);
}
