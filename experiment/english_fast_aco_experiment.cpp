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
#include "../include/aco/PerformanceMonitor.hpp"

struct InstanceConfig {
    std::string name;
    std::string file_path;
    int optimal_value;
    int cities;
};

struct ExperimentResult {
    std::string instance_name;
    std::string strategy_name;
    int num_threads;
    int run_id;
    double execution_time_ms;
    double best_cost;
    double gap_percent;
    double speedup;
    double efficiency;
};

class EnglishFastAcoExperiment {
private:
    std::vector<ExperimentResult> results;
    std::vector<InstanceConfig> instances;
    std::vector<int> thread_counts = {1, 2, 4, 8};
    
    // Experiment parameters
    int dev_iterations = 10;        // Reduced from original
    int dev_runs = 3;               // Reduced from original
    double time_budget_seconds = 2.0;  // Very short time budget
    
    void initializeInstances() {
        instances = {
            {"eil51", "d:/D_backup/2025/tum/Parallel ACO for TSP/data/eil51.tsp", 426, 51},
            {"kroA100", "d:/D_backup/2025/tum/Parallel ACO for TSP/data/kroA100.tsp", 21282, 100},
            {"ch150", "d:/D_backup/2025/tum/Parallel ACO for TSP/data/ch150.tsp", 6528, 150},
            {"gr202", "d:/D_backup/2025/tum/Parallel ACO for TSP/data/gr202.tsp", 40160, 202}
        };
    }
    
    double measureBaseline(const InstanceConfig& instance) {
        std::cout << "Measuring baseline for " << instance.name << " (single-threaded)..." << std::flush;
        
        auto graph_unique = aco::TSPLibReader::loadGraphFromTSPLib(instance.file_path);
        if (!graph_unique) {
            std::cerr << "Failed to load: " << instance.file_path << std::endl;
            return 0.0;
        }
        std::shared_ptr<Graph> graph = std::move(graph_unique);
        
        std::vector<double> times;
        for (int run = 0; run < 3; run++) {
            AcoParameters params;
            params.num_ants = 16;  // Standard ant count
            params.max_iterations = dev_iterations;
            params.num_threads = 1;
            params.alpha = 1.0;
            params.beta = 2.0;
            params.rho = 0.1;
            params.random_seed = 42 + run;
            
            auto start = std::chrono::high_resolution_clock::now();
            AcoEngine engine(graph, params);
            auto results = engine.run();
            auto end = std::chrono::high_resolution_clock::now();
            
            double time_ms = std::chrono::duration<double, std::milli>(end - start).count();
            times.push_back(time_ms);
        }
        
        double avg_time = 0.0;
        for (double t : times) avg_time += t;
        avg_time /= times.size();
        
        std::cout << " " << std::fixed << std::setprecision(1) << avg_time << "ms" << std::endl;
        return avg_time;
    }
    
    void runFixedIterationExperiment() {
        std::cout << "\n=== Fixed Iteration Experiment ===\n";
        std::cout << "Configuration: " << dev_iterations << " iterations, " << dev_runs << " runs per configuration\n\n";
        
        for (const auto& instance : instances) {
            std::cout << "Testing instance: " << instance.name << " (" << instance.cities << " cities)\n";
            
            auto graph_unique = aco::TSPLibReader::loadGraphFromTSPLib(instance.file_path);
            if (!graph_unique) {
                std::cerr << "Failed to load: " << instance.file_path << std::endl;
                continue;
            }
            std::shared_ptr<Graph> graph = std::move(graph_unique);
            
            double baseline_time = measureBaseline(instance);
            
            for (int threads : thread_counts) {
                std::string strategy_name = (threads == 1) ? "Serial_ACO" : "Parallel_ACO_" + std::to_string(threads);
                std::cout << "  Strategy: " << strategy_name << " (" << threads << " threads)\n";
                
                for (int run = 0; run < dev_runs; run++) {
                    AcoParameters params;
                    params.num_ants = 16;
                    params.max_iterations = dev_iterations;
                    params.num_threads = threads;
                    params.alpha = 1.0;
                    params.beta = 2.0;
                    params.rho = 0.1;
                    params.random_seed = 42 + run + threads;
                    
                    auto start = std::chrono::high_resolution_clock::now();
                    AcoEngine engine(graph, params);
                    auto aco_results = engine.run();
                    auto end = std::chrono::high_resolution_clock::now();
                    
                    double time_ms = std::chrono::duration<double, std::milli>(end - start).count();
                    double gap = ((aco_results.best_tour_length - instance.optimal_value) / double(instance.optimal_value)) * 100.0;
                    double speedup = (baseline_time > 0) ? baseline_time / time_ms : 1.0;
                    double efficiency = speedup / threads;
                    
                    std::cout << "    Run " << (run + 1) << "/" << dev_runs 
                              << " - Cost: " << std::fixed << std::setprecision(1) << aco_results.best_tour_length
                              << " (Gap: " << std::setprecision(1) << gap << "%)"
                              << " - Time: " << std::setprecision(2) << time_ms << "ms"
                              << " - Speedup: " << std::setprecision(2) << speedup << "x\n";
                    
                    ExperimentResult result;
                    result.instance_name = instance.name;
                    result.strategy_name = strategy_name;
                    result.num_threads = threads;
                    result.run_id = run + 1;
                    result.execution_time_ms = time_ms;
                    result.best_cost = aco_results.best_tour_length;
                    result.gap_percent = gap;
                    result.speedup = speedup;
                    result.efficiency = efficiency;
                    results.push_back(result);
                }
            }
            std::cout << std::endl;
        }
    }
    
