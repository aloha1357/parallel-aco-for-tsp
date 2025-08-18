#include "aco/Graph.hpp"
#include "aco/Tour.hpp"
#include "aco/AcoEngine.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

struct TSPBenchmark {
    std::string name;
    std::string tsp_file;
    std::string opt_file;
    int expected_optimal;
    int dimension;
};

// 讀取 .opt.tour 文件
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
                std::istringstream iss(line);
                int city;
                if (iss >> city && city > 0) {  // TSPLIB 使用 1-based 索引
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
    std::cout << "=== 修正後 ACO 算法驗證測試 ===" << std::endl;
    std::cout << "測試規模：50、100、150、202 城市" << std::endl;
    std::cout << "驗證：1) 距離計算 2) 路徑長度計算 3) ACO 性能" << std::endl;
    std::cout << std::string(70, '=') << std::endl;
    
    // 定義測試實例
    std::vector<TSPBenchmark> benchmarks = {
        {"eil51", "data/eil51.tsp", "data/eil51.opt.tour", 426, 51},
        {"berlin52", "data/berlin52.tsp", "data/berlin52.opt.tour", 7542, 52},
        {"kroA100", "data/kroA100.tsp", "data/kroA100.opt.tour", 21282, 100},
        {"gr202", "data/gr202.tsp", "data/gr202.opt.tour", 40160, 202}
    };
    
    // ACO 參數設置（根據問題規模調整）
    AcoParameters base_params;
    base_params.alpha = 1.0;
    base_params.beta = 2.0;
    base_params.rho = 0.1;  // evaporation rate
    base_params.random_seed = 42;
    
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "實例\t維度\t預期最優\t實際最優\t差異\tACO結果\tACO差異\t相對差異(%)" << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    
    for (const auto& benchmark : benchmarks) {
        try {
            std::cout << "\n🔍 測試 " << benchmark.name << "..." << std::endl;
            
            // 1. 載入圖
            auto graph = Graph::fromTSPFile(benchmark.tsp_file);
            std::cout << "✓ 載入圖，實際維度: " << graph->size() << std::endl;
            
            // 2. 讀取並驗證最優解
            std::vector<int> optimal_tour = readOptimalTour(benchmark.opt_file);
            
            if (optimal_tour.empty()) {
                std::cout << "✗ 無法讀取最優解文件" << std::endl;
                continue;
            }
            
            std::cout << "✓ 讀取最優解路徑，長度: " << optimal_tour.size() << " 城市" << std::endl;
            
            // 3. 計算最優解的實際長度（驗證我們的距離計算）
            Tour optimal_tour_obj(graph);
            optimal_tour_obj.setPath(optimal_tour);
            double actual_optimal = optimal_tour_obj.getLength();
            
            std::cout << "✓ 計算最優解實際長度: " << actual_optimal << std::endl;
            
            // 檢查距離計算是否正確
            double optimal_diff = actual_optimal - benchmark.expected_optimal;
            bool distance_correct = std::abs(optimal_diff) < 1.0; // 允許小數舍入誤差
            
            if (!distance_correct) {
                std::cout << "⚠️  警告：最優解長度不匹配！預期 " << benchmark.expected_optimal 
                          << "，實際 " << actual_optimal << std::endl;
            }
            
            // 4. 設置 ACO 參數（根據問題規模調整）
            AcoParameters params = base_params;
            if (graph->size() <= 60) {
                params.num_ants = 30;
                params.max_iterations = 100;
            } else if (graph->size() <= 120) {
                params.num_ants = 50;
                params.max_iterations = 150;
            } else {
                params.num_ants = 100;
                params.max_iterations = 200;
            }
            
            std::cout << "🚀 運行 ACO (螞蟻:" << params.num_ants << ", 迭代:" << params.max_iterations << ")..." << std::endl;
            
            // 5. 運行 ACO 算法
            AcoEngine engine(graph, params);
            auto results = engine.run();
            
            // 6. 計算結果
            double aco_diff = results.best_tour_length - actual_optimal;
            double relative_diff = (aco_diff / actual_optimal) * 100.0;
            
            // 7. 輸出結果
            std::cout << benchmark.name << "\t" 
                      << graph->size() << "\t" 
                      << benchmark.expected_optimal << "\t" 
                      << actual_optimal << "\t" 
                      << optimal_diff << "\t"
                      << results.best_tour_length << "\t"
                      << aco_diff << "\t" 
                      << relative_diff << "%" << std::endl;
            
            // 8. 結果評估
            if (!distance_correct) {
                std::cout << "❌ 距離計算有誤" << std::endl;
            } else if (relative_diff < 0) {
                std::cout << "⚠️  ACO 結果比最優解還好，可能有問題" << std::endl;
            } else if (relative_diff < 5) {
                std::cout << "🎉 優秀：相對差異 < 5%" << std::endl;
            } else if (relative_diff < 15) {
                std::cout << "✅ 良好：相對差異 < 15%" << std::endl;
            } else if (relative_diff < 30) {
                std::cout << "⚖️  可接受：相對差異 < 30%" << std::endl;
            } else {
                std::cout << "❌ 需改進：相對差異 > 30%" << std::endl;
            }
            
            std::cout << "   執行時間: " << results.execution_time_ms << " ms" << std::endl;
            std::cout << "   收斂迭代: " << results.convergence_iteration << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "✗ " << benchmark.name << ": 測試失敗 - " << e.what() << std::endl;
        }
    }
    
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "驗證完成！" << std::endl;
    std::cout << "\n評分標準：" << std::endl;
    std::cout << "🎉 優秀：相對差異 < 5%" << std::endl;
    std::cout << "✅ 良好：相對差異 < 15%" << std::endl;
    std::cout << "⚖️  可接受：相對差異 < 30%" << std::endl;
    std::cout << "❌ 需改進：相對差異 > 30%" << std::endl;
    
    return 0;
}
