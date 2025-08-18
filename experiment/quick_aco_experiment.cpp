#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <limits>
#include <omp.h>
#include "../include/aco/Graph.hpp"
#include "../include/aco/AcoEngine.hpp"
#include "../include/aco/PerformanceMonitor.hpp"

struct ExperimentConfig {
    std::string instance_name;
    std::string tsp_file;
    double best_known_length;
    int num_cities;
};

struct ExperimentResult {
    std::string instance_name;
    int threads;
    int iterations;
    int round;
    double tour_length;
    double ratio_to_best;
    double execution_time_ms;
    int completed_iterations;
    double speedup;
    double efficiency;
};

class QuickACOExperiment {
private:
    std::vector<ExperimentConfig> test_instances;
    std::string output_dir;
    std::string timestamp;
    
    void initializeTestInstances() {
        test_instances = {
            {"eil51", "data/eil51.tsp", 426.0, 51},
            {"kroA100", "data/kroA100.tsp", 21282.0, 100}, 
            {"ch150", "data/ch150.tsp", 6528.0, 150},
            {"gr202", "data/gr202.tsp", 40160.0, 202}
        };
    }
    
    std::string generateTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
        return ss.str();
    }
    
    void saveResults(const std::vector<ExperimentResult>& results) {
        std::string filename = output_dir + "/quick_experiment_results_" + timestamp + ".csv";
        std::ofstream file(filename);
        
        // CSV Header
        file << "instance_name,threads,iterations,round,tour_length,ratio_to_best,execution_time_ms,completed_iterations,speedup,efficiency\n";
        
        // Data rows
        for (const auto& result : results) {
            file << result.instance_name << ","
                 << result.threads << ","
                 << result.iterations << ","
                 << result.round << ","
                 << std::fixed << std::setprecision(2) << result.tour_length << ","
                 << std::fixed << std::setprecision(4) << result.ratio_to_best << ","
                 << std::fixed << std::setprecision(2) << result.execution_time_ms << ","
                 << result.completed_iterations << ","
                 << std::fixed << std::setprecision(2) << result.speedup << ","
                 << std::fixed << std::setprecision(2) << result.efficiency << "\n";
        }
        
        file.close();
        std::cout << "Results saved to: " << filename << std::endl;
    }
    
    ExperimentResult runSingleExperiment(const ExperimentConfig& config, int threads, int max_iterations, int round) {
        // Suppress ACO engine output
        std::streambuf* orig = std::cout.rdbuf();
        std::cout.rdbuf(nullptr);
        
        try {
            auto graph = Graph::fromTSPFile(config.tsp_file);
            
            AcoParameters params;
            params.num_ants = std::min(10, config.num_cities / 5); // Adaptive ant count
            params.max_iterations = max_iterations;
            params.num_threads = threads;
            params.random_seed = 42 + round;
            
            AcoEngine engine(graph, params);
            
            auto start_time = std::chrono::high_resolution_clock::now();
            auto result = engine.run();
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            
            std::cout.rdbuf(orig);
            
            ExperimentResult exp_result;
            exp_result.instance_name = config.instance_name;
            exp_result.threads = threads;
            exp_result.iterations = max_iterations;
            exp_result.round = round;
            exp_result.tour_length = result.best_tour_length;
            exp_result.ratio_to_best = result.best_tour_length / config.best_known_length;
            exp_result.execution_time_ms = duration.count();
            exp_result.completed_iterations = result.actual_iterations;
            exp_result.speedup = 0.0; // Will be calculated later
            exp_result.efficiency = 0.0; // Will be calculated later
            
            return exp_result;
        } catch (...) {
            std::cout.rdbuf(orig);
            throw;
        }
    }
    
    void calculateSpeedupAndEfficiency(std::vector<ExperimentResult>& results) {
        // Group results by instance and iteration count
        std::map<std::string, double> baseline_times;
        
        // Find baseline times (1 thread)
        for (const auto& result : results) {
            if (result.threads == 1) {
                std::string key = result.instance_name + "_" + std::to_string(result.iterations);
                if (baseline_times.find(key) == baseline_times.end()) {
                    baseline_times[key] = result.execution_time_ms;
                } else {
                    baseline_times[key] = std::min(baseline_times[key], result.execution_time_ms);
                }
            }
        }
        
        // Calculate speedup and efficiency
        for (auto& result : results) {
            std::string key = result.instance_name + "_" + std::to_string(result.iterations);
            if (baseline_times.find(key) != baseline_times.end() && result.execution_time_ms > 0) {
                result.speedup = baseline_times[key] / result.execution_time_ms;
                result.efficiency = result.speedup / result.threads * 100.0;
            }
        }
    }
    
