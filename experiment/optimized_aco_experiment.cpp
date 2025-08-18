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
};

class OptimizedACOExperiment {
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
    
    void saveResults(const std::vector<ExperimentResult>& results, const std::string& experiment_type) {
        std::string filename = output_dir + "/" + experiment_type + "_results_" + timestamp + ".csv";
        std::ofstream file(filename);
        
        // CSV Header
        file << "instance_name,threads,iterations,round,tour_length,ratio_to_best,execution_time_ms,completed_iterations\n";
        
        // Data rows
        for (const auto& result : results) {
            file << result.instance_name << ","
                 << result.threads << ","
                 << result.iterations << ","
                 << result.round << ","
                 << std::fixed << std::setprecision(2) << result.tour_length << ","
                 << std::fixed << std::setprecision(4) << result.ratio_to_best << ","
                 << std::fixed << std::setprecision(2) << result.execution_time_ms << ","
                 << result.completed_iterations << "\n";
        }
        
        file.close();
    }
    
    ExperimentResult runSingleExperiment(const ExperimentConfig& config, int threads, int max_iterations, int round) {
        // Redirect cout to null to suppress ACO engine output
        std::streambuf* orig = std::cout.rdbuf();
        std::cout.rdbuf(nullptr);
        
        try {
            // Load graph using the correct static method
            auto graph = Graph::fromTSPFile(config.tsp_file);
            
            // Set up ACO parameters
            AcoParameters params;
            params.num_ants = 10;
            params.max_iterations = max_iterations;
            params.num_threads = threads;
            params.random_seed = 42 + round; // Different seed for each round
            
            // Create ACO engine
            AcoEngine engine(graph, params);
            
            // Run experiment
            auto start_time = std::chrono::high_resolution_clock::now();
            auto result = engine.run();
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            
            // Restore cout
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
            
            return exp_result;
        } catch (...) {
            // Restore cout in case of exception
            std::cout.rdbuf(orig);
            throw;
        }
    }
    
    ExperimentResult runTimeBudgetExperiment(const ExperimentConfig& config, int threads, double time_budget_ms, int round) {
        // Redirect cout to null to suppress ACO engine output
        std::streambuf* orig = std::cout.rdbuf();
        std::cout.rdbuf(nullptr);
        
        try {
            // Load graph using the correct static method
            auto graph = Graph::fromTSPFile(config.tsp_file);
            
            // Set up ACO parameters with a high iteration limit (will be stopped by time)
            AcoParameters params;
            params.num_ants = 10;
            params.max_iterations = 10000; // High limit, will be stopped by time
            params.num_threads = threads;
            params.random_seed = 42 + round;
            
            // Create ACO engine
            AcoEngine engine(graph, params);
            
            // Create performance budget for time limit
            PerformanceBudget budget;
            budget.max_execution_time_ms = time_budget_ms;
            budget.max_memory_usage_mb = 1000;
            budget.min_speedup_factor = 1.0;
            budget.min_efficiency_percent = 50.0;
            
            // Run with time budget
            auto start_time = std::chrono::high_resolution_clock::now();
            auto result = engine.runWithBudget(budget);
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            
            // Restore cout
            std::cout.rdbuf(orig);
            
            ExperimentResult exp_result;
            exp_result.instance_name = config.instance_name;
            exp_result.threads = threads;
            exp_result.iterations = -1; // Time budget experiment
            exp_result.round = round;
            exp_result.tour_length = result.best_tour_length;
            exp_result.ratio_to_best = result.best_tour_length / config.best_known_length;
            exp_result.execution_time_ms = duration.count();
            exp_result.completed_iterations = result.actual_iterations;
            
            return exp_result;
        } catch (...) {
            // Restore cout in case of exception
            std::cout.rdbuf(orig);
            throw;
        }
    }
    
    void showProgress(const std::string& phase, int current, int total, const std::string& details = "") {
        int percent = (current * 100) / total;
        std::cout << "\r[" << phase << "] " << percent << "% (" << current << "/" << total << ") " << details << std::flush;
        if (current == total) std::cout << std::endl;
    }
    
public:
    OptimizedACOExperiment() {
        initializeTestInstances();
        timestamp = generateTimestamp();
        output_dir = "experiment/results";
        
        // Create output directory if it doesn't exist
        system(("mkdir " + output_dir + " 2>nul").c_str());
    }
    
