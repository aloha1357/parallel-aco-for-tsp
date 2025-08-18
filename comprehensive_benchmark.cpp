#include "aco/BenchmarkAnalyzer.hpp"
#include "aco/TSPLibReader.hpp"
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
    
    // 獲取 TSPLIB 標準測試集
    auto tsplib_benchmarks = TSPLibReader::getStandardBenchmarks();
    
    // 選擇用於可擴展性測試的問題（不同規模）
    std::vector<TSPBenchmarkInstance> scalability_tests = {
        {"eil51", "data/eil51.tsp", 426},      // 51 城市
        {"st70", "data/st70.tsp", 675},       // 70 城市  
        {"kroA100", "data/kroA100.tsp", 21282} // 100 城市
    };
    
    // 1. 可擴展性測試
    std::cout << "=== 階段 1: 可擴展性分析 (使用 TSPLIB 標準測試集) ===" << std::endl;
    
    std::vector<ScalabilityResult> all_scalability_results;
    
    for (const auto& benchmark : scalability_tests) {
        std::cout << "\n測試 " << benchmark.name << " (" << benchmark.filename << ")..." << std::endl;
        
        // 載入 TSPLIB 檔案
        auto graph = TSPLibReader::loadGraphFromTSPLib(benchmark.filename);
        if (!graph) {
            std::cerr << "無法載入 " << benchmark.filename << "，跳過此測試" << std::endl;
            continue;
        }
        
        std::vector<int> thread_counts = {1, 2, 4, 8};
        auto results = analyzer.analyzeScalability(*graph, thread_counts);
        
        // 設置問題資訊
        for (auto& result : results) {
            result.best_length = benchmark.optimal_solution;  // 使用已知最優解
        }
        
        all_scalability_results.insert(all_scalability_results.end(), 
                                      results.begin(), results.end());
    }
    
    // 2. 策略基準測試
    std::cout << "\n=== 階段 2: 策略效能基準測試 (使用 TSPLIB 標準測試集) ===" << std::endl;
    
    // 選擇用於策略比較的問題
    std::vector<TSPBenchmark> strategy_test_benchmarks = {
        {"eil51", "data/eil51.tsp", 426, 51},
        {"berlin52", "data/berlin52.tsp", 7542, 52},
        {"kroA100", "data/kroA100.tsp", 21282, 100}
    };
    
    auto strategy_results = analyzer.benchmarkStrategies(strategy_test_benchmarks, 3);
    
    // 3. 導出結果
    std::cout << "\n=== 階段 3: 結果導出與分析 ===" << std::endl;
    
    analyzer.exportScalabilityResultsCSV(all_scalability_results, "scalability_results.csv");
    analyzer.exportStrategyBenchmarkCSV(strategy_results, "strategy_benchmark_results.csv");
    
    // 4. 生成圖表腳本
    analyzer.generatePlotScript("scalability_results.csv", "strategy_benchmark_results.csv");
    
    // 5. 生成詳細報告
    analyzer.generateBenchmarkReport(all_scalability_results, strategy_results);
    
    std::cout << "\n=== 基準測試完成 (使用 TSPLIB 標準測試集) ===" << std::endl;
    std::cout << "測試的 TSPLIB 問題：" << std::endl;
    std::cout << "- 可擴展性測試: eil51 (51城市), st70 (70城市), kroA100 (100城市)" << std::endl;
    std::cout << "- 策略比較測試: eil51, berlin52, kroA100" << std::endl;
    std::cout << std::endl;
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
