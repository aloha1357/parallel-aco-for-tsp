#include "aco/PheromoneModel.hpp"
#include "aco/ThreadLocalPheromoneModel.hpp"
#include <stdexcept>
#include <algorithm>
#include <mutex>

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
    
    // 關鍵：補上封閉邊的費洛蒙更新（回到起點）
    if (tour_path.size() >= 2) {
        int last = tour_path.back();
        int first = tour_path.front();
        
        // Validate city indices
        if (last < 0 || last >= size_ || first < 0 || first >= size_) {
            throw std::out_of_range("Tour contains invalid city index");
        }
        
        // Add delta pheromone for the closing edge
        double current_pheromone = pheromone_[last][first];
        double new_pheromone = current_pheromone + delta_tau;
        pheromone_[last][first] = std::max(new_pheromone, MIN_PHEROMONE);
    }
}

void PheromoneModel::mergeDeltas(const std::vector<ThreadLocalPheromoneModel>& delta_models) {
    // Thread-safe merge of multiple delta models
    std::lock_guard<std::mutex> lock(pheromone_mutex_);
    
    for (const auto& delta_model : delta_models) {
        if (delta_model.size() != size_) {
            throw std::invalid_argument("Delta model size must match pheromone matrix size");
        }
        
        // Merge each delta model
        for (int i = 0; i < size_; ++i) {
            for (int j = 0; j < size_; ++j) {
                double delta = delta_model.getDelta(i, j);
                if (delta != 0.0) {
                    double new_value = pheromone_[i][j] + delta;
                    // Ensure minimum pheromone level
                    pheromone_[i][j] = std::max(new_value, MIN_PHEROMONE);
                }
            }
        }
    }
}

void PheromoneModel::mergeDelta(const ThreadLocalPheromoneModel& delta_model) {
    // Thread-safe merge of a single delta model
    std::lock_guard<std::mutex> lock(pheromone_mutex_);
    
    if (delta_model.size() != size_) {
        throw std::invalid_argument("Delta model size must match pheromone matrix size");
    }
    
    // Merge the delta model
    for (int i = 0; i < size_; ++i) {
        for (int j = 0; j < size_; ++j) {
            double delta = delta_model.getDelta(i, j);
            if (delta != 0.0) {
                double new_value = pheromone_[i][j] + delta;
                // Ensure minimum pheromone level
                pheromone_[i][j] = std::max(new_value, MIN_PHEROMONE);
            }
        }
    }
}

int PheromoneModel::size() const {
    return size_;
}
