#include <cucumber-cpp/autodetect.hpp>
#include <gtest/gtest.h>

int main(int argc, char** argv) {
    // Initialize GoogleTest
    ::testing::InitGoogleTest(&argc, argv);
    
    // Run cucumber-cpp
    return cucumber::BddRunner().run(argc, argv);
}
