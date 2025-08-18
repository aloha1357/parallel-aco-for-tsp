#include <iostream>
#include <vector>
#include <memory>

// Simplified test for Tour length calculation
class SimpleGraph {
private:
    std::vector<std::vector<double>> distances_;
    int size_;
    
public:
    SimpleGraph(int n) : size_(n) {
        distances_.resize(n);
        for (int i = 0; i < n; ++i) {
            distances_[i].resize(n);
        }
        
        // Initialize with some test distances
        distances_[0][1] = distances_[1][0] = 10;
        distances_[1][2] = distances_[2][1] = 15;
        distances_[2][3] = distances_[3][2] = 5;
        distances_[3][0] = distances_[0][3] = 30;
        
        // Self distances are 0
        for (int i = 0; i < n; ++i) {
            distances_[i][i] = 0;
        }
    }
    
    double getDistance(int from, int to) const {
        return distances_[from][to];
    }
    
    int size() const { return size_; }
};

class SimpleTour {
private:
    std::shared_ptr<SimpleGraph> graph_;
    std::vector<int> path_;
    double length_;
    
    void calculateLength() {
        if (path_.size() < 2) {
            length_ = 0.0;
            return;
        }
        
        length_ = 0.0;
        
        // Calculate distances between consecutive cities
        for (size_t i = 0; i + 1 < path_.size(); ++i) {
            length_ += graph_->getDistance(path_[i], path_[i + 1]);
        }
        
        // FIXED: Only add return edge if path doesn't already form a cycle
        if (path_.size() >= 2 && path_.back() != path_.front()) {
            length_ += graph_->getDistance(path_.back(), path_.front());
        }
    }
    
public:
    SimpleTour(std::shared_ptr<SimpleGraph> graph) : graph_(graph), length_(0.0) {}
    
    void setPath(const std::vector<int>& path) {
        path_ = path;
        calculateLength();
    }
    
    double getLength() const { return length_; }
    const std::vector<int>& getPath() const { return path_; }
};

int main() {
    auto graph = std::make_shared<SimpleGraph>(4);
    
    std::cout << "=== Testing Fixed Tour Length Calculation ===" << std::endl;
    
    // Test case 1: Path without explicit return (0->1->2->3)
    std::vector<int> path1 = {0, 1, 2, 3};
    SimpleTour tour1(graph);
    tour1.setPath(path1);
    
    std::cout << "Path 1 (no explicit return): ";
    for (int city : path1) std::cout << city << " ";
    std::cout << std::endl;
    std::cout << "Length: " << tour1.getLength() << std::endl;
    
    // Test case 2: Path with explicit return (0->1->2->3->0)
    std::vector<int> path2 = {0, 1, 2, 3, 0};
    SimpleTour tour2(graph);
    tour2.setPath(path2);
    
    std::cout << "\nPath 2 (with explicit return): ";
    for (int city : path2) std::cout << city << " ";
    std::cout << std::endl;
    std::cout << "Length: " << tour2.getLength() << std::endl;
    
    // Both should be equal now
    std::cout << "\nResults:" << std::endl;
    std::cout << "Both lengths equal: " << (tour1.getLength() == tour2.getLength() ? "YES" : "NO") << std::endl;
    std::cout << "Expected length: 60 (10+15+5+30)" << std::endl;
    
    // Test case 3: What if someone passes a malformed path that ends where it starts but is incomplete?
    std::vector<int> path3 = {0, 1, 0}; // This is weird but let's see
    SimpleTour tour3(graph);
    tour3.setPath(path3);
    
    std::cout << "\nPath 3 (malformed - partial cycle): ";
    for (int city : path3) std::cout << city << " ";
    std::cout << std::endl;
    std::cout << "Length: " << tour3.getLength() << std::endl;
    std::cout << "Expected: 20 (10+10, no extra return edge)" << std::endl;
    
    return 0;
}
