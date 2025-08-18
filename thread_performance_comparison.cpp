#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <map>
#include "aco/Graph.hpp"
#include "aco/AcoEngine.hpp"
#include "aco/TSPLibReader.hpp"
#include "aco/PerformanceMonitor.hpp"

struct ThreadTestResult {
    int num_threads;
    double execution_time_ms;
    double best_tour_length;
    double speedup;
    double efficiency;
    int actual_iterations;
    std::string map_name;
};

class ThreadPerformanceComparator {
private:
    std::vector<std::string> map_files_;
    std::vector<int> thread_counts_;
    AcoParameters base_params_;
    int num_runs_per_test_;
    
public:
    ThreadPerformanceComparator() : num_runs_per_test_(3) {
        // 设置测试地图
        map_files_ = {
            "data/eil51.tsp",     // 小地图 - 51 城市
            "data/st70.tsp",      // 中小地图 - 70 城市 
            "data/eil76.tsp",     // 中小地图 - 76 城市
            "data/kroA100.tsp",   // 中等地图 - 100 城市
            "data/ch130.tsp",     // 中大地图 - 130 城市
            "data/kroA150.tsp"    // 大地图 - 150 城市
        };
        
        // 设置测试线程数
        thread_counts_ = {1, 2, 4, 6, 8, 12, 16};
        
        // 设置基础 ACO 参数
        base_params_.alpha = 1.0;
        base_params_.beta = 2.0;
        base_params_.rho = 0.1;
        base_params_.num_ants = 50;
        base_params_.max_iterations = 100;
        base_params_.random_seed = 42;
        base_params_.enable_early_stopping = false;
    }
    
    void setTestMaps(const std::vector<std::string>& maps) {
        map_files_ = maps;
    }
    
    void setThreadCounts(const std::vector<int>& threads) {
        thread_counts_ = threads;
    }
    
    void setBaseParameters(const AcoParameters& params) {
        base_params_ = params;
    }
    
    void setNumRuns(int runs) {
        num_runs_per_test_ = runs;
    }
    
