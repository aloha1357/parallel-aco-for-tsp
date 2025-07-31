Feature: Parallel consistency
  Validate that multi-threaded execution produces the same best tour length as single-threaded execution.

  Background:
    Given a symmetric 50×50 distance matrix loaded from "datasets/kroA50.dat"
    And ACO parameters: alpha=1.0, beta=2.0, rho=0.1, iterations=100
    And random seed is set to 42

  Scenario Outline: Result equivalence across thread counts
    When I run the ACO engine with <threads> thread(s) for 100 iterations and seed 42
    Then the best path length should equal the single-threaded result within 0.1% tolerance
    And the algorithm should converge to a reasonable solution

    Examples:
      | threads |
      | 1       |
      | 4       |
      | 8       |

  Scenario: Deterministic behavior with same seed
    Given the same random seed 12345
    When I run the parallel ACO engine twice with 4 threads
    Then both runs should produce identical best tour lengths
    And the tour paths should be identical

  Scenario: Thread safety validation
    When I run the ACO engine with 8 threads for 50 iterations
    Then no race conditions should occur
    And the pheromone matrix should remain valid (all positive values)
    And all threads should complete successfully
