Feature: Delta pheromone accumulation
  Validate that local Δτ accumulates pheromone updates correctly for individual ant tours.

  Background:
    Given a 4×4 distance matrix
    And an empty local delta pheromone matrix

  Scenario: Local delta records only tour edges
    Given an ant tour visiting cities in order [0, 2, 1, 3, 0]
    And tour quality Q is 100
    When I accumulate the delta pheromone for this tour
    Then delta pheromone should be Q/L at edges (0,2), (2,1), (1,3), (3,0)
    And delta pheromone should be 0 at all other edges
    Where L is the tour length

  Scenario: Multiple tours accumulate additively
    Given first ant tour [0, 1, 2, 3, 0] with quality 80
    And second ant tour [0, 2, 1, 3, 0] with quality 120
    When I accumulate delta pheromone for both tours
    Then edges used by both tours should have sum of their contributions
    And edges used by only one tour should have that tour's contribution
    And unused edges should have zero delta pheromone

  Scenario: Thread-local delta isolation
    Given multiple ants running on different threads
    When each ant accumulates its delta pheromone locally
    Then each thread's delta matrix should be independent
    And no cross-thread contamination should occur
