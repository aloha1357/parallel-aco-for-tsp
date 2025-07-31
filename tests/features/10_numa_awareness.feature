Feature: NUMA awareness
  Validate that the algorithm performs well on NUMA (Non-Uniform Memory Access) systems.

  Background:
    Given a two-socket NUMA system
    And a TSP instance large enough to benefit from NUMA optimization

  @numa
  Scenario: Two-socket system shows acceptable variance
    When I run the ACO engine with NUMA-aware thread binding
    Then the performance variance should be less than 10%
    And memory access patterns should favor local NUMA nodes
    And inter-socket communication should be minimized

  @numa
  Scenario: NUMA-aware memory allocation
    Given NUMA topology information is available
    When I allocate pheromone matrices and working memory
    Then memory should be allocated close to the threads that use it
    And memory bandwidth utilization should be optimized

  @numa
  Scenario: Thread affinity optimization
    Given a system with multiple NUMA nodes
    When I run with thread count equal to total cores
    Then threads should be bound to specific cores
    And work should be distributed evenly across NUMA nodes
    And cache locality should be maintained

  @numa
  Scenario: Scalability on large NUMA systems
    Given a system with 4 or more NUMA nodes
    When I run the ACO algorithm with maximum thread count
    Then performance should scale reasonably across all nodes
    And memory contention should be minimized
