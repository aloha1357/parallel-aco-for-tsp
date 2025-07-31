#include "aco/Ant.hpp"
#include "aco/Graph.hpp"
#include "aco/Tour.hpp"
#include "aco/PheromoneModel.hpp"
#include <stdexcept>
#include <vector>
#include <set>
#include <algorithm>
#include <limits>
#include <numeric>

Ant::Ant(std::shared_ptr<Graph> graph, 
         std::shared_ptr<PheromoneModel> pheromone,
         double alpha, 
         double beta,
         unsigned int seed) 
    : graph_(graph), pheromone_(pheromone), rng_(seed), external_rng_(nullptr),
      alpha_(alpha), beta_(beta), current_city_(0) {
    
    if (!graph) {
        throw std::invalid_argument("Graph cannot be null");
    }
    
    // Initialize visited array
    visited_.resize(graph_->size(), false);
}

Ant::Ant(std::shared_ptr<Graph> graph, 
         std::mt19937* external_rng,
         double alpha, 
         double beta)
    : graph_(graph), pheromone_(nullptr), rng_(0), external_rng_(external_rng),
      alpha_(alpha), beta_(beta), current_city_(0) {
    
    if (!graph) {
        throw std::invalid_argument("Graph cannot be null");
    }
    
    if (!external_rng) {
        throw std::invalid_argument("External RNG cannot be null");
    }
    
    // Initialize visited array
    visited_.resize(graph_->size(), false);
}

Ant::Ant(std::shared_ptr<Graph> graph,
         std::shared_ptr<PheromoneModel> pheromone,
         std::mt19937* external_rng,
         double alpha,
         double beta)
    : graph_(graph), pheromone_(pheromone), rng_(0), external_rng_(external_rng),
      alpha_(alpha), beta_(beta), current_city_(0) {
    
    if (!graph) {
        throw std::invalid_argument("Graph cannot be null");
    }
    
    if (!external_rng) {
        throw std::invalid_argument("External RNG cannot be null");
    }
    
    // Initialize visited array
    visited_.resize(graph_->size(), false);
}

std::unique_ptr<Tour> Ant::constructTour() {
    if (!graph_) {
        throw std::runtime_error("Graph is not available for tour construction");
    }
    
    std::vector<int> path;
    
    // Use ACO construction if pheromone model is available, otherwise use greedy
    if (pheromone_) {
        path = constructACOTour();
    } else {
        path = constructGreedyTour();
    }
    
    // Create tour and set the path
    auto tour = std::make_unique<Tour>(graph_);
    tour->setPath(path);
    
    return tour;
}

void Ant::setCurrentCity(int city) {
    if (city < 0 || city >= graph_->size()) {
        throw std::out_of_range("Invalid city index");
    }
    current_city_ = city;
    reset();
    markVisited(city);
}

int Ant::getCurrentCity() const {
    return current_city_;
}

int Ant::chooseNextCity() {
    if (!pheromone_) {
        throw std::runtime_error("Pheromone model required for probabilistic choice");
    }
    
    std::vector<double> probabilities = calculateSelectionProbabilities();
    
    // Find unvisited cities
    std::vector<int> available_cities;
    for (int i = 0; i < graph_->size(); ++i) {
        if (!visited_[i]) {
            available_cities.push_back(i);
        }
    }
    
    if (available_cities.empty()) {
        return -1; // No cities available
    }
    
    // Calculate total probability for available cities only
    double total_prob = 0.0;
    for (int city : available_cities) {
        total_prob += probabilities[city];
    }
    
    if (total_prob <= 0.0) {
        // Fallback: random selection if all probabilities are zero
        std::uniform_int_distribution<int> dist(0, available_cities.size() - 1);
        return available_cities[dist(getRNG())];
    }
    
    // Use roulette wheel selection with normalized probabilities
    std::uniform_real_distribution<double> dist(0.0, total_prob);
    double random_value = dist(getRNG());
    
    double cumulative_probability = 0.0;
    for (int city : available_cities) {
        cumulative_probability += probabilities[city];
        if (random_value <= cumulative_probability) {
            return city;
        }
    }
    
    // Fallback: return last available city
    return available_cities.back();
}

std::vector<double> Ant::calculateSelectionProbabilities() const {
    if (!pheromone_) {
        throw std::runtime_error("Pheromone model required for probability calculation");
    }
    
    int n = graph_->size();
    std::vector<double> probabilities(n, 0.0);
    
    // Calculate τ^α * η^β for each unvisited city
    double total_attractiveness = 0.0;
    for (int city = 0; city < n; ++city) {
        if (!visited_[city]) {
            double pheromone = pheromone_->getPheromone(current_city_, city);
            double distance = graph_->getDistance(current_city_, city);
            double heuristic = (distance > 0.0) ? (1.0 / distance) : 1.0; // η = 1/d
            
            double attractiveness = std::pow(pheromone, alpha_) * std::pow(heuristic, beta_);
            probabilities[city] = attractiveness;
            total_attractiveness += attractiveness;
        }
    }
    
    // Normalize to get probabilities
    if (total_attractiveness > 0.0) {
        for (int city = 0; city < n; ++city) {
            probabilities[city] /= total_attractiveness;
        }
    }
    
    return probabilities;
}

void Ant::markVisited(int city) {
    if (city >= 0 && city < static_cast<int>(visited_.size())) {
        visited_[city] = true;
    }
}

void Ant::reset() {
    std::fill(visited_.begin(), visited_.end(), false);
}

std::vector<int> Ant::constructGreedyTour() {
    int n = graph_->size();
    if (n == 0) {
        return {};
    }
    
    std::vector<int> path;
    std::set<int> visited;
    
    // Start from city 0
    int current_city = 0;
    path.push_back(current_city);
    visited.insert(current_city);
    
    // Visit all other cities (simple greedy: go to nearest unvisited city)
    while (visited.size() < static_cast<size_t>(n)) {
        int next_city = -1;
        double min_distance = std::numeric_limits<double>::max();
        
        for (int city = 0; city < n; ++city) {
            if (visited.find(city) == visited.end()) {
                double distance = graph_->getDistance(current_city, city);
                if (distance < min_distance) {
                    min_distance = distance;
                    next_city = city;
                }
            }
        }
        
        if (next_city != -1) {
            path.push_back(next_city);
            visited.insert(next_city);
            current_city = next_city;
        }
    }
    
    // Return to starting city to complete the cycle
    path.push_back(0);
    
    return path;
}

std::vector<int> Ant::constructACOTour() {
    int n = graph_->size();
    if (n == 0) {
        return {};
    }
    
    std::vector<int> path;
    
    // Reset and start from city 0
    reset();
    current_city_ = 0;
    markVisited(current_city_);
    path.push_back(current_city_);
    
    // Visit all other cities using probabilistic selection
    for (int step = 1; step < n; ++step) {
        int next_city = chooseNextCity();
        if (next_city == -1) {
            break; // No more cities available
        }
        
        path.push_back(next_city);
        markVisited(next_city);
        current_city_ = next_city;
    }
    
    // Return to starting city to complete the cycle
    path.push_back(0);
    
    return path;
}

std::mt19937& Ant::getRNG() {
    return external_rng_ ? *external_rng_ : rng_;
}
