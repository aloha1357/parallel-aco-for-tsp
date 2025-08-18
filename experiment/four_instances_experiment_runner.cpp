#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <random>
#include <thread>
#include <map>
#include <algorithm>
#include <numeric>
#include <cmath>

#include "../src/aco/Graph.hpp"
#include "../src/aco/AcoEngine.hpp"
#include "../src/aco/TSPLibReader.hpp"
#include "../src/aco/PerformanceMonitor.hpp"

/**
 * 四個實例(50,100,150,202城市)的完整平行ACO實驗
 * 
 * 實驗設計：
 * 1. 固定迭代數實驗 (100 iterations, 100 runs)
 * 2. 固定時間預算實驗 (基於1thread/100iter時間, 100 runs)
 * 
 * 測試配置：1, 2, 4, 8 threads
 * 評估指標：解品質、執行時間、加速比、效率
 */

struct ExperimentInstance {
    std::string name;
    std::string filename;
    int optimal_solution;
    int size;
    std::string category;
    std::string description;
};

struct ExperimentConfig {
    std::string name;
    int num_threads;
    double alpha;
    double beta;
    double rho;
    int num_ants;
};

struct ExperimentResult {
    std::string timestamp;
    std::string experiment_type;
    std::string algorithm;
    std::string instance_name;
    int instance_size;
    int run_number;
    int max_iterations;
    int actual_iterations;
    double time_budget_s;
    double wall_time_s;
    double tour_length;
    int optimal_solution;
    double ratio_to_best;
    int num_threads;
    int convergence_iteration;
    int random_seed;
    double speedup_ratio;
    double efficiency;
    double iterations_per_second;
};

struct BaselineTime {
    std::string instance_name;
    double average_time_s;
    double std_dev_s;
    int measurements;
};

class FourInstancesExperimentRunner {
private:
    std::vector<ExperimentInstance> instances_;
    std::vector<ExperimentConfig> thread_configs_;
    std::map<std::string, BaselineTime> baseline_times_;
    
    bool development_mode_;
    int production_iterations_;
    int production_runs_;
    int dev_iterations_;
    int dev_runs_;
    
    std::string results_directory_;
    
public:
    FourInstancesExperimentRunner(bool dev_mode = true) : development_mode_(dev_mode) {
        // 實驗參數設定
        production_iterations_ = 100;
        production_runs_ = 100;
        dev_iterations_ = 2;
        dev_runs_ = 5;
        
        results_directory_ = "experiment/results";
        
        setupInstances();
        setupThreadConfigurations();
        createOutputDirectories();
        
        if (development_mode_) {
            std::cout << "=== 開發模式 ===" << std::endl;
            std::cout << "迭代數: " << dev_iterations_ << ", 執行次數: " << dev_runs_ << std::endl;
        } else {
            std::cout << "=== 正式實驗模式 ===" << std::endl;
            std::cout << "迭代數: " << production_iterations_ << ", 執行次數: " << production_runs_ << std::endl;
        }
    }
    
    void setupInstances() {
        instances_ = {
            {"eil51", "data/eil51.tsp", 426, 51, "small", "51城市實例(近似50城市)"},
            {"kroA100", "data/kroA100.tsp", 21282, 100, "medium", "100城市Krolak問題A"},
            {"kroA150", "data/kroA150.tsp", 26524, 150, "large", "150城市Krolak問題A"},
            {"gr202", "data/gr202.tsp", 40160, 202, "extra_large", "202城市地理實例"}
        };
    }
    
    void setupThreadConfigurations() {
        // 標準ACO參數 (經過調優)
        double alpha = 1.0, beta = 2.0, rho = 0.1;
        int num_ants = 50;
        
        thread_configs_ = {
            {"Serial_ACO", 1, alpha, beta, rho, num_ants},
            {"Parallel_ACO_2", 2, alpha, beta, rho, num_ants},
            {"Parallel_ACO_4", 4, alpha, beta, rho, num_ants},
            {"Parallel_ACO_8", 8, alpha, beta, rho, num_ants}
        };
    }
    
