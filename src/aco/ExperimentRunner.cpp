#include "aco/ExperimentRunner.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace aco {

ExperimentRunner::ExperimentRunner(const ExperimentConfig& config) 
    : config_(config) {
    createOutputDirectory();
}

std::vector<ExperimentResult> ExperimentRunner::runCompleteExperiment(
    const std::vector<TSPBenchmarkInstance>& instances) {
    
    std::vector<ExperimentResult> all_results;
    
    std::cout << "=== Starting Complete Experiment ===" << std::endl;
    
    for (size_t i = 0; i < instances.size(); ++i) {
        const auto& instance = instances[i];
        
        std::cout << "\nProgress: [" << (i + 1) << "/" << instances.size() << "] "
                  << "Testing problem: " << instance.name << " (" << instance.optimal_solution << ")" << std::endl;
        
        // Establish baseline (single thread)
        establishBaseline(instance);
        
        // Run all configurations for this problem
        auto problem_results = runProblemExperiment(instance);
        
        // Add to total results
        all_results.insert(all_results.end(), problem_results.begin(), problem_results.end());
        
        // Save intermediate results in real time
        std::string partial_filename = config_.output_directory + "/partial_results_" + instance.name + ".csv";
        exportResults(problem_results, partial_filename);
    }
    
    std::cout << "\n=== Experiment Complete ===" << std::endl;
    std::cout << "Total experiment runs: " << all_results.size() << std::endl;
    
    return all_results;
}

std::vector<ExperimentResult> ExperimentRunner::runProblemExperiment(
    const TSPBenchmarkInstance& instance) {
    
    std::vector<ExperimentResult> results;
    
    // Load graph
    auto graph = TSPLibReader::loadGraphFromTSPLib(instance.filename);
    if (!graph) {
        std::cerr << "Unable to load graph file: " << instance.filename << std::endl;
        return results;
    }
    
    std::cout << "  Problem size: " << graph->size() << " cities" << std::endl;
    
    for (int thread_count : config_.thread_counts) {
        std::cout << "    Testing " << thread_count << " threads..." << std::flush;
        
        for (int run = 1; run <= config_.runs_per_configuration; ++run) {
            auto result = runSingleConfiguration(instance, thread_count, run);
            results.push_back(result);
            
            std::cout << " [" << run << "]" << std::flush;
        }
        
        // Calculate average results for this thread configuration
        auto thread_results = std::vector<ExperimentResult>(
            results.end() - config_.runs_per_configuration, results.end());
        
        double avg_time = calculateMean(extractValues(thread_results, 
            [](const ExperimentResult& r) { return r.execution_time_ms; }));
        double avg_length = calculateMean(extractValues(thread_results,
            [](const ExperimentResult& r) { return r.best_tour_length; }));
        
        std::cout << " Completed (avg: " << std::fixed << std::setprecision(1) 
                  << avg_time << "ms, " << avg_length << ")" << std::endl;
    }
    
    return results;
}

ExperimentResult ExperimentRunner::runSingleConfiguration(
    const TSPBenchmarkInstance& instance,
    int thread_count,
    int run_number) {
    
    // Load graph
    auto graph = TSPLibReader::loadGraphFromTSPLib(instance.filename);
    
    // Set ACO parameters
    AcoParameters params;
    params.num_ants = graph->size();  // Number of ants = number of cities
    params.max_iterations = config_.max_iterations;
    params.alpha = config_.alpha;
    params.beta = config_.beta;
    params.rho = config_.rho;
    params.num_threads = thread_count;
    
    // Create ACO engine
    AcoEngine engine(graph, params);
    
    // Start performance monitoring
    monitor_.startMonitoring();
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Run algorithm
    auto aco_result = engine.run();
    
    // Stop monitoring
    auto end_time = std::chrono::high_resolution_clock::now();
    monitor_.stopMonitoring();
    
    // Calculate execution time
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    double execution_time_ms = duration.count() / 1000.0;
    
    // Get performance metrics
    auto metrics = monitor_.getMetrics();
    
    // Calculate speedup and efficiency
    double baseline_time = baseline_times_[instance.name];
    double speedup = baseline_time / execution_time_ms;
    double efficiency = (speedup / thread_count) * 100.0;
    
    // Calculate approximation ratio
    double approximation_ratio = aco_result.best_tour_length / instance.optimal_solution;
    
    // Construct result
    ExperimentResult result;
    result.problem_name = instance.name;
    result.problem_size = graph->size();
    result.optimal_solution = instance.optimal_solution;
    result.thread_count = thread_count;
    result.run_number = run_number;
    
    result.execution_time_ms = execution_time_ms;
    result.best_tour_length = aco_result.best_tour_length;
    result.speedup = speedup;
    result.efficiency = efficiency;
    result.approximation_ratio = approximation_ratio;
    
    result.convergence_iteration = aco_result.convergence_iteration;
    result.memory_usage_mb = metrics.peak_memory_usage_mb;
    result.cpu_utilization = metrics.cpu_utilization;
    result.timestamp = std::chrono::system_clock::now();
    
    return result;
}