public:
    QuickACOExperiment() {
        initializeTestInstances();
        timestamp = generateTimestamp();
        output_dir = "experiment/results";
        system(("mkdir " + output_dir + " 2>nul").c_str());
    }
    
    void runQuickScalabilityTest() {
        std::vector<ExperimentResult> results;
        std::vector<int> thread_counts = {1, 2, 4, 8};
        int iterations = 50; // 中等迭代數
        int rounds = 5; // 只跑5輪，足夠統計
        
        std::cout << "=== Quick ACO Scalability Test ===" << std::endl;
        std::cout << "Instances: " << test_instances.size() 
                  << ", Threads: " << thread_counts.size() 
                  << ", Rounds: " << rounds 
                  << ", Iterations: " << iterations << std::endl;
        
        int total_runs = test_instances.size() * thread_counts.size() * rounds;
        int current_run = 0;
        
        for (const auto& config : test_instances) {
            std::cout << "\nTesting " << config.instance_name << " (" << config.num_cities << " cities)..." << std::endl;
            
            for (int threads : thread_counts) {
                for (int round = 1; round <= rounds; round++) {
                    current_run++;
                    
                    try {
                        auto result = runSingleExperiment(config, threads, iterations, round);
                        results.push_back(result);
                        
                        // Show progress with basic stats
                        int percent = (current_run * 100) / total_runs;
                        std::cout << "\r[" << percent << "%] " 
                                  << config.instance_name << " t=" << threads 
                                  << " r=" << round << "/5 "
                                  << "(" << (int)result.execution_time_ms << "ms, "
                                  << "ratio=" << std::fixed << std::setprecision(2) << result.ratio_to_best << ")" 
                                  << std::flush;
                        
                    } catch (const std::exception& e) {
                        std::cerr << "\nError: " << e.what() << std::endl;
                    }
                }
                std::cout << std::endl;
            }
        }
        
        // Calculate speedup and efficiency
        calculateSpeedupAndEfficiency(results);
        
        // Save results
        saveResults(results);
        
        // Print summary
        printSummary(results);
        
        std::cout << "\nQuick scalability test completed!" << std::endl;
    }
    
    void printSummary(const std::vector<ExperimentResult>& results) {
        std::cout << "\n=== Summary (Average Results) ===" << std::endl;
        std::cout << std::setw(10) << "Instance" << std::setw(8) << "Threads" 
                  << std::setw(10) << "Time(ms)" << std::setw(8) << "Ratio" 
                  << std::setw(8) << "Speedup" << std::setw(10) << "Efficiency" << std::endl;
        std::cout << std::string(60, '-') << std::endl;
        
        // Group and average results
        std::map<std::string, std::vector<ExperimentResult>> grouped;
        for (const auto& result : results) {
            std::string key = result.instance_name + "_" + std::to_string(result.threads);
            grouped[key].push_back(result);
        }
        
        for (const auto& config : test_instances) {
            for (int threads : {1, 2, 4, 8}) {
                std::string key = config.instance_name + "_" + std::to_string(threads);
                if (grouped.find(key) != grouped.end()) {
                    auto& group = grouped[key];
                    
                    double avg_time = 0, avg_ratio = 0, avg_speedup = 0, avg_efficiency = 0;
                    for (const auto& r : group) {
                        avg_time += r.execution_time_ms;
                        avg_ratio += r.ratio_to_best;
                        avg_speedup += r.speedup;
                        avg_efficiency += r.efficiency;
                    }
                    int count = group.size();
                    avg_time /= count;
                    avg_ratio /= count;
                    avg_speedup /= count;
                    avg_efficiency /= count;
                    
                    std::cout << std::setw(10) << config.instance_name 
                              << std::setw(8) << threads
                              << std::setw(10) << std::fixed << std::setprecision(0) << avg_time
                              << std::setw(8) << std::fixed << std::setprecision(2) << avg_ratio
                              << std::setw(8) << std::fixed << std::setprecision(1) << avg_speedup
                              << std::setw(10) << std::fixed << std::setprecision(1) << avg_efficiency << "%" << std::endl;
                }
            }
            std::cout << std::endl;
        }
    }
};

int main() {
    try {
        QuickACOExperiment experiment;
        
        std::cout << "=== Quick ACO Performance Experiment ===" << std::endl;
        std::cout << "Fast scalability test with reduced parameters" << std::endl;
        std::cout << "Estimated time: ~2-5 minutes" << std::endl << std::endl;
        
        experiment.runQuickScalabilityTest();
        
        std::cout << "\n=== Experiment completed successfully! ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Experiment failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
