#pragma once

#include <vector>
#include <memory>

// Forward declaration
class Graph;

class Tour {
private:
    std::vector<int> path_;
    double length_;
    std::shared_ptr<Graph> graph_;

public:
    // Constructor
    Tour(std::shared_ptr<Graph> graph);
    
    // Get the tour path (sequence of city indices)
    const std::vector<int>& getPath() const;
    
    // Get total tour length
    double getLength() const;
    
    // Set the tour path and calculate length
    void setPath(const std::vector<int>& path);
    
    // Calculate tour length from current path
    void calculateLength();
};
