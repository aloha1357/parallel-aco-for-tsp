Feature: Reproducibility
  Validate that the ACO algorithm produces consistent, reproducible results when using the same random seed.

  Background:
    Given a fixed TSP instance "eil51.tsp"
    And standard ACO parameters

  Scenario Outline: Seed repeatability
    Given random seed <seed>
    When I run the ACO algorithm twice with the same seed
    Then both runs should produce identical best tour lengths
    And both runs should follow identical execution paths
    And the pheromone evolution should be identical

    Examples:
      | seed  |
      | 12345 |
      | 42    |
      | 99999 |

  Scenario: Cross-platform reproducibility
    Given the same random seed 777
    And the same TSP instance and parameters
    When I run the algorithm on different systems
    Then the results should be identical across platforms
    And floating-point calculations should be consistent

  Scenario: Thread count independence for reproducibility
    Given random seed 888
    When I run with 1 thread and then with 8 threads
    Then both runs should produce the same best tour (with proper synchronization)
    And the random number generation should be deterministic

  Scenario: Long-term reproducibility
    Given a specific seed and configuration
    When I run the algorithm multiple times over different sessions
    Then all runs should produce identical results
    And the implementation should be robust against compiler optimizations

  Scenario: Partial run reproducibility
    Given random seed 555
    When I run for 100 iterations and save the state
    And then resume for another 100 iterations
    Then the result should be identical to running 200 iterations continuously
