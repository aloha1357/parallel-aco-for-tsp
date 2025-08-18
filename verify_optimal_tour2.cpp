#include <iostream>
#include <vector>
#include <memory>
#include "aco/Graph.hpp"
#include "aco/Tour.hpp"

int main() {
    try {
        // 載入kroA100問題
        auto graph = Graph::fromTSPFile("data/kroA100.tsp");
        
        std::cout << "Loaded kroA100 with " << graph->size() << " cities" << std::endl;
        
        // 最優路徑 (從.opt.tour文件，轉換為0-based索引)
        std::vector<int> optimal_tour = {
            0, 46, 92, 27, 66, 57, 60, 50, 86, 24, 80, 68, 63, 39, 53, 1, 43, 49, 72, 67, 84, 81, 94, 12, 75, 32, 36, 4, 51, 77, 95, 38, 29, 47, 99, 40, 70, 13, 2, 42, 45, 28, 33, 82, 54, 6, 8, 56, 19, 11, 26, 85, 34, 61, 59, 76, 22, 97, 90, 44, 31, 10, 14, 16, 58, 73, 20, 71, 9, 83, 35, 98, 37, 23, 17, 78, 52, 87, 15, 93, 21, 69, 65, 25, 64, 3, 96, 55, 79, 30, 88, 41, 7, 91, 74, 18, 89, 48, 5, 62
        };
        
        // 創建Tour對象並設置路徑
        Tour tour(graph);
        tour.setPath(optimal_tour);
        
        double tour_length = tour.getLength();
        
        std::cout << "Optimal tour length calculated by our algorithm: " << tour_length << std::endl;
        std::cout << "Expected optimal tour length: 21282" << std::endl;
        std::cout << "Difference: " << (tour_length - 21282) << std::endl;
        
        // 也手動檢查前幾段距離來檢查
        std::cout << "\n手動檢查前幾段距離:" << std::endl;
        for (int i = 0; i < 5; i++) {
            int from = optimal_tour[i];
            int to = optimal_tour[(i + 1) % optimal_tour.size()];
            double dist = graph->getDistance(from, to);
            std::cout << "City " << from << " -> City " << to << ": " << dist << std::endl;
        }
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
