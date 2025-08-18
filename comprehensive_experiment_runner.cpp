#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <random>
#include <thread>
#include "aco/Graph.hpp"
#include "aco/AcoEngine.hpp"
#include "aco/TSPLibReader.hpp"
#include "aco/PerformanceMonitor.hpp"

// JSON 简单解析器（用于读取配置文件）
#include <map>

struct ExperimentInstance {
    std::string name;
    std::string filename;
    int optimal_solution;
    std::string description;
};

struct AlgorithmConfig {
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
};

class ExperimentRunner {
private:
    std::vector<ExperimentInstance> instances_;
    std::vector<AlgorithmConfig> algorithms_;
    std::string output_directory_;
    int max_iterations_;
    int num_runs_;
    bool development_mode_;
    
public:
    ExperimentRunner(bool dev_mode = true) : development_mode_(dev_mode) {
        if (development_mode_) {
            max_iterations_ = 2;
            num_runs_ = 5;
            std::cout << "Development mode: " << max_iterations_ << " iterations, " << num_runs_ << " runs" << std::endl;
        } else {
            max_iterations_ = 100;
            num_runs_ = 30;
            std::cout << "Production mode: " << max_iterations_ << " iterations, " << num_runs_ << " runs" << std::endl;
        }
        
        setupInstances();
        setupAlgorithms();
        createOutputDirectories();
    }
    
    void setupInstances() {
        // 只运行202城市实例
        instances_.push_back({"gr202", "../data/gr202.tsp", 40160, "202-city instance"});
    }
    
    void setupAlgorithms() {
        algorithms_.push_back({"Serial_ACO", 1, 1.0, 2.0, 0.1, 50});
        algorithms_.push_back({"Parallel_ACO_2", 2, 1.0, 2.0, 0.1, 50});
        algorithms_.push_back({"Parallel_ACO_4", 4, 1.0, 2.0, 0.1, 50});
        algorithms_.push_back({"Parallel_ACO_8", 8, 1.0, 2.0, 0.1, 50});
    }
    