    void createOutputDirectories() {
        std::filesystem::create_directories(results_directory_);
        std::filesystem::create_directories(results_directory_ + "/fixed_iterations");
        std::filesystem::create_directories(results_directory_ + "/time_budget");
        std::filesystem::create_directories(results_directory_ + "/baseline_measurements");
        std::filesystem::create_directories(results_directory_ + "/analysis");
        
        std::cout << "創建輸出目錄: " << results_directory_ << std::endl;
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
        return ss.str();
    }
    
    // 步驟1: 測量基準執行時間 (1 thread, 100 iterations)
    void measureBaselineTimes() {
        std::cout << "\n=== 步驟1: 測量基準執行時間 ===" << std::endl;
        std::cout << "測量 1 thread, 100 iterations 的執行時間..." << std::endl;
        
        std::string csv_filename = results_directory_ + "/baseline_measurements/baseline_times_" + getCurrentTimestamp() + ".csv";
        std::ofstream csv_file(csv_filename);
        
        csv_file << "instance_name,instance_size,run_number,wall_time_s,tour_length,optimal_solution,ratio_to_best\n";
        
        int baseline_iterations = development_mode_ ? dev_iterations_ : production_iterations_;
        int baseline_runs = development_mode_ ? 3 : 5; // 基準測量用較少次數
        
        for (const auto& instance : instances_) {
            std::cout << "\n測量實例: " << instance.name << " (" << instance.size << "城市)" << std::endl;
            
            std::vector<double> execution_times;
            
            try {
                auto graph_unique = aco::TSPLibReader::loadGraphFromTSPLib(instance.filename);
                if (!graph_unique) {
                    std::cerr << "無法載入: " << instance.filename << std::endl;
                    continue;
                }
                std::shared_ptr<Graph> graph(graph_unique.release());
                
                for (int run = 0; run < baseline_runs; ++run) {
                    // 配置ACO參數 (1 thread)
                    AcoParameters params;
                    params.alpha = 1.0;
                    params.beta = 2.0;
                    params.rho = 0.1;
                    params.num_ants = 50;
                    params.max_iterations = baseline_iterations;
                    params.num_threads = 1;
                    params.random_seed = 42 + run;
                    params.enable_early_stopping = false;
                    
                    // 執行測量
                    auto start_time = std::chrono::high_resolution_clock::now();
                    
                    AcoEngine engine(graph, params);
                    auto results = engine.run();
                    
                    auto end_time = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
                    double wall_time_s = duration.count() / 1000000.0;
                    
                    execution_times.push_back(wall_time_s);
                    
                    // 記錄到CSV
                    csv_file << instance.name << "," << instance.size << "," << (run + 1) << ","
                             << std::fixed << std::setprecision(3) << wall_time_s << ","
                             << std::setprecision(1) << results.best_tour_length << ","
                             << instance.optimal_solution << ","
                             << std::setprecision(4) << (results.best_tour_length / instance.optimal_solution) << "\n";
                    
                    std::cout << "  執行 " << (run + 1) << "/" << baseline_runs 
                              << " - 時間: " << std::setprecision(2) << wall_time_s << "s"
                              << " - 解長度: " << std::setprecision(1) << results.best_tour_length << std::endl;
                }
                
                // 計算統計值
                double mean_time = std::accumulate(execution_times.begin(), execution_times.end(), 0.0) / execution_times.size();
                double variance = 0.0;
                for (double time : execution_times) {
                    variance += (time - mean_time) * (time - mean_time);
                }
                double std_dev = std::sqrt(variance / execution_times.size());
                
                // 儲存基準時間
                baseline_times_[instance.name] = {instance.name, mean_time, std_dev, (int)execution_times.size()};
                
                std::cout << "  基準時間: " << std::setprecision(3) << mean_time 
                          << " ± " << std::setprecision(3) << std_dev << " 秒" << std::endl;
                
            } catch (const std::exception& e) {
                std::cerr << "錯誤: " << e.what() << std::endl;
            }
        }
        
        csv_file.close();
        std::cout << "\n基準時間測量完成! 結果儲存至: " << csv_filename << std::endl;
        
        // 輸出基準時間總結
        std::cout << "\n=== 基準時間總結 ===" << std::endl;
        for (const auto& [name, baseline] : baseline_times_) {
            std::cout << name << ": " << std::fixed << std::setprecision(3) 
                      << baseline.average_time_s << "s (±" << baseline.std_dev_s << "s)" << std::endl;
        }
    }
    
