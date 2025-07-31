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

void PheromoneModel::evaporate(double rho) {
    // Validate evaporation rate
    if (rho < 0.0 || rho > 1.0) {
        throw std::invalid_argument("Evaporation rate must be between 0.0 and 1.0");
    }
    
    double retention_factor = 1.0 - rho;
    
    // Apply evaporation to all pheromone values
    for (int i = 0; i < size_; ++i) {
        for (int j = 0; j < size_; ++j) {
            double new_value = pheromone_[i][j] * retention_factor;
            // Enforce minimum pheromone level
            pheromone_[i][j] = std::max(new_value, MIN_PHEROMONE);
        }
    }
}

void PheromoneModel::deposit(const std::vector<int>& tour_path, double tour_length, double quality) {
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
    
    // Deposit pheromone along each edge in the tour
    for (size_t i = 0; i < tour_path.size() - 1; ++i) {
        int from = tour_path[i];
        int to = tour_path[i + 1];
        
        // Validate city indices
        if (from < 0 || from >= size_ || to < 0 || to >= size_) {
            throw std::out_of_range("Tour contains invalid city index");
        }
        
        // Add delta pheromone to the edge
        double current_pheromone = pheromone_[from][to];
        double new_pheromone = current_pheromone + delta_tau;
        
        // Ensure minimum pheromone level (though this should not be necessary for deposition)
        pheromone_[from][to] = std::max(new_pheromone, MIN_PHEROMONE);
    }
}

int PheromoneModel::size() const {
    return size_;
}
