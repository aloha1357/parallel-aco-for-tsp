#include "aco/ThreadLocalPheromoneModel.hpp"
#include <stdexcept>
#include <algorithm>

ThreadLocalPheromoneModel::ThreadLocalPheromoneModel(int size) : size_(size) {
    if (size <= 0) {
        throw std::invalid_argument("ThreadLocalPheromoneModel size must be positive");
    }
    
    // Initialize delta pheromone matrix with zeros
    delta_pheromone_.resize(size_);
    for (int i = 0; i < size_; ++i) {
        delta_pheromone_[i].resize(size_, 0.0);
    }
}

double ThreadLocalPheromoneModel::getDelta(int from, int to) const {
    if (from < 0 || from >= size_ || to < 0 || to >= size_) {
        throw std::out_of_range("City index out of range");
    }
    return delta_pheromone_[from][to];
}

void ThreadLocalPheromoneModel::setDelta(int from, int to, double value) {
    if (from < 0 || from >= size_ || to < 0 || to >= size_) {
        throw std::out_of_range("City index out of range");
    }
    delta_pheromone_[from][to] = value;
}

void ThreadLocalPheromoneModel::addDelta(int from, int to, double delta) {
    if (from < 0 || from >= size_ || to < 0 || to >= size_) {
        throw std::out_of_range("City index out of range");
    }
    delta_pheromone_[from][to] += delta;
}

void ThreadLocalPheromoneModel::accumulateDelta(const std::vector<int>& tour_path, double tour_length, double quality) {
    // Validate input parameters
    if (tour_path.empty()) {
        throw std::invalid_argument("Tour path cannot be empty");
    }
    
    if (tour_path.size() < 2) {
        throw std::invalid_argument("Tour path must contain at least 2 cities");
    }
    
    if (tour_length <= 0.0) {
        throw std::invalid_argument("Tour length must be positive");
    }
    
    if (quality < 0.0) {
        throw std::invalid_argument("Quality must be non-negative");
    }
    
    // Calculate delta pheromone: Δτ = Q / L
    double delta_tau = quality / tour_length;
    
    // Accumulate delta pheromone along each edge in the tour
    for (size_t i = 0; i < tour_path.size() - 1; ++i) {
        int from = tour_path[i];
        int to = tour_path[i + 1];
        
        // Validate city indices
        if (from < 0 || from >= size_ || to < 0 || to >= size_) {
            throw std::out_of_range("Tour contains invalid city index");
        }
        
        // Add delta pheromone to the edge
        delta_pheromone_[from][to] += delta_tau;
    }
}

void ThreadLocalPheromoneModel::reset() {
    for (int i = 0; i < size_; ++i) {
        for (int j = 0; j < size_; ++j) {
            delta_pheromone_[i][j] = 0.0;
        }
    }
}

int ThreadLocalPheromoneModel::size() const {
    return size_;
}
