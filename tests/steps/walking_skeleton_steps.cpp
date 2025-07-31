#include <gtest/gtest.h>
#include <cucumber-cpp/defs.hpp>
#include <memory>

// Mock classes for walking skeleton - will be replaced with real implementations
class MockDistanceMatrix {
public:
    MockDistanceMatrix() = default;
    bool empty() const { return true; }
};

class MockAcoEngine {
private:
    std::shared_ptr<MockDistanceMatrix> matrix_;
    
public:
    MockAcoEngine(std::shared_ptr<MockDistanceMatrix> matrix) : matrix_(matrix) {}
    
    void run(int iterations) {
        // Walking skeleton - do nothing for 0 iterations
        EXPECT_EQ(iterations, 0);
    }
    
    double getBestPathLength() const {
        return 0.0; // Walking skeleton returns 0
    }
};

// Step definitions for Walking Skeleton feature
namespace {
    std::shared_ptr<MockDistanceMatrix> distance_matrix;
    std::unique_ptr<MockAcoEngine> aco_engine;
    double best_path_length = 0.0;
}

GIVEN("^an empty distance matrix$") {
    distance_matrix = std::make_shared<MockDistanceMatrix>();
    EXPECT_TRUE(distance_matrix->empty());
}

WHEN("^I run the ACO engine for (\\d+) iteration$") {
    REGEX_PARAM(int, iterations);
    
    aco_engine = std::make_unique<MockAcoEngine>(distance_matrix);
    aco_engine->run(iterations);
    best_path_length = aco_engine->getBestPathLength();
}

THEN("^the best path length should be (\\d+)$") {
    REGEX_PARAM(double, expected_length);
    
    EXPECT_DOUBLE_EQ(best_path_length, expected_length);
}
