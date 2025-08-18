#include "aco/Graph.hpp"
#include "aco/Tour.hpp"
#include <iostream>
#include <memory>

int main() {
    std::cout << "=== 驗證路徑長度修正 ===" << std::endl;
    
    // 測試小圖的完整循環計算
    auto graph = std::make_shared<Graph>(4);
    
    // 設定已知距離
    graph->setDistance(0, 1, 10.0);
    graph->setDistance(0, 2, 15.0);
    graph->setDistance(0, 3, 20.0);
    graph->setDistance(1, 2, 35.0);
    graph->setDistance(1, 3, 25.0);
    graph->setDistance(2, 3, 30.0);
    
    // 測試路徑：0 -> 1 -> 2 -> 3 (應該自動加上 3 -> 0 = 20)
    std::vector<int> path = {0, 1, 2, 3};
    Tour tour(graph);
    tour.setPath(path);
    
    std::cout << "測試路徑: 0 -> 1 -> 2 -> 3 -> 0 (自動封閉)" << std::endl;
    std::cout << "邊長度: 0->1=" << graph->getDistance(0,1) 
              << ", 1->2=" << graph->getDistance(1,2) 
              << ", 2->3=" << graph->getDistance(2,3) 
              << ", 3->0=" << graph->getDistance(3,0) << std::endl;
    
    double expected = 10.0 + 35.0 + 30.0 + 20.0; // 95
    std::cout << "預期總長度: " << expected << std::endl;
    std::cout << "計算總長度: " << tour.getLength() << std::endl;
    std::cout << "修正結果: " << (tour.getLength() == expected ? "✓ 正確" : "✗ 錯誤") << std::endl;
    
    return 0;
}
