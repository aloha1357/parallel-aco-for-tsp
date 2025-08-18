#include "aco/Tour.hpp"
#include "aco/Graph.hpp"
#include <stdexcept>

Tour::Tour(std::shared_ptr<Graph> graph) : graph_(graph), length_(0.0) {
    if (!graph) {
        throw std::invalid_argument("Graph cannot be null");
    }
}

const std::vector<int>& Tour::getPath() const {
    return path_;
}

double Tour::getLength() const {
    return length_;
}

void Tour::setPath(const std::vector<int>& path) {
    path_ = path;
    calculateLength();
}

void Tour::calculateLength() {
    if (!graph_) {
        throw std::runtime_error("Graph is not available for length calculation");
    }
    
    if (path_.size() < 2) {
        length_ = 0.0;
        return;
    }
    
    length_ = 0.0;
    
    // Calculate distances between consecutive cities in the path
    for (size_t i = 0; i < path_.size() - 1; ++i) {
        length_ += graph_->getDistance(path_[i], path_[i + 1]);
    }
    
    // For TSP, we need a complete cycle
    // If the path doesn't already end at the starting city, add the return distance
    if (path_.size() > 2 && path_.front() != path_.back()) {
        length_ += graph_->getDistance(path_.back(), path_.front());
    }
}
