#include "aco/BenchmarkAnalyzer.hpp"
#include "aco/SyntheticTSPGenerator.hpp"
#include <iostream>
#include <vector>

using namespace aco;

int main() {
    std::cout << "=== ACO TSP 完整基準測試與分析 ===" << std::endl;
    std::cout << "此程式將進行以下測試：" << std::endl;
    std::cout << "1. 串行 vs 平行性能分析" << std::endl;
    std::cout << "2. 不同ACO策略效能比較" << std::endl;
    std::cout << "3. 生成詳細分析報告與圖表" << std::endl;
    std::cout << std::endl;

    BenchmarkAnalyzer analyzer;
    
    // 準備測試資料 - 使用不同規模的問題
    std::vector<int> test_sizes = {50, 75, 100};
    
    // 1. 可擴展性測試
    std::cout << "=== 階段 1: 可擴展性分析 ===" << std::endl;
    
    std::vector<ScalabilityResult> all_scalability_results;
    
    for (int size : test_sizes) {
        std::cout << "\n測試 " << size << " 城市問題..." << std::endl;
        
        auto graph = SyntheticTSPGenerator::generateRandomInstance(size, 1000.0, 12345);
        
        std::vector<int> thread_counts = {1, 2, 4, 8};
        auto results = analyzer.analyzeScalability(*graph, thread_counts);
        
        // 添加問題規模資訊
        for (auto& result : results) {
            result.best_length = size;  // 暫時用問題規模標記
        }
        
        all_scalability_results.insert(all_scalability_results.end(), 
                                      results.begin(), results.end());
    }
    
    // 2. 策略基準測試
    std::cout << "\n=== 階段 2: 策略效能基準測試 ===" << std::endl;
    
    // 創建標準測試問題集
    std::vector<TSPBenchmark> test_benchmarks = {
        {"small_random", "small_random.tsp", 2500, 50},
        {"medium_random", "medium_random.tsp", 4000, 75}, 
        {"large_random", "large_random.tsp", 6000, 100}
    };
    
    auto strategy_results = analyzer.benchmarkStrategies(test_benchmarks, 3);
    
    // 3. 導出結果
    std::cout << "\n=== 階段 3: 結果導出與分析 ===" << std::endl;
    
    analyzer.exportScalabilityResultsCSV(all_scalability_results, "scalability_results.csv");
    analyzer.exportStrategyBenchmarkCSV(strategy_results, "strategy_benchmark_results.csv");
    
    // 4. 生成圖表腳本
    analyzer.generatePlotScript("scalability_results.csv", "strategy_benchmark_results.csv");
    
    // 5. 生成詳細報告
    analyzer.generateBenchmarkReport(all_scalability_results, strategy_results);
    
    std::cout << "\n=== 基準測試完成 ===" << std::endl;
    std::cout << "生成的檔案：" << std::endl;
    std::cout << "- scalability_results.csv: 可擴展性測試資料" << std::endl;
    std::cout << "- strategy_benchmark_results.csv: 策略比較資料" << std::endl;
    std::cout << "- generate_plots.py: Python圖表生成腳本" << std::endl;
    std::cout << "- BENCHMARK_REPORT.md: 詳細分析報告" << std::endl;
    std::cout << std::endl;
    
    std::cout << "接下來的步驟：" << std::endl;
    std::cout << "1. 安裝Python依賴: pip install pandas matplotlib seaborn numpy" << std::endl;
    std::cout << "2. 運行繪圖腳本: python generate_plots.py" << std::endl;
    std::cout << "3. 查看生成的圖表檔案 (plots/目錄)" << std::endl;
    std::cout << "4. 閱讀詳細報告: BENCHMARK_REPORT.md" << std::endl;
    
    return 0;
}
