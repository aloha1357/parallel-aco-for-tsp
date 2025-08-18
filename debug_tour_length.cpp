#include <iostream>
#include <vector>
#include <memory>

// Minimal graph implementation for testing
class TestGraph {
public:
    TestGraph(const std::vector<std::vector<double>>& distances) : distances_(distances), size_(distances.size()) {}
    
    double getDistance(int from, int to) const {
        return distances_[from][to];
    }
    
    int size() const { return size_; }
    
private:
    std::vector<std::vector<double>> distances_;
    int size_;
};

// Minimal tour implementation for testing
class TestTour {
public:
    TestTour(std::shared_ptr<TestGraph> graph) : graph_(graph), length_(0.0) {}
    
    void setPath(const std::vector<int>& path) {
        path_ = path;
        calculateLength();
    }
    
    double getLength() const { return length_; }
    const std::vector<int>& getPath() const { return path_; }
    
private:
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
        
        // Add return edge if path is not already a complete cycle
        if (path_.size() >= 2) {
            length_ += graph_->getDistance(path_.back(), path_.front());
        }
    }
    
    std::shared_ptr<TestGraph> graph_;
    std::vector<int> path_;
    double length_;
};

int main() {
    // Create a simple 4-city test case
    std::vector<std::vector<double>> distances = {
        //   0    1    2    3
        {0.0, 10.0, 20.0, 30.0},  // City 0
        {10.0, 0.0, 15.0, 25.0},  // City 1  
        {20.0, 15.0, 0.0, 5.0},   // City 2
        {30.0, 25.0, 5.0, 0.0}    // City 3
    };
    
    auto graph = std::make_shared<TestGraph>(distances);
    
    // Test different path representations
    std::cout << "=== Testing Tour Length Calculation ===" << std::endl;
    
    // Test 1: Path without explicit return
    std::vector<int> path1 = {0, 1, 2, 3};
    TestTour tour1(graph);
    tour1.setPath(path1);
    
    std::cout << "Path without return: ";
    for (int city : path1) std::cout << city << " ";
    std::cout << std::endl;
    std::cout << "Calculated length: " << tour1.getLength() << std::endl;
    
    // Manual calculation
    double manual1 = 10 + 15 + 5 + 30; // 0->1->2->3->0
    std::cout << "Expected length: " << manual1 << std::endl;
    std::cout << "Match: " << (tour1.getLength() == manual1 ? "YES" : "NO") << std::endl;
    
    std::cout << std::endl;
    
    // Test 2: Path with explicit return 
    std::vector<int> path2 = {0, 1, 2, 3, 0};
    TestTour tour2(graph);
    tour2.setPath(path2);
    
    std::cout << "Path with return: ";
    for (int city : path2) std::cout << city << " ";
    std::cout << std::endl;
    std::cout << "Calculated length: " << tour2.getLength() << std::endl;
    
    // Manual calculation for path with explicit return
    double manual2 = 10 + 15 + 5 + 30 + 0; // 0->1->2->3->0->0 (last edge is 0)
    std::cout << "Expected length (if no double-counting): " << manual1 << std::endl;
    std::cout << "Actual would be (with double-counting): " << (manual1 + 0) << std::endl;
    std::cout << std::endl;
    
    // Test the issue: what if we have a path that ends at the start?
    std::vector<int> path3 = {0, 1, 2, 3, 0};
    
    // Manual calculation step by step for path3
    double step_by_step = 0;
    std::cout << "Step-by-step calculation for path with return:" << std::endl;
    for (size_t i = 0; i + 1 < path3.size(); ++i) {
        double dist = graph->getDistance(path3[i], path3[i + 1]);
        step_by_step += dist;
        std::cout << "  " << path3[i] << " -> " << path3[i + 1] << ": " << dist << std::endl;
    }
    
    // The Tour class would add this extra edge:
    double extra_edge = graph->getDistance(path3.back(), path3.front()); // 0 -> 0 = 0
    std::cout << "  Extra edge (last to first): " << path3.back() << " -> " << path3.front() << ": " << extra_edge << std::endl;
    
    double total_with_extra = step_by_step + extra_edge;
    std::cout << "Total with extra edge: " << total_with_extra << std::endl;
    std::cout << "Tour2 calculated: " << tour2.getLength() << std::endl;
    
    return 0;
}
