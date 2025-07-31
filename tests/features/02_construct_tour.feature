Feature: Tour construction
  Validate that a single ant can construct a valid Hamiltonian tour visiting all cities exactly once.

  Background:
    Given a symmetric 10×10 distance matrix with random values

  Scenario: Single ant produces Hamiltonian tour
    When one ant constructs a tour
    Then the tour length should have 10 unique cities
    And the tour should start and end at the same city
    And each city should be visited exactly once

  Scenario: Tour has valid path length
    When one ant constructs a tour
    Then the tour path length should be positive
    And the tour path length should equal the sum of edge distances