    // 實驗1: 固定迭代數實驗
    void runFixedIterationsExperiment() {
        std::cout << "\n=== 實驗1: 固定迭代數實驗 ===" << std::endl;
        
        int iterations = development_mode_ ? dev_iterations_ : production_iterations_;
        int runs = development_mode_ ? dev_runs_ : production_runs_;
        
        std::cout << "設定: " << iterations << " 迭代, " << runs << " 次執行" << std::endl;
        
        std::string csv_filename = results_directory_ + "/fixed_iterations/fixed_iterations_" + getCurrentTimestamp() + ".csv";
        std::ofstream csv_file(csv_filename);
        
        // CSV標題
        csv_file << "timestamp,experiment_type,algorithm,instance_name,instance_size,run_number,"
                 << "max_iterations,actual_iterations,time_budget_s,wall_time_s,tour_length,"
                 << "optimal_solution,ratio_to_best,num_threads,convergence_iteration,"
                 << "random_seed,speedup_ratio,efficiency,iterations_per_second\n";
        
        int total_experiments = instances_.size() * thread_configs_.size() * runs;
        int completed = 0;
        
        // 儲存1thread的結果用於計算加速比
        std::map<std::string, std::vector<double>> serial_times;
        
        for (const auto& instance : instances_) {
            std::cout << "\n測試實例: " << instance.name << " (" << instance.size << "城市)" << std::endl;
            
            try {
                auto graph_unique = aco::TSPLibReader::loadGraphFromTSPLib(instance.filename);
                if (!graph_unique) {
                    std::cerr << "無法載入: " << instance.filename << std::endl;
                    continue;
                }
                std::shared_ptr<Graph> graph(graph_unique.release());
                
                // 先執行serial版本收集基準時間
                std::vector<double> instance_serial_times;
                
                for (const auto& config : thread_configs_) {
                    std::cout << "  演算法: " << config.name << " (" << config.num_threads << " threads)" << std::endl;
                    
                    std::vector<double> current_run_times;
                    
                    for (int run = 0; run < runs; ++run) {
                        int seed = 1000 + completed + run;
                        
                        // 配置ACO參數
                        AcoParameters params;
                        params.alpha = config.alpha;
                        params.beta = config.beta;
                        params.rho = config.rho;
                        params.num_ants = config.num_ants;
                        params.max_iterations = iterations;
                        params.num_threads = config.num_threads;
                        params.random_seed = seed;
                        params.enable_early_stopping = false;
                        
                        // 執行實驗
                        auto start_time = std::chrono::high_resolution_clock::now();
                        
                        AcoEngine engine(graph, params);
                        auto results = engine.run();
                        
                        auto end_time = std::chrono::high_resolution_clock::now();
                        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
                        double wall_time_s = duration.count() / 1000000.0;
                        
                        current_run_times.push_back(wall_time_s);
                        
                        // 如果是serial版本，儲存時間用於計算加速比
                        if (config.num_threads == 1) {
                            instance_serial_times.push_back(wall_time_s);
                        }
                        
                        // 計算性能指標
                        double speedup_ratio = 1.0;
                        double efficiency = 1.0;
                        double iterations_per_second = iterations / wall_time_s;
                        
                        if (config.num_threads > 1 && !instance_serial_times.empty()) {
                            // 使用對應run的serial時間計算加速比
                            if (run < instance_serial_times.size()) {
                                speedup_ratio = instance_serial_times[run] / wall_time_s;
                                efficiency = speedup_ratio / config.num_threads;
                            }
                        }
                        
                        // 準備結果
                        ExperimentResult result;
                        result.timestamp = getCurrentTimestamp();
                        result.experiment_type = "fixed_iterations";
                        result.algorithm = config.name;
                        result.instance_name = instance.name;
                        result.instance_size = instance.size;
                        result.run_number = run + 1;
                        result.max_iterations = iterations;
                        result.actual_iterations = results.actual_iterations;
                        result.time_budget_s = 0.0; // 不適用
                        result.wall_time_s = wall_time_s;
                        result.tour_length = results.best_tour_length;
                        result.optimal_solution = instance.optimal_solution;
                        result.ratio_to_best = results.best_tour_length / instance.optimal_solution;
                        result.num_threads = config.num_threads;
                        result.convergence_iteration = results.convergence_iteration;
                        result.random_seed = seed;
                        result.speedup_ratio = speedup_ratio;
                        result.efficiency = efficiency;
                        result.iterations_per_second = iterations_per_second;
                        
                        // 寫入CSV
                        writeResultToCSV(csv_file, result);
                        
                        completed++;
                        
                        std::cout << "    執行 " << (run + 1) << "/" << runs 
                                  << " - 解長度: " << std::fixed << std::setprecision(1) << results.best_tour_length
                                  << " (比例: " << std::setprecision(3) << result.ratio_to_best << ")"
                                  << " - 時間: " << std::setprecision(2) << wall_time_s << "s"
                                  << " - 加速比: " << std::setprecision(2) << speedup_ratio
                                  << " [" << completed << "/" << total_experiments << "]" << std::endl;
                    }
                    
                    // 儲存這個配置的平均時間
                    serial_times[instance.name + "_" + config.name] = current_run_times;
                }
                
            } catch (const std::exception& e) {
                std::cerr << "處理 " << instance.name << " 時發生錯誤: " << e.what() << std::endl;
            }
        }
        
        csv_file.close();
        std::cout << "\n固定迭代數實驗完成!" << std::endl;
        std::cout << "結果儲存至: " << csv_filename << std::endl;
    }
    
