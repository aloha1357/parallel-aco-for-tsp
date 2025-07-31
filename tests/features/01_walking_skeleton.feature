Feature: Walking skeleton
  Validate that the basic test infrastructure and CMake pipeline can execute successfully.
  This is the first step in BDD to ensure the testing framework is properly configured.

  Scenario: Walking skeleton
    Given an empty distance matrix
    When I run the ACO engine for 0 iteration
    Then the best path length should be 0
