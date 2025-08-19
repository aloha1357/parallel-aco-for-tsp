#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <omp.h>
#include "../include/aco/AcoEngine.hpp"
#include "../include/aco/Graph.hpp"
#include "../include/aco/TSPLibReader.hpp"

struct ExperimentConfig {
    std::string instance_name;
    std::string file_path;
    int optimal_value;
    std::vector<int> ant_counts;
    std::vector<int> thread_counts;
    int iterations = 50;
    int runs_per_config = 5;
};

struct ExperimentResult {
    std::string instance_name;
    int num_ants;
    int num_threads;
    int run_id;
    double execution_time_ms;
    double best_cost;
    double avg_cost;
    double gap_percent;
    double speedup;
    double efficiency;
};

class ComprehensiveAntThreadExperiment {
private:
    std::vector<ExperimentResult> results;
    
    void runSingleExperiment(const ExperimentConfig& config) {
        std::cout << "\n=== 執行實驗: " << config.instance_name << " ===" << std::endl;
        
        // 載入TSP實例
        auto graph = aco::TSPLibReader::loadGraphFromTSPLib(config.file_path);
        if (!graph) {
            std::cerr << "無法載入TSP檔案: " << config.file_path << std::endl;
            return;
        }
        
        std::cout << "城市數量: " << graph->size() << std::endl;
        std::cout << "最優解: " << config.optimal_value << std::endl;
        
        // 獲取單線程基準時間
        double baseline_time = 0.0;
        
        for (int ants : config.ant_counts) {
            std::cout << "\n--- 螞蟻數量: " << ants << " ---" << std::endl;
            
            for (int threads : config.thread_counts) {
                std::cout << "線程數: " << threads << " ";
                
                double total_time = 0.0;
                double total_best_cost = 0.0;
                double total_avg_cost = 0.0;
                
                for (int run = 0; run < config.runs_per_config; run++) {
                    // 設置ACO參數
                    AcoParameters params;
                    params.num_ants = ants;
                    params.max_iterations = config.iterations;
                    params.num_threads = threads;
                    params.alpha = 1.0;
                    params.beta = 2.0;
                    params.rho = 0.1;
                    
                    AcoEngine engine(graph, params);
                    
                    auto start = std::chrono::high_resolution_clock::now();
                    auto results = engine.run();
                    auto end = std::chrono::high_resolution_clock::now();
                    
                    double time_ms = std::chrono::duration<double, std::milli>(end - start).count();
                    
                    total_time += time_ms;
                    total_best_cost += results.best_tour_length;
                    
                    // 計算平均值（使用最後迭代的平均值）
                    double avg_cost = results.iteration_avg_lengths.empty() ? 
                        results.best_tour_length : 
                        results.iteration_avg_lengths.back();
                    total_avg_cost += avg_cost;
                    
                    // 計算效能指標
                    double gap_percent = ((results.best_tour_length - config.optimal_value) / config.optimal_value) * 100.0;
                    
                    // 儲存單線程基準時間（第一個配置）
                    if (threads == config.thread_counts[0] && ants == config.ant_counts[0] && run == 0) {
                        baseline_time = time_ms;
                    }
                    
                    double speedup = (baseline_time > 0) ? baseline_time / time_ms : 1.0;
                    double efficiency = speedup / threads;
                    
                    ExperimentResult result = {
                        config.instance_name,
                        ants,
                        threads,
                        run + 1,
                        time_ms,
                        results.best_tour_length,
                        avg_cost,
                        gap_percent,
                        speedup,
                        efficiency
                    };
                    
                    results.push_back(result);
                }
                
                double avg_time = total_time / config.runs_per_config;
                double avg_best = total_best_cost / config.runs_per_config;
                double avg_gap = ((avg_best - config.optimal_value) / config.optimal_value) * 100.0;
                double speedup = (baseline_time > 0) ? baseline_time / avg_time : 1.0;
                
                std::cout << std::fixed << std::setprecision(1);
                std::cout << "時間: " << avg_time << "ms ";
                std::cout << "最佳解: " << (int)avg_best << " ";
                std::cout << "差距: " << avg_gap << "% ";
                std::cout << "加速比: " << speedup << "x" << std::endl;
            }
        }
    }
    
public:
    void runAllExperiments() {
        std::cout << "=== 全面螞蟻-線程實驗開始 ===" << std::endl;
        #ifdef _OPENMP
        std::cout << "OpenMP 版本: " << _OPENMP << std::endl;
        std::cout << "最大線程數: " << omp_get_max_threads() << std::endl;
        #else
        std::cout << "OpenMP 未啟用" << std::endl;
        #endif
        
        // 實驗配置
        std::vector<ExperimentConfig> configs = {
            {
                "eil51", 
                "../data/eil51.tsp", 
                426,
                {16, 32, 64, 128},  // 螞蟻數量：以線程數為倍數
                {1, 2, 4, 8, 16},   // 線程數量
                50, 
                3
            },
            {
                "kroA100", 
                "../data/kroA100.tsp", 
                21282,
                {16, 32, 64, 128},
                {1, 2, 4, 8, 16},
                50, 
                3
            },
            {
                "ch150", 
                "../data/ch150.tsp", 
                6528,
                {16, 32, 64, 128},
                {1, 2, 4, 8, 16},
                50, 
                3
            },
            {
                "gr202", 
                "../data/gr202.tsp", 
                40160,
                {16, 32, 64, 128},
                {1, 2, 4, 8, 16},
                50, 
                3
            }
        };
        
        for (const auto& config : configs) {
            runSingleExperiment(config);
        }
        
        saveResults();
        generateSummaryReport();
    }
    
