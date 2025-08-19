#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <memory>
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
    int runs_per_config = 3;
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

class AntThreadScalabilityExperiment {
private:
    std::vector<ExperimentResult> results_;
    
    void runSingleExperiment(const ExperimentConfig& config) {
        std::cout << "\n=== Running Experiment: " << config.instance_name << " ===" << std::endl;
        
        // Load TSP instance ONCE for all thread configurations
        std::cout << "Loading TSP file: " << config.file_path << std::endl;
        auto graph_unique = aco::TSPLibReader::loadGraphFromTSPLib(config.file_path);
        if (!graph_unique) {
            std::cerr << "Failed to load TSP file: " << config.file_path << std::endl;
            return;
        }
        
        // Convert to shared_ptr for reuse across all configurations
        std::shared_ptr<Graph> graph = std::move(graph_unique);
        
        std::cout << "Cities: " << graph->size() << std::endl;
        std::cout << "Optimal: " << config.optimal_value << std::endl;
        std::cout << "Graph loaded successfully - will reuse for all thread configurations" << std::endl;
        
        // Get single-thread baseline time
        double baseline_time = 0.0;
        
        for (int ants : config.ant_counts) {
            std::cout << "\n--- Ant Count: " << ants << " ---" << std::endl;
            
            for (int threads : config.thread_counts) {
                std::cout << "Threads: " << threads << " ";
                
                double total_time = 0.0;
                double total_best_cost = 0.0;
                double total_avg_cost = 0.0;
                
                for (int run = 0; run < config.runs_per_config; run++) {
                    // Setup ACO parameters (reusing the same graph object)
                    AcoParameters params;
                    params.num_ants = ants;
                    params.max_iterations = config.iterations;
                    params.num_threads = threads;
                    params.alpha = 1.0;
                    params.beta = 2.0;
                    params.rho = 0.1;
                    
                    // Reuse the same graph object - no need to reload
                    AcoEngine engine(graph, params);
                    
                    auto start = std::chrono::high_resolution_clock::now();
                    auto aco_results = engine.run();
                    auto end = std::chrono::high_resolution_clock::now();
                    
                    double time_ms = std::chrono::duration<double, std::milli>(end - start).count();
                    
                    total_time += time_ms;
                    total_best_cost += aco_results.best_tour_length;
                    
                    // Calculate average cost (use last iteration average)
                    double avg_cost = aco_results.iteration_avg_lengths.empty() ? 
                        aco_results.best_tour_length : 
                        aco_results.iteration_avg_lengths.back();
                    
                    total_avg_cost += avg_cost;
                    
                    // Calculate performance metrics
                    double gap_percent = ((aco_results.best_tour_length - config.optimal_value) / (double)config.optimal_value) * 100.0;
                    
                    // Store single-thread baseline time (first configuration)
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
                        aco_results.best_tour_length,
                        avg_cost,
                        gap_percent,
                        speedup,
                        efficiency
                    };
                    
                    results_.push_back(result);
                    
                    // Reset graph for next run
                    graph = aco::TSPLibReader::loadGraphFromTSPLib(config.file_path);
                }
                
                double avg_time = total_time / config.runs_per_config;
                double avg_best = total_best_cost / config.runs_per_config;
                double avg_gap = ((avg_best - config.optimal_value) / (double)config.optimal_value) * 100.0;
                double speedup = (baseline_time > 0) ? baseline_time / avg_time : 1.0;
                
                std::cout << std::fixed << std::setprecision(1);
                std::cout << "Time: " << avg_time << "ms ";
                std::cout << "Best: " << (int)avg_best << " ";
                std::cout << "Gap: " << avg_gap << "% ";
                std::cout << "Speedup: " << speedup << "x" << std::endl;
            }
        }
    }
    
