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
    int iterations = 25;  // Reduced iterations for speed
    int runs_per_config = 3;  // Reduced runs for speed
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

class FastAntThreadExperiment {
private:
    std::vector<ExperimentResult> results;
    
    void runSingleExperiment(const ExperimentConfig& config) {
        std::cout << "\n=== " << config.instance_name << " ===\n";
        
        // Load TSP instance ONCE
        auto graph_unique = aco::TSPLibReader::loadGraphFromTSPLib(config.file_path);
        if (!graph_unique) {
            std::cerr << "Failed to load: " << config.file_path << std::endl;
            return;
        }
        std::shared_ptr<Graph> graph = std::move(graph_unique);
        
        std::cout << "Cities: " << graph->size() 
                  << ", Optimal: " << config.optimal_value << std::endl;
        
        double baseline_time = 0.0;
        
        for (int ants : config.ant_counts) {
            std::cout << "\nAnts " << ants << ": ";
            
            for (int threads : config.thread_counts) {
                std::cout << threads << "T(";
                
                double total_time = 0.0;
                double total_best = 0.0;
                
                for (int run = 0; run < config.runs_per_config; run++) {
                    AcoParameters params;
                    params.num_ants = ants;
                    params.max_iterations = config.iterations;
                    params.num_threads = threads;
                    params.alpha = 1.0;
                    params.beta = 2.0;
                    params.rho = 0.1;
                    params.random_seed = 42 + run + ants + threads;
                    
                    auto start = std::chrono::high_resolution_clock::now();
                    AcoEngine engine(graph, params);
                    auto aco_results = engine.run();
                    auto end = std::chrono::high_resolution_clock::now();
                    
                    double time_ms = std::chrono::duration<double, std::milli>(end - start).count();
                    total_time += time_ms;
                    total_best += aco_results.best_tour_length;
                    
                    // Store baseline (first configuration)
                    if (threads == config.thread_counts[0] && ants == config.ant_counts[0] && run == 0) {
                        baseline_time = time_ms;
                    }
                    
                    double speedup = (baseline_time > 0) ? baseline_time / time_ms : 1.0;
                    double efficiency = speedup / threads;
                    double gap = ((aco_results.best_tour_length - config.optimal_value) / config.optimal_value) * 100.0;
                    
                    double avg_cost = aco_results.iteration_avg_lengths.empty() ? 
                        aco_results.best_tour_length : 
                        aco_results.iteration_avg_lengths.back();
                    
                    ExperimentResult result = {
                        config.instance_name, ants, threads, run + 1,
                        time_ms, aco_results.best_tour_length, avg_cost,
                        gap, speedup, efficiency
                    };
                    results.push_back(result);
                }
                
                double avg_time = total_time / config.runs_per_config;
                double avg_best = total_best / config.runs_per_config;
                double speedup = (baseline_time > 0) ? baseline_time / avg_time : 1.0;
                
                std::cout << std::fixed << std::setprecision(0) << avg_time << "ms,"
                          << std::setprecision(1) << speedup << "x) ";
            }
        }
        std::cout << std::endl;
    }
    
public:
    void runAllExperiments() {
        std::cout << "=== Fast Ant-Thread Scalability Test ===\n";
        
        #ifdef _OPENMP
        std::cout << "OpenMP: " << _OPENMP << ", Max threads: " << omp_get_max_threads() << std::endl;
        #endif
        
        std::vector<ExperimentConfig> configs = {
            {
                "eil51", 
                "d:/D_backup/2025/tum/Parallel ACO for TSP/data/eil51.tsp", 
                426,
                {16, 32, 64},  // Fewer ant counts for speed
                {1, 2, 4, 8, 16},
                25,  // Fewer iterations
                3    // Fewer runs
            },
            {
                "kroA100", 
                "d:/D_backup/2025/tum/Parallel ACO for TSP/data/kroA100.tsp", 
                21282,
                {16, 32, 64},
                {1, 2, 4, 8, 16},
                25, 3
            },
            {
                "ch150", 
                "d:/D_backup/2025/tum/Parallel ACO for TSP/data/ch150.tsp", 
                6528,
                {16, 32, 64},
                {1, 2, 4, 8, 16},
                25, 3
            },
            {
                "gr202", 
                "d:/D_backup/2025/tum/Parallel ACO for TSP/data/gr202.tsp", 
                40160,
                {16, 32, 64},
                {1, 2, 4, 8, 16},
                25, 3
            }
        };
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (const auto& config : configs) {
            runSingleExperiment(config);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
        
        saveResults();
        generateSummary();
        
        std::cout << "\n=== Experiment Completed in " << duration.count() << " seconds ===\n";
    }
    
    void saveResults() {
        std::ofstream csv_file("fast_ant_thread_results.csv");
        csv_file << "instance,num_ants,num_threads,run_id,time_ms,best_cost,avg_cost,gap_percent,speedup,efficiency\n";
        
        for (const auto& result : results) {
            csv_file << result.instance_name << ","
                    << result.num_ants << ","
                    << result.num_threads << ","
                    << result.run_id << ","
                    << std::fixed << std::setprecision(1)
                    << result.execution_time_ms << ","
                    << result.best_cost << ","
                    << result.avg_cost << ","
                    << std::setprecision(2) << result.gap_percent << ","
                    << result.speedup << ","
                    << result.efficiency << "\n";
        }
        csv_file.close();
        std::cout << "\nResults saved to: fast_ant_thread_results.csv\n";
    }
    
    void generateSummary() {
        std::ofstream report("fast_ant_thread_summary.md");
        report << "# Fast Ant-Thread Scalability Test Results\n\n";
        
        std::vector<std::string> instances = {"eil51", "kroA100", "ch150", "gr202"};
        std::vector<int> ant_counts = {16, 32, 64};
        std::vector<int> thread_counts = {1, 2, 4, 8, 16};
        
        for (const auto& instance : instances) {
            report << "## " << instance << "\n\n";
            report << "| Ants | Threads | Avg Time(ms) | Best Solution | Gap(%) | Speedup | Efficiency |\n";
            report << "|------|---------|-------------|---------------|---------|---------|------------|\n";
            
            for (int ants : ant_counts) {
                for (int threads : thread_counts) {
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
                    }
                    
                    if (count > 0) {
                        report << "| " << ants << " | " << threads << " | "
                              << std::fixed << std::setprecision(0) << (total_time / count) << " | "
                              << std::setprecision(0) << (total_best / count) << " | "
                              << std::setprecision(1) << (total_gap / count) << " | "
                              << std::setprecision(2) << (total_speedup / count) << " | "
                              << std::setprecision(3) << (total_efficiency / count) << " |\n";
                    }
                }
                report << "\n";
            }
        }
        
        report.close();
        std::cout << "Summary saved to: fast_ant_thread_summary.md\n";
    }
};

int main() {
    try {
        FastAntThreadExperiment experiment;
        experiment.runAllExperiments();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