    std::vector<ThreadTestResult> runComparisonTest() {
        std::vector<ThreadTestResult> all_results;
        
        std::cout << "=== 多线程性能比较测试 ===" << std::endl;
        std::cout << "测试地图数量: " << map_files_.size() << std::endl;
        std::cout << "测试线程数: ";
        for (size_t i = 0; i < thread_counts_.size(); ++i) {
            std::cout << thread_counts_[i];
            if (i < thread_counts_.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
        std::cout << "每组测试运行次数: " << num_runs_per_test_ << std::endl;
        std::cout << "迭代次数: " << base_params_.max_iterations << std::endl;
        std::cout << "蚂蚁数量: " << base_params_.num_ants << std::endl;
        std::cout << std::endl;
        
        for (const auto& map_file : map_files_) {
            std::cout << "正在测试地图: " << map_file << std::endl;
            
            try {
                // 加载地图
                auto graph = aco::TSPLibReader::loadGraphFromTSPLib(map_file);
                std::string map_name = extractMapName(map_file);
                
                std::cout << "  地图大小: " << graph->size() << " 城市" << std::endl;
                
                // 存储每个线程数的测试结果
                std::vector<ThreadTestResult> map_results;
                double baseline_time = 0.0;
                
                for (int thread_count : thread_counts_) {
                    std::cout << "    测试 " << thread_count << " 线程...";
                    
                    // 多次运行取平均值
                    std::vector<double> execution_times;
                    std::vector<double> tour_lengths;
                    std::vector<int> iterations_counts;
                    
                    for (int run = 0; run < num_runs_per_test_; ++run) {
                        AcoParameters params = base_params_;
                        params.num_threads = thread_count;
                        params.random_seed = 42 + run; // 每次运行使用不同种子
                        
                        AcoEngine engine(graph, params);
                        auto results = engine.run();
                        
                        execution_times.push_back(results.execution_time_ms);
                        tour_lengths.push_back(results.best_tour_length);
                        iterations_counts.push_back(results.actual_iterations);
                    }
                    
                    // 计算平均值
                    double avg_time = calculateAverage(execution_times);
                    double avg_length = calculateAverage(tour_lengths);
                    double avg_iterations = calculateAverage(iterations_counts);
                    
                    // 计算加速比和效率
                    if (thread_count == 1) {
                        baseline_time = avg_time;
                    }
                    
                    double speedup = (baseline_time > 0) ? baseline_time / avg_time : 1.0;
                    double efficiency = speedup / thread_count * 100.0;
                    
                    ThreadTestResult result;
                    result.num_threads = thread_count;
                    result.execution_time_ms = avg_time;
                    result.best_tour_length = avg_length;
                    result.speedup = speedup;
                    result.efficiency = efficiency;
                    result.actual_iterations = static_cast<int>(avg_iterations);
                    result.map_name = map_name;
                    
                    map_results.push_back(result);
                    all_results.push_back(result);
                    
                    std::cout << " 时间: " << std::fixed << std::setprecision(1) 
                              << avg_time << "ms, 加速比: " << std::setprecision(2) 
                              << speedup << "x" << std::endl;
                }
                
                // 打印该地图的详细结果
                printMapResults(map_name, map_results);
                std::cout << std::endl;
                
            } catch (const std::exception& e) {
                std::cerr << "  错误: 无法加载地图 " << map_file << " - " << e.what() << std::endl;
            }
        }
        
        return all_results;
    }
    
    void printSummaryReport(const std::vector<ThreadTestResult>& results) {
        std::cout << "\n=== 性能总结报告 ===" << std::endl;
        
        // 按地图分组统计
        std::map<std::string, std::vector<ThreadTestResult>> results_by_map;
        for (const auto& result : results) {
            results_by_map[result.map_name].push_back(result);
        }
        
        std::cout << std::left << std::setw(12) << "地图"
                  << std::setw(8) << "线程数"
                  << std::setw(12) << "时间(ms)"
                  << std::setw(10) << "加速比"
                  << std::setw(10) << "效率(%)"
                  << std::setw(12) << "最佳距离" << std::endl;
        std::cout << std::string(64, '-') << std::endl;
        
        for (const auto& [map_name, map_results] : results_by_map) {
            for (const auto& result : map_results) {
                std::cout << std::left << std::setw(12) << map_name
                          << std::setw(8) << result.num_threads
                          << std::setw(12) << std::fixed << std::setprecision(1) << result.execution_time_ms
                          << std::setw(10) << std::setprecision(2) << result.speedup
                          << std::setw(10) << std::setprecision(1) << result.efficiency
                          << std::setw(12) << std::setprecision(1) << result.best_tour_length << std::endl;
            }
            std::cout << std::endl;
        }
        
        // 分析最佳线程数
        analyzeOptimalThreadCounts(results_by_map);
    }
    
    void saveResultsToCSV(const std::vector<ThreadTestResult>& results, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "无法创建文件: " << filename << std::endl;
            return;
        }
        
        // CSV 标题
        file << "Map,NumThreads,ExecutionTime_ms,BestTourLength,Speedup,Efficiency_percent,ActualIterations\n";
        
        // 数据行
        for (const auto& result : results) {
            file << result.map_name << ","
                 << result.num_threads << ","
                 << std::fixed << std::setprecision(2) << result.execution_time_ms << ","
                 << std::setprecision(2) << result.best_tour_length << ","
                 << std::setprecision(3) << result.speedup << ","
                 << std::setprecision(2) << result.efficiency << ","
                 << result.actual_iterations << "\n";
        }
        
        file.close();
        std::cout << "结果已保存到: " << filename << std::endl;
    }

private:
    std::string extractMapName(const std::string& filepath) {
        size_t last_slash = filepath.find_last_of("/\\");
        size_t last_dot = filepath.find_last_of(".");
        
        if (last_slash != std::string::npos && last_dot != std::string::npos) {
            return filepath.substr(last_slash + 1, last_dot - last_slash - 1);
        } else if (last_dot != std::string::npos) {
            return filepath.substr(0, last_dot);
        }
        return filepath;
    }
    
    double calculateAverage(const std::vector<double>& values) {
        if (values.empty()) return 0.0;
        double sum = 0.0;
        for (double val : values) {
            sum += val;
        }
        return sum / values.size();
    }
    
    double calculateAverage(const std::vector<int>& values) {
        if (values.empty()) return 0.0;
        double sum = 0.0;
        for (int val : values) {
            sum += val;
        }
        return sum / values.size();
    }
    
    void printMapResults(const std::string& map_name, const std::vector<ThreadTestResult>& results) {
        std::cout << "  " << map_name << " 详细结果:" << std::endl;
        std::cout << "    线程数 | 时间(ms) | 加速比 | 效率(%) | 最佳距离" << std::endl;
        std::cout << "    " << std::string(50, '-') << std::endl;
        
        for (const auto& result : results) {
            std::cout << "    " << std::setw(6) << result.num_threads
                      << " | " << std::setw(8) << std::fixed << std::setprecision(1) << result.execution_time_ms
                      << " | " << std::setw(6) << std::setprecision(2) << result.speedup
                      << " | " << std::setw(7) << std::setprecision(1) << result.efficiency
                      << " | " << std::setw(9) << std::setprecision(1) << result.best_tour_length << std::endl;
        }
    }
    
    void analyzeOptimalThreadCounts(const std::map<std::string, std::vector<ThreadTestResult>>& results_by_map) {
        std::cout << "\n=== 最佳线程数分析 ===" << std::endl;
        
        for (const auto& [map_name, results] : results_by_map) {
            // 找到最佳加速比
            auto best_speedup_iter = std::max_element(results.begin(), results.end(),
                [](const ThreadTestResult& a, const ThreadTestResult& b) {
                    return a.speedup < b.speedup;
                });
            
            // 找到最佳效率
            auto best_efficiency_iter = std::max_element(results.begin(), results.end(),
                [](const ThreadTestResult& a, const ThreadTestResult& b) {
                    return a.efficiency < b.efficiency;
                });
            
            // 找到效率 > 70% 的最大线程数
            int max_efficient_threads = 1;
            for (const auto& result : results) {
                if (result.efficiency >= 70.0 && result.num_threads > max_efficient_threads) {
                    max_efficient_threads = result.num_threads;
                }
            }
            
            std::cout << map_name << ":" << std::endl;
            std::cout << "  最佳加速比: " << best_speedup_iter->speedup << "x (" 
                      << best_speedup_iter->num_threads << " 线程)" << std::endl;
            std::cout << "  最佳效率: " << best_efficiency_iter->efficiency << "% (" 
                      << best_efficiency_iter->num_threads << " 线程)" << std::endl;
            std::cout << "  推荐线程数: " << max_efficient_threads 
                      << " (效率≥70%的最大线程数)" << std::endl;
        }
    }
};

int main() {
    try {
        ThreadPerformanceComparator comparator;
        
        // 可以自定义测试参数
        std::cout << "是否使用默认设置? (y/n): ";
        char choice;
        std::cin >> choice;
        
        if (choice == 'n' || choice == 'N') {
            // 自定义设置
            AcoParameters custom_params;
            custom_params.alpha = 1.0;
            custom_params.beta = 2.0;
            custom_params.rho = 0.1;
            
            std::cout << "输入迭代次数 (建议 50-200): ";
            std::cin >> custom_params.max_iterations;
            
            std::cout << "输入蚂蚁数量 (建议 20-100): ";
            std::cin >> custom_params.num_ants;
            
            std::cout << "输入每组测试运行次数 (建议 3-5): ";
            int num_runs;
            std::cin >> num_runs;
            
            comparator.setBaseParameters(custom_params);
            comparator.setNumRuns(num_runs);
        }
        
        // 运行比较测试
        auto results = comparator.runComparisonTest();
        
        // 打印总结报告
        comparator.printSummaryReport(results);
        
        // 保存结果到 CSV
        std::string csv_filename = "thread_performance_results.csv";
        comparator.saveResultsToCSV(results, csv_filename);
        
        // 生成建议
        std::cout << "\n=== 建议 ===" << std::endl;
        std::cout << "1. 对于小地图(<100城市)，2-4线程通常是最佳选择" << std::endl;
        std::cout << "2. 对于大地图(>100城市)，可以尝试更多线程数" << std::endl;
        std::cout << "3. 当效率低于70%时，增加线程数的收益递减" << std::endl;
        std::cout << "4. 实际选择应考虑系统负载和其他运行程序" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
