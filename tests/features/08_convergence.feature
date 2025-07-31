Feature: Algorithm convergence
  Validate that the ACO algorithm converges to reasonable solutions within expected iterations.

  Background:
    Given standard ACO parameters: alpha=1.0, beta=2.0, rho=0.1
    And sufficient number of ants (equal to number of cities)

  Scenario: Small TSP instance converges quickly
    Given a 51-city TSP instance "eil51.tsp"
    When I run the ACO algorithm for 250 iterations
    Then the best tour length should be within 5% of the known optimal (426)
    And the algorithm should show improvement over iterations

  Scenario: 100-city TSP converges within reasonable iterations
    Given a 100-city TSP instance
    When I run the ACO algorithm for 500 iterations
    Then the best tour length should be better than a random tour
    And the solution quality should stabilize in the final 50 iterations

  Scenario: Convergence monitoring
    Given any TSP instance
    When I run the ACO algorithm with convergence tracking
    Then the best solution should improve or stay the same each iteration
    And the improvement rate should decrease over time
    And early stopping should trigger if no improvement for 100 iterations

  Scenario: Population diversity maintains exploration
    Given a TSP instance with local optima traps
    When I run the ACO algorithm for 300 iterations
    Then the pheromone matrix should maintain diversity
    And the algorithm should explore multiple promising regions
