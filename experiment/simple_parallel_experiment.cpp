/**
 * @file simple_parallel_experiment.cpp
 * @brief Simplified parallel ACO experiment for 4 TSP instances
 * 
 * This experiment tests 1, 2, 4, 8 threads on eil51, kroA100, ch150, gr202
 * with fixed iterations and compares against optimal solutions.
 */

#include "aco/AcoEngine.hpp"
#include "aco/Graph.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <string>
#include <iomanip>
#include <cmath>

struct TSPInstance {
    std::string name;
    std::string file_path;
    int optimal_solution;
    int size;
};

struct ExperimentResult {
    std::string instance_name;
    int thread_count;
    int round;
    double best_tour_length;
    double execution_time_ms;
    double gap_percentage;
    double speedup;
};

class SimpleParallelExperiment {
private:
    std::vector<TSPInstance> instances_;
    std::vector<int> thread_counts_;
    int rounds_per_config_;
    std::vector<ExperimentResult> results_;
    
public:
    SimpleParallelExperiment() {
        instances_ = {
            {"eil51", "data/eil51.tsp", 426, 51},
            {"kroA100", "data/kroA100.tsp", 21282, 100}, 
            {"ch150", "data/ch150.tsp", 6528, 150},
            {"gr202", "data/gr202.tsp", 40160, 202}
        };
        
        thread_counts_ = {1, 2, 4, 8};
        rounds_per_config_ = 10;  // Start with smaller number for testing
    }
    
    void run() {
        std::cout << "=== Simple Parallel ACO Experiment ===" << std::endl;
        std::cout << "Testing " << instances_.size() << " instances with " 
                  << thread_counts_.size() << " thread configurations" << std::endl;
        std::cout << rounds_per_config_ << " rounds per configuration" << std::endl;
        
        for (const auto& instance : instances_) {
            std::cout << "\n--- Testing " << instance.name 
                      << " (optimal: " << instance.optimal_solution << ") ---" << std::endl;
            
            auto graph = Graph::fromTSPFile(instance.file_path);
            if (!graph) {
                std::cerr << "Error: Could not load " << instance.file_path << std::endl;
                continue;
            }
            
            std::cout << "Loaded graph with " << graph->size() << " cities" << std::endl;
            
            // Store baseline time for speedup calculation
            double baseline_time = 0.0;
            
            for (int threads : thread_counts_) {
                std::cout << "  Testing " << threads << " threads: ";
                
                double total_time = 0.0;
                double total_gap = 0.0;
                double best_found = std::numeric_limits<double>::max();
                
                for (int round = 0; round < rounds_per_config_; ++round) {
                    AcoParameters params;
                    params.num_threads = threads;
                    params.max_iterations = 100;  // Fixed iterations
                    params.num_ants = 50;
                    params.alpha = 1.0;
                    params.beta = 2.0;
                    params.rho = 0.5;
                    params.random_seed = 42 + round * 1000 + threads * 100;
                    
                    AcoEngine engine(graph, params);
                    
                    auto start = std::chrono::high_resolution_clock::now();
                    auto aco_results = engine.run();
                    auto end = std::chrono::high_resolution_clock::now();
                    
                    double execution_time = std::chrono::duration<double, std::milli>(end - start).count();
                    double gap = ((aco_results.best_tour_length - instance.optimal_solution) / 
                                 instance.optimal_solution) * 100.0;
                    
                    total_time += execution_time;
                    total_gap += gap;
                    best_found = std::min(best_found, aco_results.best_tour_length);
                    
                    // Store individual result
                    ExperimentResult result;
                    result.instance_name = instance.name;
                    result.thread_count = threads;
                    result.round = round + 1;
                    result.best_tour_length = aco_results.best_tour_length;
                    result.execution_time_ms = execution_time;
                    result.gap_percentage = gap;
                    result.speedup = 0.0;  // Will calculate after baseline
                    
                    results_.push_back(result);
                }
                
                double avg_time = total_time / rounds_per_config_;
                double avg_gap = total_gap / rounds_per_config_;
                
                // Store baseline for speedup calculation
                if (threads == 1) {
                    baseline_time = avg_time;
                }
                
                double speedup = (baseline_time > 0) ? baseline_time / avg_time : 1.0;
                
                std::cout << std::fixed << std::setprecision(1) 
                          << "Avg " << avg_time << "ms, " 
                          << "Gap " << avg_gap << "%, "
                          << "Best " << best_found << ", "
                          << "Speedup " << speedup << "x" << std::endl;
            }
        }
        
        exportResultsToCSV();
        printSummary();
    }
    
private:
    void exportResultsToCSV() {
        std::string filename = "simple_experiment_results.csv";
        std::ofstream file(filename);
        
        file << "instance,threads,round,best_length,time_ms,gap_percent" << std::endl;
        
        for (const auto& result : results_) {
            file << result.instance_name << ","
                 << result.thread_count << ","
                 << result.round << ","
                 << std::fixed << std::setprecision(2) << result.best_tour_length << ","
                 << result.execution_time_ms << ","
                 << result.gap_percentage << std::endl;
        }
        
        std::cout << "\nResults exported to: " << filename << std::endl;
    }
    
    void printSummary() {
        std::cout << "\n=== EXPERIMENT SUMMARY ===" << std::endl;
        std::cout << "Instance\tThreads\tAvg_Gap%\tAvg_Time(ms)\tSpeedup" << std::endl;
        std::cout << "--------\t-------\t--------\t------------\t-------" << std::endl;
        
        for (const auto& instance : instances_) {
            double baseline_time = 0.0;
            
            // First pass: get baseline time
            for (const auto& result : results_) {
                if (result.instance_name == instance.name && result.thread_count == 1) {
                    baseline_time += result.execution_time_ms;
                }
            }
            baseline_time /= rounds_per_config_;
            
            // Second pass: calculate and display averages
            for (int threads : thread_counts_) {
                double sum_gap = 0.0, sum_time = 0.0;
                int count = 0;
                
                for (const auto& result : results_) {
                    if (result.instance_name == instance.name && result.thread_count == threads) {
                        sum_gap += result.gap_percentage;
                        sum_time += result.execution_time_ms;
                        count++;
                    }
                }
                
                if (count > 0) {
                    double avg_gap = sum_gap / count;
                    double avg_time = sum_time / count;
                    double speedup = baseline_time / avg_time;
                    
                    std::cout << instance.name << "\t" << threads << "\t"
                              << std::fixed << std::setprecision(2) << avg_gap << "\t"
                              << avg_time << "\t\t" << speedup << std::endl;
                }
            }
        }
    }
};

int main() {
    try {
        SimpleParallelExperiment experiment;
        experiment.run();
        std::cout << "\n✅ Experiment completed successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "❌ Experiment failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