void ExperimentRunner::establishBaseline(const TSPBenchmarkInstance& instance) {
    if (baseline_times_.find(instance.name) != baseline_times_.end()) {
        return;  // Already established baseline
    }
    
    std::cout << "  Establishing baseline (single-threaded)..." << std::flush;
    
    // Run 3 single-threaded tests, take average
    std::vector<double> times;
    for (int i = 0; i < 3; ++i) {
        auto result = runSingleConfiguration(instance, 1, i + 1);
        times.push_back(result.execution_time_ms);
    }
    
    baseline_times_[instance.name] = calculateMean(times);
    
    std::cout << " Completed (" << std::fixed << std::setprecision(1) 
              << baseline_times_[instance.name] << "ms)" << std::endl;
}

std::vector<AggregatedResult> ExperimentRunner::aggregateResults(
    const std::vector<ExperimentResult>& results) {
    
    std::vector<AggregatedResult> aggregated;
    
    // Group by problem name and thread count
    std::map<std::pair<std::string, int>, std::vector<ExperimentResult>> groups;
    
    for (const auto& result : results) {
        auto key = std::make_pair(result.problem_name, result.thread_count);
        groups[key].push_back(result);
    }
    
    // Calculate statistics for each group
    for (const auto& [key, group_results] : groups) {
        AggregatedResult agg;
        agg.problem_name = key.first;
        agg.thread_count = key.second;
        
        // Extract numerical values
        auto extract_times = [](const std::vector<ExperimentResult>& results) {
            std::vector<double> values;
            for (const auto& r : results) values.push_back(r.execution_time_ms);
            return values;
        };
        
        auto extract_lengths = [](const std::vector<ExperimentResult>& results) {
            std::vector<double> values;
            for (const auto& r : results) values.push_back(r.best_tour_length);
            return values;
        };
        
        auto extract_speedups = [](const std::vector<ExperimentResult>& results) {
            std::vector<double> values;
            for (const auto& r : results) values.push_back(r.speedup);
            return values;
        };
        
        auto extract_efficiencies = [](const std::vector<ExperimentResult>& results) {
            std::vector<double> values;
            for (const auto& r : results) values.push_back(r.efficiency);
            return values;
        };
        
        auto extract_ratios = [](const std::vector<ExperimentResult>& results) {
            std::vector<double> values;
            for (const auto& r : results) values.push_back(r.approximation_ratio);
            return values;
        };
        
        std::vector<double> times = extract_times(group_results);
        std::vector<double> lengths = extract_lengths(group_results);
        std::vector<double> speedups = extract_speedups(group_results);
        std::vector<double> efficiencies = extract_efficiencies(group_results);
        std::vector<double> ratios = extract_ratios(group_results);
        
        // Calculate statistics
        agg.mean_execution_time = calculateMean(times);
        agg.std_execution_time = calculateStandardDeviation(times);
        agg.mean_tour_length = calculateMean(lengths);
        agg.std_tour_length = calculateStandardDeviation(lengths);
        agg.mean_speedup = calculateMean(speedups);
        agg.mean_efficiency = calculateMean(efficiencies);
        agg.mean_approximation_ratio = calculateMean(ratios);
        
        agg.best_tour_length = *std::min_element(lengths.begin(), lengths.end());
        agg.worst_tour_length = *std::max_element(lengths.begin(), lengths.end());
        
        // Calculate success rate (proportion with approximation ratio <= 1.2)
        int success_count = 0;
        for (double ratio : ratios) {
            if (ratio <= 1.2) success_count++;
        }
        agg.success_rate = (double)success_count / ratios.size() * 100.0;
        
        aggregated.push_back(agg);
    }
    
    return aggregated;
}

void ExperimentRunner::exportResults(const std::vector<ExperimentResult>& results) {
    std::string filename = config_.output_directory + "/detailed_results.csv";
    exportResults(results, filename);
}

