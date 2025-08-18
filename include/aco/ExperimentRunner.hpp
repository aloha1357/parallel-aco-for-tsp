#pragma once

#include "TSPLibReader.hpp"
#include "BenchmarkAnalyzer.hpp"
#include "AcoEngine.hpp"
#include "PerformanceMonitor.hpp"
#include <vector>
#include <string>
#include <map>
#include <chrono>

namespace aco {

struct ExperimentConfig {
    std::vector<int> thread_counts = {1, 2, 4, 8};
    int runs_per_configuration = 5;
    std::string output_directory = "experiment_results";
    
    // ACO參數 (保持一致)
    int max_iterations = 100;
    double alpha = 1.0;
    double beta = 2.0;
    double rho = 0.5;
    double q0 = 0.9;  // 對於ACS策略
};

struct ExperimentResult {
    // 基本資訊
    std::string problem_name;
    int problem_size;
    int optimal_solution;
    int thread_count;
    int run_number;
    
    // 性能指標
    double execution_time_ms;
    double best_tour_length;
    double speedup;  // 相對於單執行緒
    double efficiency;  // speedup / thread_count * 100%
    double approximation_ratio;  // best_tour_length / optimal_solution
    
    // 詳細統計
    int convergence_iteration;
    size_t memory_usage_mb;
    double cpu_utilization;
    
    // 時間戳
    std::chrono::system_clock::time_point timestamp;
};

struct AggregatedResult {
    std::string problem_name;
    int thread_count;
    
    // 統計數據 (5次運行的統計)
    double mean_execution_time;
    double std_execution_time;
    double mean_tour_length;
    double std_tour_length;
    double mean_speedup;
    double mean_efficiency;
    double mean_approximation_ratio;
    
    // 最佳結果
    double best_tour_length;
    double worst_tour_length;
    
    // 成功率 (在20%範圍內找到好解的比例)
    double success_rate;
};

class ExperimentRunner {
private:
    ExperimentConfig config_;
    PerformanceMonitor monitor_;
    std::map<std::string, double> baseline_times_;  // 單執行緒基準時間
    
public:
    explicit ExperimentRunner(const ExperimentConfig& config);
    
    // 運行完整實驗
    std::vector<ExperimentResult> runCompleteExperiment(
        const std::vector<TSPBenchmarkInstance>& instances);
    
    // 運行單一問題的實驗
    std::vector<ExperimentResult> runProblemExperiment(
        const TSPBenchmarkInstance& instance);
    
    // 運行單次配置
    ExperimentResult runSingleConfiguration(
        const TSPBenchmarkInstance& instance,
        int thread_count,
        int run_number);
    
    // 聚合結果統計
    std::vector<AggregatedResult> aggregateResults(
        const std::vector<ExperimentResult>& results);
    
    // 導出結果到CSV
    void exportResults(const std::vector<ExperimentResult>& results);
    void exportAggregatedResults(const std::vector<AggregatedResult>& results);
    
    // 生成分析報告
    void generateAnalysisReport(const std::vector<ExperimentResult>& results);
    
    // 生成可視化
    void generateVisualization(const std::vector<ExperimentResult>& results);
    
private:
    // 計算統計數據
    double calculateMean(const std::vector<double>& values);
    double calculateStandardDeviation(const std::vector<double>& values);
    
    // 創建輸出目錄
    void createOutputDirectory();
    
    // 載入或創建基準時間
    void establishBaseline(const TSPBenchmarkInstance& instance);
    
    // 驗證結果一致性
    bool validateResults(const std::vector<ExperimentResult>& results);
};