    void runTimeBudgetExperiment() {
        std::cout << "\n=== Time Budget Experiment ===\n";
        std::cout << "Time budget: " << time_budget_seconds << " seconds per run\n\n";
        
        for (const auto& instance : instances) {
            std::cout << "Testing instance: " << instance.name << " (budget: " << time_budget_seconds << "s)\n";
            
            auto graph_unique = aco::TSPLibReader::loadGraphFromTSPLib(instance.file_path);
            if (!graph_unique) {
                std::cerr << "Failed to load: " << instance.file_path << std::endl;
                continue;
            }
            std::shared_ptr<Graph> graph = std::move(graph_unique);
            
            for (int threads : thread_counts) {
                std::string strategy_name = (threads == 1) ? "Serial_ACO" : "Parallel_ACO_" + std::to_string(threads);
                std::cout << "  Strategy: " << strategy_name << "\n";
                
                for (int run = 0; run < dev_runs; run++) {
                    AcoParameters params;
                    params.num_ants = 16;
                    params.max_iterations = 10000;  // Large number, will be limited by time
                    params.num_threads = threads;
                    params.alpha = 1.0;
                    params.beta = 2.0;
                    params.rho = 0.1;
                    params.random_seed = 42 + run + threads;
                    
                    AcoEngine engine(graph, params);
                    
                    // Set performance budget for time limit
                    PerformanceBudget budget(time_budget_seconds * 1000.0, 0, 1.0, 50.0);
                    
                    // Run with time budget
                    auto start = std::chrono::high_resolution_clock::now();
                    auto aco_results = engine.runWithBudget(budget);
                    auto end = std::chrono::high_resolution_clock::now();
                    
                    double actual_time = std::chrono::duration<double>(end - start).count();
                    
                    double gap = ((aco_results.best_tour_length - instance.optimal_value) / double(instance.optimal_value)) * 100.0;
                    
                    std::cout << "    Run " << (run + 1) << "/" << dev_runs 
                              << " - Iterations: " << aco_results.actual_iterations
                              << " - Cost: " << std::fixed << std::setprecision(1) << aco_results.best_tour_length
                              << " - Time: " << std::setprecision(2) << actual_time << "s\n";
                    
                    ExperimentResult result;
                    result.instance_name = instance.name;
                    result.strategy_name = strategy_name + "_Budget";
                    result.num_threads = threads;
                    result.run_id = run + 1;
                    result.execution_time_ms = actual_time * 1000.0;
                    result.best_cost = aco_results.best_tour_length;
                    result.gap_percent = gap;
                    result.speedup = 1.0;  // Speedup not meaningful in time budget context
                    result.efficiency = 1.0 / threads;
                    results.push_back(result);
                }
            }
            std::cout << std::endl;
        }
    }
    
public:
    void runExperiment(bool development_mode = true, bool enable_time_budget = false) {
        std::cout << "=== English Fast ACO Parallel Experiment ===\n";
        std::cout << "OpenMP: " << _OPENMP << ", Max threads: " << omp_get_max_threads() << std::endl;
        
        if (development_mode) {
            std::cout << "Mode: Development (fast testing)\n";
        } else {
            std::cout << "Mode: Production (comprehensive testing)\n";
            dev_iterations = 50;
            dev_runs = 10;
            time_budget_seconds = 10.0;
        }
        
        std::cout << "========================================\n";
        std::cout << "    Parallel ACO Scalability Study\n";
        std::cout << "========================================\n";
        std::cout << "Instances: eil51(51), kroA100(100), ch150(150), gr202(202)\n";
        std::cout << "Thread counts: ";
        for (size_t i = 0; i < thread_counts.size(); ++i) {
            std::cout << thread_counts[i];
            if (i < thread_counts.size() - 1) std::cout << ", ";
        }
        std::cout << "\n\n";
        
        initializeInstances();
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        runFixedIterationExperiment();
        
        if (enable_time_budget) {
            runTimeBudgetExperiment();
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
        
        saveResults();
        generateSummary();
        
        std::cout << "=== Experiment Completed Successfully ===\n";
        std::cout << "Total execution time: " << duration.count() << " seconds\n";
        std::cout << "Results saved to: english_fast_aco_results.csv\n";
        std::cout << "Summary saved to: english_fast_aco_summary.md\n";
    }
    
    void saveResults() {
        std::ofstream csv_file("english_fast_aco_results.csv");
        csv_file << "instance,strategy,threads,run_id,time_ms,best_cost,gap_percent,speedup,efficiency\n";
        
        for (const auto& result : results) {
            csv_file << result.instance_name << ","
                    << result.strategy_name << ","
                    << result.num_threads << ","
                    << result.run_id << ","
                    << std::fixed << std::setprecision(2) << result.execution_time_ms << ","
                    << std::setprecision(1) << result.best_cost << ","
                    << std::setprecision(2) << result.gap_percent << ","
                    << std::setprecision(3) << result.speedup << ","
                    << std::setprecision(3) << result.efficiency << "\n";
        }
        csv_file.close();
    }
    
    void generateSummary() {
        std::ofstream report("english_fast_aco_summary.md");
        report << "# English Fast ACO Parallel Experiment Results\n\n";
        
        report << "## Experiment Configuration\n";
        report << "- Development mode: " << dev_iterations << " iterations, " << dev_runs << " runs\n";
        report << "- Time budget: " << time_budget_seconds << " seconds per run\n";
        report << "- Thread counts: ";
        for (size_t i = 0; i < thread_counts.size(); ++i) {
            report << thread_counts[i];
            if (i < thread_counts.size() - 1) report << ", ";
        }
        report << "\n\n";
        
        // Group results by experiment type
        std::vector<std::string> experiment_types = {"Fixed_Iterations", "Time_Budget"};
        
        for (const auto& exp_type : experiment_types) {
            report << "## " << exp_type << " Results\n\n";
            
            for (const auto& instance : instances) {
                report << "### " << instance.name << " (" << instance.cities << " cities, optimal: " << instance.optimal_value << ")\n\n";
                report << "| Strategy | Threads | Avg Time | Best Cost | Avg Gap(%) | Avg Speedup | Efficiency |\n";
                report << "|----------|---------|----------|-----------|------------|-------------|------------|\n";
                
                for (int threads : thread_counts) {
                    std::string strategy_base = (threads == 1) ? "Serial_ACO" : "Parallel_ACO_" + std::to_string(threads);
                    std::string strategy_name = strategy_base;
                    if (exp_type == "Time_Budget") strategy_name += "_Budget";
                    
                    // Calculate averages for this configuration
                    double total_time = 0.0, total_cost = 0.0, total_gap = 0.0, total_speedup = 0.0;
                    int count = 0;
                    
                    for (const auto& result : results) {
                        if (result.instance_name == instance.name && result.strategy_name == strategy_name) {
                            total_time += result.execution_time_ms;
                            total_cost += result.best_cost;
                            total_gap += result.gap_percent;
                            total_speedup += result.speedup;
                            count++;
                        }
                    }
                    
                    if (count > 0) {
                        double avg_time = total_time / count;
                        double avg_cost = total_cost / count;
                        double avg_gap = total_gap / count;
                        double avg_speedup = total_speedup / count;
                        double efficiency = avg_speedup / threads;
                        
                        report << "| " << strategy_base << " | " << threads << " | "
                              << std::fixed << std::setprecision(1) << avg_time << "ms | "
                              << std::setprecision(0) << avg_cost << " | "
                              << std::setprecision(1) << avg_gap << "% | "
                              << std::setprecision(2) << avg_speedup << "x | "
                              << std::setprecision(3) << efficiency << " |\n";
                    }
                }
                report << "\n";
            }
        }
        
        report.close();
    }
};

int main(int argc, char* argv[]) {
    try {
        bool development_mode = true;
        bool enable_time_budget = false;
        
        // Parse command line arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--production") {
                development_mode = false;
            } else if (arg == "--time-budget") {
                enable_time_budget = true;
            } else if (arg == "--help") {
                std::cout << "Usage: " << argv[0] << " [options]\n";
                std::cout << "Options:\n";
                std::cout << "  --production     Run comprehensive experiment (50 iterations, 10 runs)\n";
                std::cout << "  --time-budget    Enable time budget experiment (slower)\n";
                std::cout << "  --help          Show this help message\n\n";
                std::cout << "Default: Development mode (10 iterations, 3 runs) with fixed iterations only\n";
                return 0;
            }
        }
        
        EnglishFastAcoExperiment experiment;
        experiment.runExperiment(development_mode, enable_time_budget);
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
