#include "aco/Graph.hpp"
#include <stdexcept>
#include <algorithm>

Graph::Graph(int size) : size_(size) {
    if (size <= 0) {
        throw std::invalid_argument("Graph size must be positive");
    }
    
    // Initialize distance matrix
    distances_.resize(size_);
    for (int i = 0; i < size_; ++i) {
        distances_[i].resize(size_);
    }
    
    // Initialize with random symmetric distances
    initializeRandomSymmetric();
}

double Graph::getDistance(int from, int to) const {
    if (from < 0 || from >= size_ || to < 0 || to >= size_) {
        throw std::out_of_range("City index out of range");
    }
    return distances_[from][to];
}

int Graph::size() const {
    return size_;
}

void Graph::setDistance(int from, int to, double distance) {
    if (from < 0 || from >= size_ || to < 0 || to >= size_) {
        throw std::out_of_range("City index out of range");
    }
    distances_[from][to] = distance;
    distances_[to][from] = distance; // Maintain symmetry
}

void Graph::initializeRandomSymmetric(unsigned int seed) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<double> dist(MIN_DISTANCE, MAX_DISTANCE);
    
    // Initialize diagonal to 0 (distance from city to itself)
    for (int i = 0; i < size_; ++i) {
        distances_[i][i] = 0.0;
    }
    
    // Initialize upper triangle with random values and copy to lower triangle for symmetry
    for (int i = 0; i < size_; ++i) {
        for (int j = i + 1; j < size_; ++j) {
            double distance = dist(rng);
            distances_[i][j] = distance;
            distances_[j][i] = distance; // Ensure symmetry
        }
    }
}