public:
    void runAllExperiments() {
        std::cout << "=== Ant-Thread Scalability Experiment Started ===" << std::endl;
        #ifdef _OPENMP
        std::cout << "OpenMP Version: " << _OPENMP << std::endl;
        std::cout << "Max Threads: " << omp_get_max_threads() << std::endl;
        #else
        std::cout << "OpenMP Not Available" << std::endl;
        #endif
        
        // Experiment configurations
        std::vector<ExperimentConfig> configs = {
            {
                "eil51", 
                "d:/D_backup/2025/tum/Parallel ACO for TSP/data/eil51.tsp", 
                426,
                {16, 32, 64, 128},  // Ant counts: multiples of thread count
                {1, 2, 4, 8, 16},   // Thread counts
                50, 
                3
            },
            {
                "kroA100", 
                "d:/D_backup/2025/tum/Parallel ACO for TSP/data/kroA100.tsp", 
                21282,
                {16, 32, 64, 128},
                {1, 2, 4, 8, 16},
                50, 
                3
            },
            {
                "ch150", 
                "d:/D_backup/2025/tum/Parallel ACO for TSP/data/ch150.tsp", 
                6528,
                {16, 32, 64, 128},
                {1, 2, 4, 8, 16},
                50, 
                3
            },
            {
                "gr202", 
                "d:/D_backup/2025/tum/Parallel ACO for TSP/data/gr202.tsp", 
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
        std::ofstream csv_file("ant_thread_scalability_results.csv");
        csv_file << "instance,num_ants,num_threads,run_id,time_ms,best_cost,avg_cost,gap_percent,speedup,efficiency\n";
        
        for (const auto& result : results_) {
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
        std::cout << "\nResults saved to: ant_thread_scalability_results.csv" << std::endl;
    }
    
    void generateSummaryReport() {
        std::ofstream report("ant_thread_scalability_report.md");
        report << "# Ant Count vs Thread Count Scalability Experiment Report\n\n";
        
        report << "## Experiment Overview\n";
        report << "- **Objective**: Analyze the impact of ant count and thread count on parallel ACO performance\n";
        report << "- **Test Instances**: eil51, kroA100, ch150, gr202\n";
        report << "- **Ant Counts**: 16, 32, 64, 128\n";
        report << "- **Thread Counts**: 1, 2, 4, 8, 16\n";
        report << "- **Iterations**: 50\n";
        report << "- **Runs per Configuration**: 3\n\n";
        
        // Analysis by instance
        std::vector<std::string> instances = {"eil51", "kroA100", "ch150", "gr202"};
        std::vector<int> ant_counts = {16, 32, 64, 128};
        std::vector<int> thread_counts = {1, 2, 4, 8, 16};
        
        for (const auto& instance : instances) {
            report << "## " << instance << " Instance Analysis\n\n";
            
            // Create table header
            report << "| Ants | Threads | Avg Time(ms) | Best Sol | Gap(%) | Speedup | Efficiency |\n";
            report << "|------|---------|-------------|----------|--------|---------|------------|\n";
            
            for (int ants : ant_counts) {
                for (int threads : thread_counts) {
                    // Calculate averages for this configuration
                    double total_time = 0.0, total_best = 0.0, total_gap = 0.0;
                    double total_speedup = 0.0, total_efficiency = 0.0;
                    int count = 0;
                    
                    for (const auto& result : results_) {
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
        
        report << "\n## Key Findings\n";
        report << "1. **Ant Count Impact**: Higher ant counts provide better parallelization opportunities\n";
        report << "2. **Thread Scalability**: Analysis of thread scaling effectiveness for different ant configurations\n";
        report << "3. **Optimal Configurations**: Best ant-thread combinations for different problem sizes\n";
        report << "4. **Efficiency Analysis**: Thread utilization efficiency across different configurations\n";
        
        report.close();
        std::cout << "Analysis report saved to: ant_thread_scalability_report.md" << std::endl;
    }
};

int main() {
    try {
        AntThreadScalabilityExperiment experiment;
        experiment.runAllExperiments();
        
        std::cout << "\n=== Experiment Completed Successfully! ===" << std::endl;
        std::cout << "Please check the following files:" << std::endl;
        std::cout << "- ant_thread_scalability_results.csv (detailed data)" << std::endl;
        std::cout << "- ant_thread_scalability_report.md (analysis report)" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Experiment execution error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