    void saveResults() {
        std::ofstream csv_file("comprehensive_ant_thread_results.csv");
        csv_file << "instance,num_ants,num_threads,run_id,time_ms,best_cost,avg_cost,gap_percent,speedup,efficiency\n";
        
        for (const auto& result : results) {
            csv_file << result.instance_name << ","
                    << result.num_ants << ","
                    << result.num_threads << ","
                    << result.run_id << ","
                    << std::fixed << std::setprecision(3)
                    << result.execution_time_ms << ","
                    << result.best_cost << ","
                    << result.avg_cost << ","
                    << result.gap_percent << ","
                    << result.speedup << ","
                    << result.efficiency << "\n";
        }
        csv_file.close();
        std::cout << "\n結果已儲存至: comprehensive_ant_thread_results.csv" << std::endl;
    }
    
    void generateSummaryReport() {
        std::ofstream report("comprehensive_ant_thread_report.md");
        report << "# 螞蟻數量-線程數量綜合實驗報告\n\n";
        
        report << "## 實驗概述\n";
        report << "- **實驗目的**: 分析螞蟻數量和線程數量對平行ACO性能的影響\n";
        report << "- **測試實例**: eil51, kroA100, ch150, gr202\n";
        report << "- **螞蟻數量**: 16, 32, 64, 128\n";
        report << "- **線程數量**: 1, 2, 4, 8, 16\n";
        report << "- **迭代次數**: 50\n";
        report << "- **重複次數**: 3\n\n";
        
        // 按實例分組分析
        std::vector<std::string> instances = {"eil51", "kroA100", "ch150", "gr202"};
        std::vector<int> ant_counts = {16, 32, 64, 128};
        std::vector<int> thread_counts = {1, 2, 4, 8, 16};
        
        for (const auto& instance : instances) {
            report << "## " << instance << " 實例分析\n\n";
            
            // 創建表格標題
            report << "| 螞蟻數 | 線程數 | 平均時間(ms) | 最佳解 | 差距(%) | 加速比 | 效率 |\n";
            report << "|--------|--------|-------------|--------|---------|--------|------|\n";
            
            for (int ants : ant_counts) {
                for (int threads : thread_counts) {
                    // 計算該配置的平均值
                    double total_time = 0.0, total_best = 0.0, total_gap = 0.0;
                    double total_speedup = 0.0, total_efficiency = 0.0;
                    int count = 0;
                    
                for (const auto& result : results) {
                    if (result.instance_name == instance && 
                        result.num_ants == ants && 
                        result.num_threads == threads) {
                        total_time += result.execution_time_ms;
                        total_best += result.best_cost;
                        total_gap += result.gap_percent;
                        total_speedup += result.speedup;
                        total_efficiency += result.efficiency;
                        count++;
                    }
                }                    if (count > 0) {
                        report << "| " << ants << " | " << threads << " | "
                              << std::fixed << std::setprecision(1) << (total_time / count) << " | "
                              << std::fixed << std::setprecision(0) << (total_best / count) << " | "
                              << std::fixed << std::setprecision(2) << (total_gap / count) << " | "
                              << std::fixed << std::setprecision(2) << (total_speedup / count) << " | "
                              << std::fixed << std::setprecision(3) << (total_efficiency / count) << " |\n";
                    }
                }
                report << "\n";
            }
        }
        
        report << "\n## 關鍵發現\n";
        report << "1. **螞蟻數量影響**: 更多螞蟻能更好地利用多線程並行性\n";
        report << "2. **線程擴展性**: 分析不同螞蟻配置下的線程擴展效果\n";
        report << "3. **最佳配置**: 針對不同問題規模的最佳螞蟻-線程組合\n";
        
        report.close();
        std::cout << "分析報告已儲存至: comprehensive_ant_thread_report.md" << std::endl;
    }
};

int main() {
    try {
        ComprehensiveAntThreadExperiment experiment;
        experiment.runAllExperiments();
        
        std::cout << "\n=== 實驗完成！ ===" << std::endl;
        std::cout << "請查看以下文件：" << std::endl;
        std::cout << "- comprehensive_ant_thread_results.csv (詳細數據)" << std::endl;
        std::cout << "- comprehensive_ant_thread_report.md (分析報告)" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "實驗執行錯誤: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