    // 實驗2: 固定時間預算實驗
    void runTimeBudgetExperiment() {
        std::cout << "\n=== 實驗2: 固定時間預算實驗 ===" << std::endl;
        
        if (baseline_times_.empty()) {
            std::cout << "錯誤: 必須先執行基準時間測量!" << std::endl;
            return;
        }
        
        int runs = development_mode_ ? dev_runs_ : production_runs_;
        std::cout << "使用基準時間作為各配置的時間預算，執行 " << runs << " 次" << std::endl;
        
        std::string csv_filename = results_directory_ + "/time_budget/time_budget_" + getCurrentTimestamp() + ".csv";
        std::ofstream csv_file(csv_filename);
        
        // CSV標題
        csv_file << "timestamp,experiment_type,algorithm,instance_name,instance_size,run_number,"
                 << "max_iterations,actual_iterations,time_budget_s,wall_time_s,tour_length,"
                 << "optimal_solution,ratio_to_best,num_threads,convergence_iteration,"
                 << "random_seed,speedup_ratio,efficiency,iterations_per_second\n";
        
        int total_experiments = instances_.size() * thread_configs_.size() * runs;
        int completed = 0;
        
        for (const auto& instance : instances_) {
            // 獲取該實例的基準時間作為時間預算
            if (baseline_times_.find(instance.name) == baseline_times_.end()) {
                std::cerr << "找不到 " << instance.name << " 的基準時間，跳過" << std::endl;
                continue;
            }
            
            double time_budget = baseline_times_[instance.name].average_time_s;
            
            std::cout << "\n測試實例: " << instance.name 
                      << " (時間預算: " << std::fixed << std::setprecision(2) << time_budget << "s)" << std::endl;
            
            try {
                auto graph_unique = aco::TSPLibReader::loadGraphFromTSPLib(instance.filename);
                if (!graph_unique) {
                    std::cerr << "無法載入: " << instance.filename << std::endl;
                    continue;
                }
                std::shared_ptr<Graph> graph(graph_unique.release());
                
                // 收集serial結果用於計算加速比
                std::vector<double> serial_times_for_speedup;
                std::vector<int> serial_iterations_for_comparison;
                
                for (const auto& config : thread_configs_) {
                    std::cout << "  演算法: " << config.name << std::endl;
                    
                    for (int run = 0; run < runs; ++run) {
                        int seed = 2000 + completed + run;
                        
                        // 配置參數 - 設定很大的迭代數讓時間預算控制停止
                        AcoParameters params;
                        params.alpha = config.alpha;
                        params.beta = config.beta;
                        params.rho = config.rho;
                        params.num_ants = config.num_ants;
                        params.max_iterations = 10000; // 設定很大的值
                        params.num_threads = config.num_threads;
                        params.random_seed = seed;
                        params.enable_early_stopping = false;
                        
                        // 創建時間預算
                        auto budget = PerformanceMonitor::createTimeBudget(time_budget * 1000); // 轉換為毫秒
                        
                        AcoEngine engine(graph, params);
                        auto results = engine.runWithBudget(budget);
                        
                        double wall_time_s = results.execution_time_ms / 1000.0;
                        
                        // 收集serial數據用於加速比計算
                        if (config.num_threads == 1) {
                            serial_times_for_speedup.push_back(wall_time_s);
                            serial_iterations_for_comparison.push_back(results.actual_iterations);
                        }
                        
                        // 計算性能指標
                        double speedup_ratio = 1.0;
                        double efficiency = 1.0;
                        double iterations_per_second = results.actual_iterations / wall_time_s;
                        
                        if (config.num_threads > 1 && !serial_times_for_speedup.empty()) {
                            if (run < serial_times_for_speedup.size()) {
                                speedup_ratio = serial_times_for_speedup[run] / wall_time_s;
                                efficiency = speedup_ratio / config.num_threads;
                            }
                        }
                        
                        // 準備結果
                        ExperimentResult result;
                        result.timestamp = getCurrentTimestamp();
                        result.experiment_type = "time_budget";
                        result.algorithm = config.name;
                        result.instance_name = instance.name;
                        result.instance_size = instance.size;
                        result.run_number = run + 1;
                        result.max_iterations = 10000; // 理論最大值
                        result.actual_iterations = results.actual_iterations;
                        result.time_budget_s = time_budget;
                        result.wall_time_s = wall_time_s;
                        result.tour_length = results.best_tour_length;
                        result.optimal_solution = instance.optimal_solution;
                        result.ratio_to_best = results.best_tour_length / instance.optimal_solution;
                        result.num_threads = config.num_threads;
                        result.convergence_iteration = results.convergence_iteration;
                        result.random_seed = seed;
                        result.speedup_ratio = speedup_ratio;
                        result.efficiency = efficiency;
                        result.iterations_per_second = iterations_per_second;
                        
                        writeResultToCSV(csv_file, result);
                        
                        completed++;
                        
                        std::cout << "    執行 " << (run + 1) << "/" << runs 
                                  << " - 迭代數: " << results.actual_iterations
                                  << " - 解長度: " << std::fixed << std::setprecision(1) << results.best_tour_length
                                  << " - 時間: " << std::setprecision(2) << wall_time_s << "s"
                                  << " - 加速比: " << std::setprecision(2) << speedup_ratio
                                  << " (" << completed << "/" << total_experiments << ")" << std::endl;
                    }
                }
                
            } catch (const std::exception& e) {
                std::cerr << "錯誤: " << e.what() << std::endl;
            }
        }
        
        csv_file.close();
        std::cout << "\n固定時間預算實驗完成!" << std::endl;
        std::cout << "結果儲存至: " << csv_filename << std::endl;
    }
    
