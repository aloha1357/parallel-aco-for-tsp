#include "aco/PheromoneModel.hpp"
#include <stdexcept>
#include <algorithm>

PheromoneModel::PheromoneModel(int size) : size_(size) {
    if (size <= 0) {
        throw std::invalid_argument("PheromoneModel size must be positive");
    }
    
    // Initialize pheromone matrix
    pheromone_.resize(size_);
    for (int i = 0; i < size_; ++i) {
        pheromone_[i].resize(size_);
    }
    
    // Initialize with default values
    initialize();
}

double PheromoneModel::getPheromone(int from, int to) const {
    if (from < 0 || from >= size_ || to < 0 || to >= size_) {
        throw std::out_of_range("City index out of range");
    }
    return pheromone_[from][to];
}

void PheromoneModel::setPheromone(int from, int to, double value) {
    if (from < 0 || from >= size_ || to < 0 || to >= size_) {
        throw std::out_of_range("City index out of range");
    }
    
    // Ensure minimum pheromone level
    value = std::max(value, MIN_PHEROMONE);
    pheromone_[from][to] = value;
}

void PheromoneModel::initialize(double value) {
    value = std::max(value, MIN_PHEROMONE);
    
    for (int i = 0; i < size_; ++i) {
        for (int j = 0; j < size_; ++j) {
            pheromone_[i][j] = value;
        }
    }
}

int PheromoneModel::size() const {
    return size_;
}
