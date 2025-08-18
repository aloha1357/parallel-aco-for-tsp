#include "aco/Graph.hpp"
#include "aco/Tour.hpp"
#include "aco/AcoEngine.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <fstream>
#include <iomanip>
#include <sstream>

struct TestInstance {
    std::string name;
    std::string tsp_file;
    std::string opt_file;
    int known_optimal;
};

// 四個重點測試實例
std::vector<TestInstance> test_instances = {
    {"eil51", "data/eil51.tsp", "data/eil51.opt.tour", 426},
    {"kroA100", "data/kroA100.tsp", "data/kroA100.opt.tour", 21282},
    {"kroA150", "data/kroA150.tsp", "data/kroA150.opt.tour", 26524},
    {"kroA200", "data/kroA200.tsp", "data/kroA200.opt.tour", 29368}
};

std::vector<int> readOptimalTour(const std::string& filename) {
    // 讀取 .opt.tour 文件並返回路徑
    std::vector<int> tour;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cout << "⚠️  無法打開最優解文件: " << filename << std::endl;
        return tour;
    }
    
    std::string line;
    bool in_tour_section = false;
    
    while (std::getline(file, line)) {
        // 跳過空行和註釋
        if (line.empty() || line[0] == '#') continue;
        
        if (line.find("TOUR_SECTION") != std::string::npos) {
            in_tour_section = true;
            continue;
        }
        
        if (line == "EOF" || line == "-1") {
            break;
        }
        
        if (in_tour_section) {
            try {
                // 處理可能的空格或製表符
                std::istringstream iss(line);
                int city;
                if (iss >> city && city > 0) {
                    tour.push_back(city - 1);  // 轉換為 0-based 索引
                }
            } catch (const std::exception& e) {
                // 忽略解析錯誤
            }
        }
    }
    
    return tour;
}

double calculateOptimalLength(std::shared_ptr<Graph> graph, const std::vector<int>& optimal_tour) {
    // 計算最優路徑的實際長度
    if (optimal_tour.empty()) {
        return -1.0;
    }
    
    Tour tour(graph);
    tour.setPath(optimal_tour);
    return tour.getLength();
}

void testSingleInstance(const TestInstance& instance) {
    // 測試單個實例
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "測試實例: " << instance.name << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    try {
        // 載入圖
        std::cout << "載入 " << instance.tsp_file << "..." << std::endl;
        auto graph = Graph::fromTSPFile(instance.tsp_file);
        std::cout << "✓ 圖載入成功，維度: " << graph->size() << std::endl;
        
        // 讀取最優解
        std::cout << "讀取最優解 " << instance.opt_file << "..." << std::endl;
        std::vector<int> optimal_tour = readOptimalTour(instance.opt_file);
        
        double calculated_optimal_length = -1.0;
        if (!optimal_tour.empty()) {
            calculated_optimal_length = calculateOptimalLength(graph, optimal_tour);
            std::cout << "✓ 最優解讀取成功，路徑長度: " << optimal_tour.size() << " 城市" << std::endl;
            std::cout << "✓ 計算的最優解長度: " << std::fixed << std::setprecision(1) 
                      << calculated_optimal_length << std::endl;
            std::cout << "✓ 已知最優解長度: " << instance.known_optimal << std::endl;
            
            // 驗證我們的距離計算是否正確
            double difference = calculated_optimal_length - instance.known_optimal;
            if (std::abs(difference) < 1.0) {
                std::cout << "✓ 距離計算驗證: 正確 (差異: " << difference << ")" << std::endl;
            } else {
                std::cout << "⚠️  距離計算驗證: 可能有問題 (差異: " << difference << ")" << std::endl;
            }
        } else {
            std::cout << "⚠️  無法讀取最優解文件" << std::endl;
        }
        
        // 設置 ACO 參數
        aco::AcoParameters params;
        params.num_ants = std::min(50, graph->size());  // 調整螞蟻數量
        params.max_iterations = 100;
        params.alpha = 1.0;
        params.beta = 2.0;
        params.evaporation_rate = 0.5;
        params.random_seed = 42;
        
        std::cout << "\n運行 ACO 算法..." << std::endl;
        std::cout << "參數: 螞蟻=" << params.num_ants 
                  << ", 迭代=" << params.max_iterations 
                  << ", α=" << params.alpha 
                  << ", β=" << params.beta << std::endl;
        
        // 運行 ACO
        AcoEngine engine(graph, params);
        auto results = engine.optimize();
        
        // 分析結果
        std::cout << "\n" << std::string(40, '-') << std::endl;
        std::cout << "結果分析:" << std::endl;
        std::cout << std::string(40, '-') << std::endl;
        
        std::cout << "我們的最佳結果: " << std::fixed << std::setprecision(1) << results.best_length << std::endl;
        std::cout << "已知最優解: " << instance.known_optimal << std::endl;
        
        double our_difference = results.best_length - instance.known_optimal;
        double relative_error = (our_difference / instance.known_optimal) * 100.0;
        
        std::cout << "絕對差異: " << our_difference << std::endl;
        std::cout << "相對誤差: " << relative_error << "%" << std::endl;
        
        // 評估結果
        if (relative_error < 0) {
            std::cout << "🚨 異常：我們的結果比已知最優解還好！可能有計算錯誤。" << std::endl;
        } else if (relative_error < 5) {
            std::cout << "🎉 優秀：相對誤差 < 5%" << std::endl;
        } else if (relative_error < 15) {
            std::cout << "✅ 良好：相對誤差 < 15%" << std::endl;
        } else if (relative_error < 30) {
            std::cout << "⚠️  可接受：相對誤差 < 30%" << std::endl;
        } else {
            std::cout << "❌ 需要改進：相對誤差 > 30%" << std::endl;
        }
        
        std::cout << "收斂迭代: " << results.convergence_iteration << std::endl;
        std::cout << "執行時間: " << results.total_time_ms << " ms" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 測試失敗: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "🧪 修正後的 ACO 算法全面測試" << std::endl;
    std::cout << "測試四個關鍵實例：eil51, kroA100, kroA150, kroA200" << std::endl;
    std::cout << "驗證：1) 距離計算修正 2) 路徑長度計算修正 3) 算法性能" << std::endl;
    
    for (const auto& instance : test_instances) {
        testSingleInstance(instance);
    }
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "🏁 全部測試完成！" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    return 0;
}