    void runFixedIterationExperiment() {
        std::vector<ExperimentResult> results;
        std::vector<int> thread_counts = {1, 2, 4, 8};
        std::vector<int> iteration_counts = {2, 100};
        int rounds = 100;
        
        std::cout << "=== Fixed Iteration Experiment ===" << std::endl;
        std::cout << "Instances: " << test_instances.size() << ", Iterations: " << iteration_counts.size() 
                  << ", Threads: " << thread_counts.size() << ", Rounds: " << rounds << std::endl;
        
        int total_runs = test_instances.size() * iteration_counts.size() * thread_counts.size() * rounds;
        int current_run = 0;
        
        for (const auto& config : test_instances) {
            for (int iterations : iteration_counts) {
                for (int threads : thread_counts) {
                    for (int round = 1; round <= rounds; round++) {
                        current_run++;
                        
                        try {
                            auto result = runSingleExperiment(config, threads, iterations, round);
                            results.push_back(result);
                            
                            std::string details = config.instance_name + " i=" + std::to_string(iterations) + 
                                                " t=" + std::to_string(threads) + " r=" + std::to_string(round);
                            showProgress("Fixed", current_run, total_runs, details);
                            
                        } catch (const std::exception& e) {
                            std::cerr << "\nError: " << e.what() << std::endl;
                        }
                    }
                }
            }
        }
        
        saveResults(results, "fixed_iteration");
        std::cout << "Fixed iteration experiment completed! Results saved." << std::endl;
    }
    
    void runTimeBudgetExperiment() {
        std::vector<ExperimentResult> results;
        std::vector<int> thread_counts = {1, 2, 4, 8};
        int rounds = 100;
        
        std::cout << "\n=== Time Budget Experiment ===" << std::endl;
        
        // First, measure 1-thread 100-iteration time for each instance (base timing)
        std::vector<double> base_times;
        std::cout << "Measuring base times..." << std::endl;
        
        for (const auto& config : test_instances) {
            auto result = runSingleExperiment(config, 1, 100, 1);
            base_times.push_back(result.execution_time_ms);
            std::cout << config.instance_name << ": " << (int)result.execution_time_ms << "ms" << std::endl;
        }
        
        int total_runs = test_instances.size() * thread_counts.size() * rounds;
        int current_run = 0;
        
        // Run time budget experiments
        for (size_t i = 0; i < test_instances.size(); i++) {
            const auto& config = test_instances[i];
            double time_budget = base_times[i];
            
            for (int threads : thread_counts) {
                for (int round = 1; round <= rounds; round++) {
                    current_run++;
                    
                    try {
                        auto result = runTimeBudgetExperiment(config, threads, time_budget, round);
                        results.push_back(result);
                        
                        std::string details = config.instance_name + " b=" + std::to_string((int)time_budget) + 
                                            "ms t=" + std::to_string(threads) + " r=" + std::to_string(round);
                        showProgress("Budget", current_run, total_runs, details);
                        
                    } catch (const std::exception& e) {
                        std::cerr << "\nError: " << e.what() << std::endl;
                    }
                }
            }
        }
        
        saveResults(results, "time_budget");
        std::cout << "Time budget experiment completed! Results saved." << std::endl;
    }
};

int main() {
    try {
        OptimizedACOExperiment experiment;
        
        std::cout << "=== Optimized ACO Performance Experiment ===" << std::endl;
        std::cout << "Instances: eil51, kroA100, ch150, gr202" << std::endl;
        std::cout << "Thread configurations: 1, 2, 4, 8" << std::endl;
        std::cout << "Rounds per configuration: 100" << std::endl;
        std::cout << "Output optimized for performance." << std::endl << std::endl;
        
        // Run both experiments
        experiment.runFixedIterationExperiment();
        experiment.runTimeBudgetExperiment();
        
        std::cout << "\n=== All experiments completed successfully! ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Experiment failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
