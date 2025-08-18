#include "aco/Graph.hpp"
#include <iostream>
#include <memory>

int main() {
    std::cout << "=== 測試 eil51.tsp ===" << std::endl;
    
    try {
        // 測試 eil51
        std::cout << "載入 eil51.tsp..." << std::endl;
        auto graph = Graph::fromTSPFile("data/eil51.tsp");
        std::cout << "成功載入，維度: " << graph->size() << std::endl;
        
        // 顯示前幾個距離作為示例
        std::cout << "\n距離示例 (前5個城市):" << std::endl;
        for (int i = 0; i < std::min(5, graph->size()); ++i) {
            for (int j = i + 1; j < std::min(5, graph->size()); ++j) {
                std::cout << "  城市 " << i << " -> " << j << ": " 
                          << graph->getDistance(i, j) << std::endl;
            }
        }
        
        // 計算一個簡單貪心解作為基準
        std::cout << "\n計算貪心解作為參考..." << std::endl;
        std::vector<int> greedy_path = {0}; // 從城市0開始
        std::vector<bool> visited(graph->size(), false);
        visited[0] = true;
        
        int current = 0;
        double total_length = 0.0;
        
        for (int step = 1; step < graph->size(); ++step) {
            int next = -1;
            double min_dist = std::numeric_limits<double>::max();
            
            for (int j = 0; j < graph->size(); ++j) {
                if (!visited[j]) {
                    double dist = graph->getDistance(current, j);
                    if (dist < min_dist) {
                        min_dist = dist;
                        next = j;
                    }
                }
            }
            
            if (next != -1) {
                greedy_path.push_back(next);
                visited[next] = true;
                total_length += min_dist;
                current = next;
            }
        }
        
        // 加上回到起點的距離
        total_length += graph->getDistance(current, 0);
        greedy_path.push_back(0);
        
        std::cout << "貪心解路徑長度: " << total_length << std::endl;
        std::cout << "貪心解路徑: ";
        for (size_t i = 0; i < std::min(size_t(10), greedy_path.size()); ++i) {
            std::cout << greedy_path[i] << " ";
            if (i == 9 && greedy_path.size() > 10) std::cout << "...";
        }
        std::cout << std::endl;
        
        std::cout << "\n注意：eil51 的已知最優解約為 426" << std::endl;
        std::cout << "我們的貪心解: " << total_length << " (相對差異: " 
                  << ((total_length - 426.0) / 426.0 * 100.0) << "%)" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "錯誤: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
