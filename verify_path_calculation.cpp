#include "aco/Graph.hpp"
#include "aco/Tour.hpp"
#include "aco/Ant.hpp"
#include "aco/AcoEngine.hpp"
#include <iostream>
#include <vector>
#include <memory>

int main() {
    std::cout << "=== TSP路徑長度計算驗證 ===" << std::endl;
    
    // 建立一個簡單的4城市測試圖
    auto graph = std::make_shared<Graph>(4);
    
    // 設定已知距離 (對稱矩陣)
    graph->setDistance(0, 1, 10.0);
    graph->setDistance(0, 2, 15.0);
    graph->setDistance(0, 3, 20.0);
    graph->setDistance(1, 2, 35.0);
    graph->setDistance(1, 3, 25.0);
    graph->setDistance(2, 3, 30.0);
    
    std::cout << "測試圖的距離矩陣:" << std::endl;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            std::cout << graph->getDistance(i, j) << "\t";
        }
        std::cout << std::endl;
    }
    
    // 測試幾種不同的路徑
    std::vector<std::vector<int>> test_paths = {
        {0, 1, 2, 3, 0},    // 完整循環
        {0, 1, 3, 2, 0},    // 不同順序的完整循環
        {0, 1, 2, 3},       // 不完整路徑 (應該自動添加回到起點)
        {0, 2, 1, 3}        // 另一個不完整路徑
    };
    
    std::cout << "\n路徑測試結果:" << std::endl;
    
    for (size_t i = 0; i < test_paths.size(); ++i) {
        auto& path = test_paths[i];
        Tour tour(graph);
        tour.setPath(path);
        
        std::cout << "路徑 " << (i+1) << ": ";
        for (size_t j = 0; j < path.size(); ++j) {
            std::cout << path[j];
            if (j < path.size() - 1) std::cout << " -> ";
        }
        
        // 手動計算預期長度
        double expected_length = 0.0;
        for (size_t j = 0; j < path.size() - 1; ++j) {
            expected_length += graph->getDistance(path[j], path[j+1]);
        }
        
        // 如果路徑不是完整循環，添加回到起點的距離
        if (path.front() != path.back()) {
            expected_length += graph->getDistance(path.back(), path.front());
            std::cout << " -> " << path.front() << " (自動添加)";
        }
        
        std::cout << std::endl;
        std::cout << "  計算長度: " << tour.getLength() << std::endl;
        std::cout << "  預期長度: " << expected_length << std::endl;
        std::cout << "  匹配: " << (tour.getLength() == expected_length ? "✓" : "✗") << std::endl;
        std::cout << std::endl;
    }
    
    return 0;
}
