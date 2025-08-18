#include "aco/Graph.hpp"
#include "aco/Tour.hpp"
#include "aco/AcoEngine.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <iomanip>

struct TSPBenchmark {
    std::string name;
    std::string filename;
    int optimal_value;
};

std::vector<TSPBenchmark> benchmarks = {
    {"eil51", "data/eil51.tsp", 426},
    {"eil76", "data/eil76.tsp", 538},
    {"berlin52", "data/berlin52.tsp", 7542},
    {"kroA100", "data/kroA100.tsp", 21282},
    {"kroA150", "data/kroA150.tsp", 26524}
};

int main() {
    std::cout << "=== 修正後的 ACO 算法測試 ===" << std::endl;
    std::cout << "測試我們修正的路徑長度計算和 TSPLIB 距離計算" << std::endl;
    std::cout << "=" * 60 << std::endl;
    
    // ACO 參數設置
    aco::AcoParameters params;
    params.num_ants = 50;
    params.max_iterations = 100;
    params.alpha = 1.0;
    params.beta = 2.0;
    params.evaporation_rate = 0.5;
    params.random_seed = 42;
    
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "實例\t維度\t最優解\t我們的結果\t差異\t相對差異(%)" << std::endl;
    std::cout << "-" * 60 << std::endl;
    
    for (const auto& benchmark : benchmarks) {
        try {
            std::cout << "\n正在測試 " << benchmark.name << "..." << std::endl;
            
            // 載入圖
            auto graph = Graph::fromTSPFile(benchmark.filename);
            
            // 運行 ACO
            AcoEngine engine(graph, params);
            auto results = engine.optimize();
            
            // 計算差異
            double difference = results.best_length - benchmark.optimal_value;
            double relative_diff = (difference / benchmark.optimal_value) * 100.0;
            
            // 輸出結果
            std::cout << benchmark.name << "\t" 
                      << graph->size() << "\t" 
                      << benchmark.optimal_value << "\t" 
                      << results.best_length << "\t" 
                      << difference << "\t" 
                      << relative_diff << "%" << std::endl;
            
            // 檢查結果合理性
            if (relative_diff < 0) {
                std::cout << "⚠️  注意：我們的結果比已知最優解還好，可能有計算錯誤！" << std::endl;
            } else if (relative_diff < 20) {
                std::cout << "✓ 好結果：相對差異小於 20%" << std::endl;
            } else if (relative_diff < 50) {
                std::cout << "△ 可接受：相對差異在 20-50% 之間" << std::endl;
            } else {
                std::cout << "✗ 需要改進：相對差異超過 50%" << std::endl;
            }
            
        } catch (const std::exception& e) {
            std::cout << "✗ " << benchmark.name << ": 測試失敗 - " << e.what() << std::endl;
        }
    }
    
    std::cout << "\n" << "=" * 60 << std::endl;
    std::cout << "測試完成！" << std::endl;
    std::cout << "\n說明：" << std::endl;
    std::cout << "- 相對差異 < 20%：算法表現良好" << std::endl;
    std::cout << "- 相對差異 20-50%：可接受範圍" << std::endl;
    std::cout << "- 相對差異 > 50%：需要調整參數或算法" << std::endl;
    std::cout << "- 負差異：可能存在計算錯誤" << std::endl;
    
    return 0;
}
