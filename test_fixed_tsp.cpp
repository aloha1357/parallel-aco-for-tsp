#include "aco/Graph.hpp"
#include "aco/AcoEngine.hpp"
#include "aco/TSPLibReader.hpp"
#include <iostream>
#include <memory>

int main() {
    std::cout << "=== 修正後的 TSP 路徑長度測試 ===" << std::endl;
    
    try {
        // 測試 berlin52
        std::cout << "\n=== 載入 berlin52.tsp ===" << std::endl;
        auto graph_berlin = Graph::fromTSPFile("data/berlin52.tsp");
        std::cout << "DIMENSION: " << graph_berlin->size() << std::endl;
        
        // 設置 ACO 參數
        aco::AcoParameters params;
        params.num_ants = 20;
        params.max_iterations = 50;
        params.alpha = 1.0;
        params.beta = 2.0;
        params.evaporation_rate = 0.5;
        params.random_seed = 42;
        
        // 運行 ACO
        std::cout << "\n運行 ACO 算法..." << std::endl;
        AcoEngine engine(graph_berlin, params);
        auto results = engine.optimize();
        
        std::cout << "\n=== berlin52 結果 ===" << std::endl;
        std::cout << "最佳路徑長度: " << results.best_length << std::endl;
        std::cout << "收斂迭代: " << results.convergence_iteration << std::endl;
        std::cout << "總執行時間: " << results.total_time_ms << " ms" << std::endl;
        
        // 顯示最後幾次迭代的統計
        if (!results.iteration_best_lengths.empty() && !results.iteration_avg_lengths.empty()) {
            int last_idx = results.iteration_best_lengths.size() - 1;
            std::cout << "\n最後迭代統計:" << std::endl;
            std::cout << "  iter_min: " << results.iteration_best_lengths[last_idx] << std::endl;
            std::cout << "  iter_avg: " << results.iteration_avg_lengths[last_idx] << std::endl;
            std::cout << "  global_best: " << results.best_length << std::endl;
            
            // 驗證 iter_avg >= iter_min
            bool avg_ge_min = results.iteration_avg_lengths[last_idx] >= results.iteration_best_lengths[last_idx];
            std::cout << "  數據一致性檢查 (avg >= min): " << (avg_ge_min ? "✓" : "✗") << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "錯誤: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
