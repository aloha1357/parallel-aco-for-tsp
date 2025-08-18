#include "aco/Graph.hpp"
#include <iostream>
#include <memory>
#include <fstream>

int main() {
    std::cout << "=== TSPLIB 距離計算測試 ===" << std::endl;
    
    try {
        // 測試 berlin52
        if (std::ifstream("data/berlin52.tsp")) {
            std::cout << "\n載入 berlin52.tsp..." << std::endl;
            auto graph = Graph::fromTSPFile("data/berlin52.tsp");
            std::cout << "成功載入，維度: " << graph->size() << std::endl;
            
            // 顯示前幾個距離作為示例
            std::cout << "距離示例:" << std::endl;
            for (int i = 0; i < std::min(5, graph->size()); ++i) {
                for (int j = i + 1; j < std::min(5, graph->size()); ++j) {
                    std::cout << "  城市 " << i << " -> " << j << ": " 
                              << graph->getDistance(i, j) << std::endl;
                }
            }
        } else {
            std::cout << "berlin52.tsp 文件不存在，跳過測試" << std::endl;
        }
        
        // 測試 kroA100
        if (std::ifstream("data/kroA100.tsp")) {
            std::cout << "\n載入 kroA100.tsp..." << std::endl;
            auto graph = Graph::fromTSPFile("data/kroA100.tsp");
            std::cout << "成功載入，維度: " << graph->size() << std::endl;
            
            // 顯示前幾個距離作為示例
            std::cout << "距離示例:" << std::endl;
            for (int i = 0; i < std::min(3, graph->size()); ++i) {
                for (int j = i + 1; j < std::min(3, graph->size()); ++j) {
                    std::cout << "  城市 " << i << " -> " << j << ": " 
                              << graph->getDistance(i, j) << std::endl;
                }
            }
        } else {
            std::cout << "kroA100.tsp 文件不存在，跳過測試" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "錯誤: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
