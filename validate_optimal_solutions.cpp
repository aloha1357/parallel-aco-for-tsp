#include "aco/Graph.hpp"
#include "aco/Tour.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <fstream>

// 讀取 .opt.tour 文件的函數
std::vector<int> readOptimalTour(const std::string& filename) {
    std::vector<int> tour;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cout << "無法打開最優解文件: " << filename << std::endl;
        return tour;
    }
    
    std::string line;
    bool in_tour_section = false;
    
    while (std::getline(file, line)) {
        if (line.find("TOUR_SECTION") != std::string::npos) {
            in_tour_section = true;
            continue;
        }
        
        if (line == "EOF" || line == "-1") {
            break;
        }
        
        if (in_tour_section && !line.empty()) {
            try {
                int city = std::stoi(line.substr(0, line.find_first_of(" \t")));
                if (city > 0) {  // TSPLIB 使用 1-based 索引
                    tour.push_back(city - 1);  // 轉換為 0-based
                }
            } catch (const std::exception& e) {
                // 忽略解析錯誤
            }
        }
    }
    
    return tour;
}

int main() {
    std::cout << "=== 驗證 TSP 實例和最優解 ===" << std::endl;
    
    // 測試 eil51
    try {
        std::cout << "\n載入 eil51.tsp..." << std::endl;
        auto graph = Graph::fromTSPFile("data/eil51.tsp");
        std::cout << "成功載入，維度: " << graph->size() << std::endl;
        
        // 檢查是否有最優解文件
        std::vector<int> optimal_tour = readOptimalTour("data/eil51.opt.tour");
        
        if (!optimal_tour.empty()) {
            std::cout << "找到最優解文件，路徑長度為: " << optimal_tour.size() << " 個城市" << std::endl;
            
            // 計算最優解的實際長度
            Tour tour(graph);
            tour.setPath(optimal_tour);
            
            std::cout << "計算的最優解長度: " << tour.getLength() << std::endl;
            std::cout << "已知最優解: 426" << std::endl;
            std::cout << "差異: " << (tour.getLength() - 426.0) << std::endl;
        } else {
            std::cout << "未找到最優解文件，創建一個測試路徑..." << std::endl;
            
            // 創建一個簡單的測試路徑 (順序訪問)
            std::vector<int> test_path;
            for (int i = 0; i < graph->size(); ++i) {
                test_path.push_back(i);
            }
            
            Tour tour(graph);
            tour.setPath(test_path);
            
            std::cout << "順序訪問路徑長度: " << tour.getLength() << std::endl;
            std::cout << "已知最優解: 426" << std::endl;
            std::cout << "與最優解差異: " << (tour.getLength() - 426.0) << std::endl;
            std::cout << "相對差異: " << ((tour.getLength() - 426.0) / 426.0 * 100.0) << "%" << std::endl;
        }
        
        // 顯示圖的基本信息
        std::cout << "\n圖的基本信息:" << std::endl;
        std::cout << "城市數量: " << graph->size() << std::endl;
        
        // 計算平均距離
        double total_distance = 0.0;
        int count = 0;
        for (int i = 0; i < graph->size(); ++i) {
            for (int j = i + 1; j < graph->size(); ++j) {
                total_distance += graph->getDistance(i, j);
                count++;
            }
        }
        double avg_distance = total_distance / count;
        std::cout << "平均城市間距離: " << avg_distance << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "錯誤: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