    void writeResultToCSV(std::ofstream& csv_file, const ExperimentResult& result) {
        csv_file << result.timestamp << ","
                 << result.experiment_type << ","
                 << result.algorithm << ","
                 << result.instance_name << ","
                 << result.instance_size << ","
                 << result.run_number << ","
                 << result.max_iterations << ","
                 << result.actual_iterations << ","
                 << std::fixed << std::setprecision(3) << result.time_budget_s << ","
                 << std::setprecision(3) << result.wall_time_s << ","
                 << std::setprecision(1) << result.tour_length << ","
                 << result.optimal_solution << ","
                 << std::setprecision(4) << result.ratio_to_best << ","
                 << result.num_threads << ","
                 << result.convergence_iteration << ","
                 << result.random_seed << ","
                 << std::setprecision(3) << result.speedup_ratio << ","
                 << std::setprecision(3) << result.efficiency << ","
                 << std::setprecision(2) << result.iterations_per_second << "\n";
        csv_file.flush();
    }
    
    void runCompleteExperiments() {
        std::cout << "\n========================================" << std::endl;
        std::cout << "    四個實例平行ACO完整實驗開始" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "模式: " << (development_mode_ ? "開發測試" : "正式實驗") << std::endl;
        std::cout << "實例: eil51(51), kroA100(100), kroA150(150), gr202(202)" << std::endl;
        std::cout << "線程配置: 1, 2, 4, 8" << std::endl;
        
        auto total_start = std::chrono::high_resolution_clock::now();
        
        try {
            // 步驟1: 測量基準時間
            measureBaselineTimes();
            
            // 步驟2: 固定迭代數實驗
            runFixedIterationsExperiment();
            
            // 步驟3: 固定時間預算實驗  
            runTimeBudgetExperiment();
            
            auto total_end = std::chrono::high_resolution_clock::now();
            auto total_duration = std::chrono::duration_cast<std::chrono::seconds>(total_end - total_start);
            
            std::cout << "\n========================================" << std::endl;
            std::cout << "         所有實驗完成!" << std::endl;
            std::cout << "========================================" << std::endl;
            std::cout << "總執行時間: " << total_duration.count() << " 秒" << std::endl;
            std::cout << "結果儲存在: " << results_directory_ << std::endl;
            
            if (development_mode_) {
                std::cout << "\n注意: 這是開發模式的快速測試結果" << std::endl;
                std::cout << "正式實驗請使用 --production 參數" << std::endl;
            }
            
        } catch (const std::exception& e) {
            std::cerr << "實驗執行錯誤: " << e.what() << std::endl;
            throw;
        }
    }
    
    void switchToProductionMode() {
        development_mode_ = false;
        std::cout << "切換至正式實驗模式: " << production_iterations_ 
                  << " 迭代, " << production_runs_ << " 次執行" << std::endl;
    }
};

void printUsage(const char* program_name) {
    std::cout << "使用方法: " << program_name << " [選項]" << std::endl;
    std::cout << "選項:" << std::endl;
    std::cout << "  --production    執行完整的正式實驗 (100 iterations, 100 runs)" << std::endl;
    std::cout << "  --help         顯示此說明" << std::endl;
    std::cout << std::endl;
    std::cout << "預設為開發模式 (2 iterations, 5 runs) 用於快速驗證" << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        bool dev_mode = true;
        
        // 處理命令行參數
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--production") {
                dev_mode = false;
            } else if (arg == "--help") {
                printUsage(argv[0]);
                return 0;
            } else {
                std::cerr << "未知參數: " << arg << std::endl;
                printUsage(argv[0]);
                return 1;
            }
        }
        
        FourInstancesExperimentRunner runner(dev_mode);
        runner.runCompleteExperiments();
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "程式執行錯誤: " << e.what() << std::endl;
        return 1;
    }
}
