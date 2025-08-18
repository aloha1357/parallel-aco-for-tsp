#pragma once

#include "AcoEngine.hpp"
#include "PerformanceMonitor.hpp"
#include "StrategyComparator.hpp"
#include <vector>
#include <string>
#include <map>
#include <fstream>

namespace aco {

struct TSPBenchmark {
    std::string name;
    std::string filename;
    int optimal_length;  // 已知最優解
    int city_count;
};

struct ScalabilityResult {
    int thread_count;
    double execution_time_ms;
    double best_length;
    double speedup;
    double efficiency;
    size_t memory_mb;
    
    // 新增統計字段
    double avg_length;          // 平均解長度
    double std_dev_time;        // 執行時間標準差
    double std_dev_length;      // 解長度標準差
    double min_time_ms;         // 最短執行時間
    double max_time_ms;         // 最長執行時間
    int total_runs;             // 總運行次數
};

struct StrategyBenchmarkResult {
    AcoStrategy strategy;
    std::string strategy_name;
    double best_length;
    double avg_length;
    double execution_time_ms;
    double gap_to_optimal;  // (solution - optimal) / optimal * 100%
    int convergence_iteration;
    double success_rate;  // 在多次運行中找到好解的比例
    
    // 新增統計字段
    double std_dev_length;      // 解長度標準差
    double std_dev_time;        // 執行時間標準差
    double worst_length;        // 最差解長度
    double min_time_ms;         // 最短執行時間
    double max_time_ms;         // 最長執行時間
    double confidence_interval; // 95%置信區間
};

class BenchmarkAnalyzer {
private:
    std::vector<TSPBenchmark> standard_benchmarks_;
    PerformanceMonitor performance_monitor_;
    StrategyComparator strategy_comparator_;
    
public:
    BenchmarkAnalyzer();
    
    // 添加標準TSP測試問題
    void addStandardBenchmarks();
    
    // 串行 vs 平行性能比較
    std::vector<ScalabilityResult> analyzeScalability(
        const Graph& graph, 
        const std::vector<int>& thread_counts = {1, 2, 4, 8, 16}
    );
    
    // 策略在標準問題集上的表現
    std::vector<StrategyBenchmarkResult> benchmarkStrategies(
        const std::vector<TSPBenchmark>& benchmarks,
        int runs_per_benchmark = 5
    );
    
    // 生成CSV格式的結果
    void exportScalabilityResultsCSV(
        const std::vector<ScalabilityResult>& results,
        const std::string& filename
    );
    
    void exportStrategyBenchmarkCSV(
        const std::vector<StrategyBenchmarkResult>& results,
        const std::string& filename
    );
    
    // 生成Python繪圖腳本
    void generatePlotScript(
        const std::string& scalability_csv,
        const std::string& benchmark_csv,
        const std::string& output_dir = "plots"
    );
    
    // 生成詳細報告
    void generateBenchmarkReport(
        const std::vector<ScalabilityResult>& scalability_results,
        const std::vector<StrategyBenchmarkResult>& strategy_results,
        const std::string& filename = "BENCHMARK_REPORT.md"
    );
    
    // 新增：進行詳細統計分析的平均測試
    void runDetailedAverageAnalysis(
        const std::vector<TSPBenchmark>& benchmarks,
        int runs_per_configuration = 10,
        const std::string& output_prefix = "detailed_analysis"
    );
    
    // 新增：統計顯著性測試
    void performStatisticalTests(
        const std::vector<ScalabilityResult>& results,
        const std::string& output_file = "statistical_analysis.md"
    );
    
private:
    // 運行單一配置
    ScalabilityResult runSingleScalabilityTest(const Graph& graph, int thread_count);
    
    // 運行策略測試
    StrategyBenchmarkResult runStrategyBenchmark(
        AcoStrategy strategy,
        const Graph& graph,
        int optimal_length,
        int runs
    );
    
    // 載入TSP檔案
    std::unique_ptr<Graph> loadTSPFile(const std::string& filename);
    
    // 新增：導出詳細統計結果
    void exportDetailedScalabilityResultsCSV(
        const std::vector<ScalabilityResult>& results,
        const std::string& filename
    );
};

} // namespace aco
