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
    for (size_t i = 0; i + 1 < path_.size(); ++i) {
        length_ += graph_->getDistance(path_[i], path_[i + 1]);
    }
    
    // 關鍵：補上最後一段（封閉迴路）- TSP必須是完整循環
    if (path_.size() >= 2) {
        length_ += graph_->getDistance(path_.back(), path_.front());
    }
}
