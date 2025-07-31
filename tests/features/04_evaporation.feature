Feature: Pheromone evaporation
  Validate that pheromone evaporation follows the τ ← (1-ρ)·τ rule correctly.

  Background:
    Given a 5×5 pheromone matrix with initial values of 1.0

  Scenario Outline: Evaporation factor reduces pheromone correctly
    When I apply evaporation with factor <rho>
    Then all pheromone values should be multiplied by <expected_factor>
    And no pheromone value should be negative

    Examples:
      | rho | expected_factor |
      | 0.1 | 0.9            |
      | 0.3 | 0.7            |
      | 0.5 | 0.5            |

  Scenario: Multiple evaporation rounds
    Given initial pheromone value of 10.0 at position (2,3)
    When I apply evaporation with factor 0.2 for 3 rounds
    Then the pheromone value at position (2,3) should be approximately 5.12
    And the pheromone matrix should remain symmetric

  Scenario: Zero evaporation rate preserves values
    Given varying pheromone values in the matrix
    When I apply evaporation with factor 0.0
    Then all pheromone values should remain unchanged