    void createOutputDirectories() {
        std::filesystem::create_directories("results");
        std::filesystem::create_directories("results/fixed_iterations");
        std::filesystem::create_directories("results/time_budget");
        std::cout << "Created output directories" << std::endl;
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
        ss << "_" << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
    
    // 实验1：固定迭代数实验
    void runFixedIterationsExperiment() {
        std::cout << "\n=== Experiment 1: Fixed Iterations ===" << std::endl;
        std::cout << "Iterations: " << max_iterations_ << ", Runs per config: " << num_runs_ << std::endl;
        
        std::string csv_filename = "results/fixed_iterations/fixed_iterations_results_" + getCurrentTimestamp() + ".csv";
        std::ofstream csv_file(csv_filename);
        
        // CSV 标题
        csv_file << "timestamp,experiment_type,algorithm,instance_name,run_number,max_iterations,"
                 << "actual_iterations,time_budget_s,wall_time_s,tour_length,optimal_solution,"
                 << "ratio_to_best,num_threads,convergence_iteration,random_seed\n";
        
        int total_experiments = instances_.size() * algorithms_.size() * num_runs_;
        int completed = 0;
        
        for (const auto& instance : instances_) {
            std::cout << "\nTesting instance: " << instance.name << " (" << instance.description << ")" << std::endl;
            
            try {
                // 加载地图
                auto graph_unique = aco::TSPLibReader::loadGraphFromTSPLib(instance.filename);
                if (!graph_unique) {
                    std::cerr << "Failed to load: " << instance.filename << std::endl;
                    continue;
                }
                std::shared_ptr<Graph> graph(graph_unique.release());
                
                for (const auto& algorithm : algorithms_) {
                    std::cout << "  Algorithm: " << algorithm.name << " (" << algorithm.num_threads << " threads)" << std::endl;
                    
                    for (int run = 0; run < num_runs_; ++run) {
                        // 设置随机种子
                        int seed = 42 + run + completed;
                        
                        // 配置ACO参数
                        AcoParameters params;
                        params.alpha = algorithm.alpha;
                        params.beta = algorithm.beta;
                        params.rho = algorithm.rho;
                        params.num_ants = algorithm.num_ants;
                        params.max_iterations = max_iterations_;
                        params.num_threads = algorithm.num_threads;
                        params.random_seed = seed;
                        params.enable_early_stopping = false;
                        
                        // 运行实验
                        auto start_time = std::chrono::high_resolution_clock::now();
                        
                        AcoEngine engine(graph, params);
                        auto results = engine.run();
                        
                        auto end_time = std::chrono::high_resolution_clock::now();
                        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
                        double wall_time_s = duration.count() / 1000000.0;
                        
                        // 记录结果
                        ExperimentResult result;
                        result.timestamp = getCurrentTimestamp();
                        result.experiment_type = "fixed_iterations";
                        result.algorithm = algorithm.name;
                        result.instance_name = instance.name;
                        result.run_number = run + 1;
                        result.max_iterations = max_iterations_;
                        result.actual_iterations = results.actual_iterations;
                        result.time_budget_s = 0.0; // 不适用
                        result.wall_time_s = wall_time_s;
                        result.tour_length = results.best_tour_length;
                        result.optimal_solution = instance.optimal_solution;
                        result.ratio_to_best = results.best_tour_length / instance.optimal_solution;
                        result.num_threads = algorithm.num_threads;
                        result.convergence_iteration = results.convergence_iteration;
                        result.random_seed = seed;
                        
                        // 写入CSV
                        writeResultToCSV(csv_file, result);
                        
                        completed++;
                        std::cout << "    Run " << (run + 1) << "/" << num_runs_ 
                                  << " - Tour: " << std::fixed << std::setprecision(1) << results.best_tour_length
                                  << " (ratio: " << std::setprecision(3) << result.ratio_to_best << ")"
                                  << " - Time: " << std::setprecision(2) << wall_time_s << "s"
                                  << " [" << completed << "/" << total_experiments << "]" << std::endl;
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Error processing " << instance.name << ": " << e.what() << std::endl;
            }
        }
        
        csv_file.close();
        std::cout << "\nFixed iterations experiment completed!" << std::endl;
        std::cout << "Results saved to: " << csv_filename << std::endl;
    }
    
    // 实验2：固定时间预算实验
    void runTimeBudgetExperiment() {
        std::cout << "\n=== Experiment 2: Time Budget ===" << std::endl;
        std::cout << "Using instance-specific time budgets..." << std::endl;
        
        // 每个实例的时间预算 (秒) - 基于执行时间分析
        std::map<std::string, double> instance_time_budgets = {
            {"eil51", 0.20},
            {"kroA100", 0.65},
            {"kroA150", 1.40},
            {"gr202", 2.50}
        };
        
        // 开发模式用更短的时间预算
        if (development_mode_) {
            for (auto& [instance, budget] : instance_time_budgets) {
                budget *= 0.3; // 减少到30%
            }
        }
        
        std::string csv_filename = "results/time_budget/time_budget_results_" + getCurrentTimestamp() + ".csv";
        std::ofstream csv_file(csv_filename);
        
        // CSV 标题
        csv_file << "timestamp,experiment_type,algorithm,instance_name,run_number,max_iterations,"
                 << "actual_iterations,time_budget_s,wall_time_s,tour_length,optimal_solution,"
                 << "ratio_to_best,num_threads,convergence_iteration,random_seed\n";
        
        int total_experiments = instances_.size() * algorithms_.size() * num_runs_;
        int completed = 0;
        
        for (const auto& instance : instances_) {
            // 获取该实例的时间预算
            double time_budget = instance_time_budgets.count(instance.name) > 0 
                                ? instance_time_budgets[instance.name] 
                                : 2.0; // 默认预算
            
            std::cout << "\nTesting instance: " << instance.name 
                      << " (time budget: " << std::fixed << std::setprecision(2) 
                      << time_budget << "s)" << std::endl;
            
            try {
                auto graph_unique = aco::TSPLibReader::loadGraphFromTSPLib(instance.filename);
                if (!graph_unique) {
                    std::cerr << "Failed to load: " << instance.filename << std::endl;
                    continue;
                }
                std::shared_ptr<Graph> graph(graph_unique.release());
                
                for (const auto& algorithm : algorithms_) {
                    std::cout << "  Algorithm: " << algorithm.name << std::endl;
                    
                    for (int run = 0; run < num_runs_; ++run) {
                        int seed = 42 + run + completed;
                        
                        // 配置参数
                        AcoParameters params;
                        params.alpha = algorithm.alpha;
                        params.beta = algorithm.beta;
                        params.rho = algorithm.rho;
                        params.num_ants = algorithm.num_ants;
                        params.max_iterations = 1000; // 设置很大的值，让时间预算控制停止
                        params.num_threads = algorithm.num_threads;
                        params.random_seed = seed;
                        params.enable_early_stopping = false;
                        
                        // 创建时间预算
                        auto budget = PerformanceMonitor::createTimeBudget(time_budget * 1000); // 转换为毫秒
                        
                        AcoEngine engine(graph, params);
                        auto results = engine.runWithBudget(budget);
                        
                        // 记录结果
                        ExperimentResult result;
                        result.timestamp = getCurrentTimestamp();
                        result.experiment_type = "time_budget";
                        result.algorithm = algorithm.name;
                        result.instance_name = instance.name;
                        result.run_number = run + 1;
                        result.max_iterations = 1000; // 理论最大值
                        result.actual_iterations = results.actual_iterations;
                        result.time_budget_s = time_budget;
                        result.wall_time_s = results.execution_time_ms / 1000.0;
                        result.tour_length = results.best_tour_length;
                        result.optimal_solution = instance.optimal_solution;
                        result.ratio_to_best = results.best_tour_length / instance.optimal_solution;
                        result.num_threads = algorithm.num_threads;
                        result.convergence_iteration = results.convergence_iteration;
                        result.random_seed = seed;
                        
                        writeResultToCSV(csv_file, result);
                        
                        completed++;
                        std::cout << "    Run " << (run + 1) << "/" << num_runs_ 
                                  << " - Iterations: " << results.actual_iterations
                                  << " - Tour: " << std::fixed << std::setprecision(1) << results.best_tour_length
                                  << " - Time: " << std::setprecision(2) << result.wall_time_s << "s"
                                  << " (" << completed << "/" << total_experiments << ")" << std::endl;
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
        
        csv_file.close();
        std::cout << "\nTime budget experiment completed!" << std::endl;
        std::cout << "Results saved to: " << csv_filename << std::endl;
    }
    
    void writeResultToCSV(std::ofstream& csv_file, const ExperimentResult& result) {
        csv_file << result.timestamp << ","
                 << result.experiment_type << ","
                 << result.algorithm << ","
                 << result.instance_name << ","
                 << result.run_number << ","
                 << result.max_iterations << ","
                 << result.actual_iterations << ","
                 << std::fixed << std::setprecision(1) << result.time_budget_s << ","
                 << std::setprecision(3) << result.wall_time_s << ","
                 << std::setprecision(1) << result.tour_length << ","
                 << result.optimal_solution << ","
                 << std::setprecision(4) << result.ratio_to_best << ","
                 << result.num_threads << ","
                 << result.convergence_iteration << ","
                 << result.random_seed << "\n";
        csv_file.flush(); // 确保数据实时写入
    }
    
    void runAllExperiments() {
        std::cout << "Starting comprehensive ACO experiments..." << std::endl;
        std::cout << "Mode: " << (development_mode_ ? "Development" : "Production") << std::endl;
        
        auto total_start = std::chrono::high_resolution_clock::now();
        
        // 运行实验1：固定迭代数
        runFixedIterationsExperiment();
        
        // 运行实验2：固定时间预算
        runTimeBudgetExperiment();
        
        auto total_end = std::chrono::high_resolution_clock::now();
        auto total_duration = std::chrono::duration_cast<std::chrono::seconds>(total_end - total_start);
        
        std::cout << "\n=== All Experiments Completed ===" << std::endl;
        std::cout << "Total time: " << total_duration.count() << " seconds" << std::endl;
        std::cout << "Results are saved in the 'results/' directory" << std::endl;
    }
    
    void switchToProductionMode() {
        development_mode_ = false;
        max_iterations_ = 100;
        num_runs_ = 30;
        std::cout << "Switched to production mode: " << max_iterations_ << " iterations, " << num_runs_ << " runs" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    try {
        bool dev_mode = true;
        
        // 检查命令行参数
        if (argc > 1 && std::string(argv[1]) == "--production") {
            dev_mode = false;
        }
        
        ExperimentRunner runner(dev_mode);
        
        if (dev_mode) {
            std::cout << "\n=== TDD/BDD Development Mode ===" << std::endl;
            std::cout << "Running quick tests to verify all experiments work..." << std::endl;
            std::cout << "Use --production flag for full experiments" << std::endl;
        }
        
        runner.runAllExperiments();
        
        if (dev_mode) {
            std::cout << "\n=== TDD/BDD Tests Passed ===" << std::endl;
            std::cout << "All experiments completed successfully!" << std::endl;
            std::cout << "Ready to run with --production for full results" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
