#include "aco/TSPLibReader.hpp"
#include <iostream>
#include <fstream>

using namespace aco;

int main() {
    std::cout << "=== TSPLibReader 測試 ===" << std::endl;
    
    // 測試載入 berlin52 (使用相對路徑，從build目錄)
    std::string filename = "../data/berlin52.tsp";
    std::cout << "嘗試載入: " << filename << std::endl;
    
    auto graph = TSPLibReader::loadGraphFromTSPLib(filename);
    
    if (graph) {
        std::cout << "✅ 成功載入圖形" << std::endl;
        std::cout << "城市數量: " << graph->size() << std::endl;
        
        // 顯示前幾個城市的距離
        if (graph->size() >= 2) {
            std::cout << "距離 (0->1): " << graph->getDistance(0, 1) << std::endl;
            std::cout << "距離 (1->2): " << graph->getDistance(1, 2) << std::endl;
        }
        
        return 0;
    } else {
        std::cout << "❌ 載入失敗" << std::endl;
        
        // 檢查文件是否存在
        std::ifstream file(filename);
        if (file.good()) {
            std::cout << "文件存在，但載入失敗" << std::endl;
            // 顯示文件的前幾行
            std::string line;
            int lineCount = 0;
            std::cout << "文件內容預覽:" << std::endl;
            while (std::getline(file, line) && lineCount < 10) {
                std::cout << "  " << line << std::endl;
                lineCount++;
            }
        } else {
            std::cout << "文件不存在: " << filename << std::endl;
        }
        
        return 1;
    }
}