void ExperimentRunner::exportResults(const std::vector<ExperimentResult>& results, 
                                    const std::string& filename) {
    
    std::ofstream file(filename);
    
    // CSV標頭
    file << "Problem_Name,Problem_Size,Optimal_Solution,Thread_Count,Run_Number,"
         << "Execution_Time_ms,Best_Tour_Length,Speedup,Efficiency,Approximation_Ratio,"
         << "Convergence_Iteration,Memory_MB,CPU_Utilization\n";
    
    for (const auto& result : results) {
        file << result.problem_name << ","
             << result.problem_size << ","
             << result.optimal_solution << ","
             << result.thread_count << ","
             << result.run_number << ","
             << std::fixed << std::setprecision(3) << result.execution_time_ms << ","
             << std::setprecision(1) << result.best_tour_length << ","
             << std::setprecision(3) << result.speedup << ","
             << std::setprecision(2) << result.efficiency << ","
             << std::setprecision(4) << result.approximation_ratio << ","
             << result.convergence_iteration << ","
             << result.memory_usage_mb << ","
             << std::setprecision(2) << result.cpu_utilization << "\n";
    }
    
    file.close();
    std::cout << "詳細結果已導出: " << filename << std::endl;
}

void ExperimentRunner::exportAggregatedResults(const std::vector<AggregatedResult>& results) {
    std::string filename = config_.output_directory + "/aggregated_results.csv";
    std::ofstream file(filename);
    
    file << "Problem_Name,Thread_Count,"
         << "Mean_Time_ms,Std_Time_ms,Mean_Length,Std_Length,"
         << "Mean_Speedup,Mean_Efficiency,Mean_Approximation_Ratio,"
         << "Best_Length,Worst_Length,Success_Rate\n";
    
    for (const auto& result : results) {
        file << result.problem_name << ","
             << result.thread_count << ","
             << std::fixed << std::setprecision(2) << result.mean_execution_time << ","
             << result.std_execution_time << ","
             << std::setprecision(1) << result.mean_tour_length << ","
             << result.std_tour_length << ","
             << std::setprecision(3) << result.mean_speedup << ","
             << std::setprecision(2) << result.mean_efficiency << ","
             << std::setprecision(4) << result.mean_approximation_ratio << ","
             << std::setprecision(1) << result.best_tour_length << ","
             << result.worst_tour_length << ","
             << std::setprecision(1) << result.success_rate << "\n";
    }
    
    file.close();
    std::cout << "聚合結果已導出: " << filename << std::endl;
}

void ExperimentRunner::generateAnalysisReport(const std::vector<ExperimentResult>& results) {
    auto aggregated = aggregateResults(results);
    exportAggregatedResults(aggregated);
    
    std::string filename = config_.output_directory + "/EXPERIMENT_REPORT.md";
    std::ofstream report(filename);
    
    report << "# 平行化 ACO TSP 標準實驗報告\n\n";
    report << "## 實驗配置\n\n";
    report << "- **執行緒配置**: ";
    for (size_t i = 0; i < config_.thread_counts.size(); ++i) {
        if (i > 0) report << ", ";
        report << config_.thread_counts[i];
    }
    report << "\n";
    report << "- **每配置運行次數**: " << config_.runs_per_configuration << "\n";
    report << "- **總實驗次數**: " << results.size() << "\n\n";
    
    // 按問題分組分析
    std::map<std::string, std::vector<ExperimentResult>> problem_groups;
    for (const auto& result : results) {
        problem_groups[result.problem_name].push_back(result);
    }
    
    report << "## 實驗結果摘要\n\n";
    
    for (const auto& [problem_name, problem_results] : problem_groups) {
        report << "### " << problem_name << "\n\n";
        
        // 找到該問題的聚合結果
        std::vector<AggregatedResult> problem_agg;
        for (const auto& agg : aggregated) {
            if (agg.problem_name == problem_name) {
                problem_agg.push_back(agg);
            }
        }
        
        report << "| 執行緒 | 平均時間(ms) | 加速比 | 效率(%) | 近似比 | 成功率(%) |\n";
        report << "|--------|-------------|--------|---------|--------|-----------|\n";
        
        for (const auto& agg : problem_agg) {
            report << "| " << agg.thread_count
                   << " | " << std::fixed << std::setprecision(1) << agg.mean_execution_time
                   << " | " << std::setprecision(2) << agg.mean_speedup << "x"
                   << " | " << std::setprecision(1) << agg.mean_efficiency << "%"
                   << " | " << std::setprecision(3) << agg.mean_approximation_ratio
                   << " | " << std::setprecision(1) << agg.success_rate << "% |\n";
        }
        report << "\n";
    }
    
    report.close();
    std::cout << "實驗報告已生成: " << filename << std::endl;
}

