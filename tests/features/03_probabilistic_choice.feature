Feature: Probabilistic choice
  Validate that the ant's probabilistic city selection follows the τ^α·η^β rule.

  Background:
    Given a 3×3 distance matrix
    And pheromone values are set to specific test values
    And alpha parameter is 1.0 and beta parameter is 2.0

  Scenario: Higher pheromone increases selection probability
    Given an ant at city 0
    And city 1 has pheromone level 2.0
    And city 2 has pheromone level 1.0
    And both cities have equal distance (heuristic value)
    When the ant chooses the next city 1000 times
    Then city 1 should be chosen more frequently than city 2

  Scenario: Closer cities are preferred when pheromone is equal
    Given an ant at city 0
    And cities 1 and 2 have equal pheromone levels
    And city 1 is at distance 10 from city 0
    And city 2 is at distance 20 from city 0
    When the ant chooses the next city 1000 times
    Then city 1 should be chosen more frequently than city 2

  Scenario: Probability calculation follows formula
    Given an ant at city 0
    And specific pheromone and distance values
    When I calculate the selection probabilities
    Then the probabilities should match the τ^α·η^β formula
    And all probabilities should sum to 1.0
