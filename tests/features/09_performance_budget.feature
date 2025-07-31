Feature: Performance budget
  Validate that the parallel ACO implementation meets performance requirements and scalability targets.

  Background:
    Given a modern multi-core CPU with at least 8 cores
    And a 100-city TSP instance

  Scenario: 8-core run completes within time budget
    When I run the ACO engine with 8 threads for 200 iterations
    Then the execution should complete within 2 seconds
    And memory usage should remain reasonable (< 1GB)

  Scenario Outline: Speedup validation
    Given baseline single-thread performance
    When I run with <threads> threads
    Then the speedup should be at least <min_speedup>x
    And efficiency should be at least <min_efficiency>%

    Examples:
      | threads | min_speedup | min_efficiency |
      | 2       | 1.5         | 75            |
      | 4       | 3.0         | 75            |
      | 8       | 6.0         | 75            |

  Scenario: Large instance performance
    Given a 442-city TSP instance "pcb442.tsp"
    When I run the ACO engine with 8 threads for 100 iterations
    Then the execution should complete within 30 seconds
    And the solution quality should be reasonable

  Scenario: Memory efficiency
    Given any TSP instance with N cities
    When I run the ACO algorithm
    Then memory usage should scale as O(N²) for pheromone matrix
    And additional thread overhead should be minimal
    And no memory leaks should occur during execution
