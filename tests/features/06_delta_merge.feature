Feature: Delta pheromone merge
  Validate that merging local Δτ matrices to global τ is idempotent and mathematically correct.

  Background:
    Given a global pheromone matrix with initial values
    And multiple thread-local delta pheromone matrices

  Scenario: Global pheromone update after one iteration
    Given global pheromone matrix with values 1.0 everywhere
    And thread 1 has delta values: (0,1)=0.5, (1,2)=0.3
    And thread 2 has delta values: (0,1)=0.2, (2,3)=0.4
    When I merge all delta matrices to global pheromone
    Then global pheromone at (0,1) should be 1.0 + 0.5 + 0.2 = 1.7
    And global pheromone at (1,2) should be 1.0 + 0.3 = 1.3
    And global pheromone at (2,3) should be 1.0 + 0.4 = 1.4
    And all other positions should remain 1.0

  Scenario: Merge operation is commutative
    Given the same delta matrices from multiple threads
    When I merge them in different orders
    Then the final global pheromone matrix should be identical
    And the merge should be mathematically commutative

  Scenario: Empty delta matrices have no effect
    Given global pheromone matrix with specific values
    And empty (all-zero) delta matrices from all threads
    When I merge the delta matrices
    Then the global pheromone matrix should remain unchanged

  Scenario: Critical section ensures atomicity
    Given concurrent threads updating delta matrices
    When multiple threads merge simultaneously
    Then the final result should be equivalent to sequential merging
    And no race conditions should occur
