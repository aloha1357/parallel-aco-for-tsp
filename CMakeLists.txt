cmake_minimum_required(VERSION 3.20)
project(ParallelACOForTSP LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Try to find OpenMP, but don't require it for walking skeleton
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP found and will be linked")
    set(HAS_OPENMP TRUE)
else()
    message(STATUS "OpenMP not found - building without parallel support for now")
    set(HAS_OPENMP FALSE)
endif()

# Add compiler flags for coverage and debugging
if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /Zi /RTC1")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
else()
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()

# Create directories for build outputs
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/coverage)

# Add include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Use FetchContent to download GoogleTest if not found locally
include(FetchContent)

# Try to find GoogleTest using vcpkg or system installation
find_package(GTest)
if(NOT GTest_FOUND)
    # Try alternative names
    find_package(googletest CONFIG)
endif()

if(NOT GTest_FOUND AND NOT googletest_FOUND)
    message(STATUS "GoogleTest not found locally - downloading via FetchContent")
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(googletest)
    set(BUILD_TESTS ON)
    message(STATUS "GoogleTest downloaded and configured successfully")
else()
    message(STATUS "GoogleTest found - enabling unit tests")
    set(BUILD_TESTS ON)
endif()

# Try to find cucumber-cpp (optional for now)
set(BUILD_BDD_TESTS OFF)
message(STATUS "BDD tests disabled for walking skeleton phase")

# Source files for the main library
set(ACO_SOURCES
    src/aco/Graph.cpp
    src/aco/Tour.cpp
    src/aco/Ant.cpp
    src/aco/PheromoneModel.cpp
    src/aco/AcoEngine.cpp
    # Add more source files as they are created
)

# Create main ACO library
add_library(aco_lib ${ACO_SOURCES})
target_include_directories(aco_lib PUBLIC 
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)

if(HAS_OPENMP)
    target_link_libraries(aco_lib PUBLIC OpenMP::OpenMP_CXX)
endif()

# Main executable (for manual testing)
add_executable(aco_main src/main.cpp)
target_link_libraries(aco_main aco_lib)

# Unit tests (if GoogleTest is available)
if(BUILD_TESTS)
    set(TEST_SOURCES
        tests/test_main.cpp
        tests/unit/test_graph.cpp
        tests/unit/test_tour.cpp
        tests/unit/test_ant.cpp
        tests/unit/test_pheromone.cpp
        tests/unit/test_engine.cpp
        # Add more test files as needed
    )

    add_executable(unit_tests ${TEST_SOURCES})
    
    if(TARGET GTest::gtest)
        target_link_libraries(unit_tests aco_lib GTest::gtest GTest::gtest_main)
    elseif(TARGET googletest::gtest)
        target_link_libraries(unit_tests aco_lib googletest::gtest googletest::gtest_main)
    elseif(TARGET gtest)
        # FetchContent targets
        target_link_libraries(unit_tests aco_lib gtest gtest_main)
    endif()
    
    if(NOT MSVC)
        target_link_libraries(unit_tests pthread)
    endif()
endif()

# BDD tests (if cucumber-cpp is available)
if(BUILD_BDD_TESTS)
    set(BDD_STEP_SOURCES
        tests/steps/walking_skeleton_steps.cpp
        tests/steps/construct_tour_steps.cpp
        tests/steps/probabilistic_choice_steps.cpp
        tests/steps/evaporation_steps.cpp
        tests/steps/delta_accumulation_steps.cpp
        tests/steps/delta_merge_steps.cpp
        tests/steps/parallel_consistency_steps.cpp
        tests/steps/convergence_steps.cpp
        tests/steps/performance_budget_steps.cpp
        tests/steps/numa_awareness_steps.cpp
        tests/steps/reproducibility_steps.cpp
    )
    
    add_executable(bdd_runner 
        tests/bdd_main.cpp
        ${BDD_STEP_SOURCES}
    )
    
    target_include_directories(bdd_runner PRIVATE ${CUCUMBER_CPP_INCLUDE_DIR})
    target_link_libraries(bdd_runner 
        aco_lib
        ${CUCUMBER_CPP_LIBRARY}
        GTest::gtest
        pthread
    )
endif()

# Enable testing
enable_testing()

# Add unit tests (if available)
if(BUILD_TESTS)
    add_test(NAME UnitTests COMMAND unit_tests)
endif()

# Add BDD tests (if available)
if(BUILD_BDD_TESTS)
    add_test(NAME BDDTests 
        COMMAND cucumber 
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
endif()

# Coverage target (Linux/GCC only)
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT MSVC)
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)
    
    if(LCOV_PATH AND GENHTML_PATH AND BUILD_TESTS)
        add_custom_target(coverage
            COMMAND ${LCOV_PATH} --directory . --zerocounters
            COMMAND $<TARGET_FILE:unit_tests>
            COMMAND ${LCOV_PATH} --directory . --capture --output-file coverage.info
            COMMAND ${LCOV_PATH} --remove coverage.info '/usr/*' --output-file coverage.info
            COMMAND ${LCOV_PATH} --remove coverage.info '*/tests/*' --output-file coverage.info
            COMMAND ${GENHTML_PATH} -o coverage coverage.info
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating code coverage report"
        )
    endif()
endif()

# Install targets
install(TARGETS aco_main DESTINATION bin)
install(DIRECTORY include/ DESTINATION include)

# Print configuration summary
message(STATUS "")
message(STATUS "Configuration Summary:")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  OpenMP: ${HAS_OPENMP}")
message(STATUS "  Unit Tests: ${BUILD_TESTS}")
message(STATUS "  BDD Tests: ${BUILD_BDD_TESTS}")
if(BUILD_TESTS)
    message(STATUS "  Coverage: ${LCOV_PATH}")
endif()
message(STATUS "")