void ExperimentRunner::generateVisualization(const std::vector<ExperimentResult>& results) {
    // 生成Python繪圖腳本
    std::string script_filename = config_.output_directory + "/generate_experiment_plots.py";
    std::ofstream script(script_filename);
    
    script << R"(#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

# 讀取數據
df = pd.read_csv('detailed_results.csv')
agg_df = pd.read_csv('aggregated_results.csv')

# 設置風格
plt.style.use('seaborn-v0_8')
sns.set_palette("husl")

def plot_speedup_analysis():
    """繪製加速比分析"""
    fig, axes = plt.subplots(2, 2, figsize=(15, 12))
    
    problems = agg_df['Problem_Name'].unique()
    
    # 1. 加速比 vs 執行緒數
    ax1 = axes[0, 0]
    for problem in problems:
        data = agg_df[agg_df['Problem_Name'] == problem]
        ax1.plot(data['Thread_Count'], data['Mean_Speedup'], 
                'o-', label=problem, linewidth=2, markersize=8)
    
    # 理想線性加速比
    thread_counts = agg_df['Thread_Count'].unique()
    ax1.plot(thread_counts, thread_counts, 'k--', alpha=0.5, label='理想線性')
    
    ax1.set_xlabel('執行緒數')
    ax1.set_ylabel('加速比')
    ax1.set_title('加速比分析')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    
    # 2. 效率 vs 執行緒數
    ax2 = axes[0, 1]
    for problem in problems:
        data = agg_df[agg_df['Problem_Name'] == problem]
        ax2.plot(data['Thread_Count'], data['Mean_Efficiency'], 
                'o-', label=problem, linewidth=2, markersize=8)
    
    ax2.axhline(y=80, color='r', linestyle='--', alpha=0.7, label='80%效率線')
    ax2.set_xlabel('執行緒數')
    ax2.set_ylabel('平行效率 (%)')
    ax2.set_title('平行效率分析')
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    
    # 3. 近似比 vs 執行緒數
    ax3 = axes[1, 0]
    for problem in problems:
        data = agg_df[agg_df['Problem_Name'] == problem]
        ax3.plot(data['Thread_Count'], data['Mean_Approximation_Ratio'], 
                'o-', label=problem, linewidth=2, markersize=8)
    
    ax3.axhline(y=1.0, color='g', linestyle='--', alpha=0.7, label='最優解')
    ax3.set_xlabel('執行緒數')
    ax3.set_ylabel('近似比')
    ax3.set_title('解品質分析')
    ax3.legend()
    ax3.grid(True, alpha=0.3)
    
    # 4. 成功率 vs 執行緒數
    ax4 = axes[1, 1]
    for problem in problems:
        data = agg_df[agg_df['Problem_Name'] == problem]
        ax4.plot(data['Thread_Count'], data['Success_Rate'], 
                'o-', label=problem, linewidth=2, markersize=8)
    
    ax4.set_xlabel('執行緒數')
    ax4.set_ylabel('成功率 (%)')
    ax4.set_title('成功率分析')
    ax4.legend()
    ax4.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('experiment_analysis.png', dpi=300, bbox_inches='tight')
    plt.show()

if __name__ == "__main__":
    plot_speedup_analysis()
    print("實驗分析圖表已生成！")
)";
    
    script.close();
    std::cout << "可視化腳本已生成: " << script_filename << std::endl;
}

double ExperimentRunner::calculateMean(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
}

double ExperimentRunner::calculateStandardDeviation(const std::vector<double>& values) {
    if (values.size() <= 1) return 0.0;
    
    double mean = calculateMean(values);
    double sq_sum = 0.0;
    for (double value : values) {
        sq_sum += (value - mean) * (value - mean);
    }
    
    return std::sqrt(sq_sum / (values.size() - 1));
}

void ExperimentRunner::createOutputDirectory() {
    std::filesystem::create_directories(config_.output_directory);
    std::cout << "輸出目錄: " << config_.output_directory << std::endl;
}

bool ExperimentRunner::validateResults(const std::vector<ExperimentResult>& results) {
    // 基本驗證：檢查結果數量
    size_t expected_count = 0;
    for (const auto& instance : TSPLibReader::getStandardBenchmarks()) {
        if (std::filesystem::exists(instance.filename)) {
            expected_count += config_.thread_counts.size() * config_.runs_per_configuration;
        }
    }
    
    return results.size() >= expected_count * 0.8;  // 允許20%的誤差
}

} // namespace aco
