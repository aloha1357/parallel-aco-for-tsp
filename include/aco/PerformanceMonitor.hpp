#pragma once

#include <chrono>
#include <vector>
#include <string>
#include <memory>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <sys/resource.h>
#include <unistd.h>
#endif

/**
 * @brief Structure to hold performance budget constraints
 */
struct PerformanceBudget {
    double max_execution_time_ms = 0.0;  // Maximum allowed execution time
    size_t max_memory_usage_mb = 0;      // Maximum allowed memory usage in MB
    double min_speedup_factor = 1.0;     // Minimum speedup compared to single-thread
    double min_efficiency_percent = 50.0; // Minimum parallel efficiency percentage
    
    PerformanceBudget() = default;
    
    PerformanceBudget(double max_time_ms, size_t max_memory_mb, 
                     double min_speedup = 1.0, double min_efficiency = 50.0)
        : max_execution_time_ms(max_time_ms), max_memory_usage_mb(max_memory_mb),
          min_speedup_factor(min_speedup), min_efficiency_percent(min_efficiency) {}
};

/**
 * @brief Performance metrics collected during execution
 */
struct PerformanceMetrics {
    double execution_time_ms = 0.0;      // Actual execution time
    size_t peak_memory_usage_mb = 0;     // Peak memory usage in MB
    double speedup_factor = 1.0;         // Speedup compared to baseline
    double efficiency_percent = 100.0;   // Parallel efficiency
    int thread_count = 1;                // Number of threads used
    bool budget_violated = false;        // Whether performance budget was exceeded
    std::string violation_reason = "";   // Reason for budget violation
    
    // Detailed timing breakdown
    double initialization_time_ms = 0.0;
    double computation_time_ms = 0.0;
    double synchronization_time_ms = 0.0;
    
    // Memory breakdown
    size_t base_memory_mb = 0;           // Base memory before algorithm
    size_t pheromone_memory_mb = 0;      // Memory for pheromone matrices
    size_t thread_overhead_mb = 0;       // Additional memory for threading
};

/**
 * @brief High-precision performance monitoring utility
 */
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor() = default;

    // Budget management
    void setBudget(const PerformanceBudget& budget);
    const PerformanceBudget& getBudget() const { return budget_; }
    
    // Monitoring lifecycle
    void startMonitoring();
    void stopMonitoring();
    void recordCheckpoint(const std::string& checkpoint_name);
    
    // Memory monitoring
    size_t getCurrentMemoryUsage() const;
    size_t getBaselineMemoryUsage() const { return baseline_memory_mb_; }
    void recordMemoryCheckpoint(const std::string& label);
    
    // Speedup and efficiency calculation
    void setBaselineTime(double baseline_time_ms);
    void calculateSpeedup(double current_time_ms, int thread_count);
    
    // Results and validation
    PerformanceMetrics getMetrics() const;
    bool validateBudget() const;
    std::vector<std::string> getBudgetViolations() const;
    
    // Utility functions
    static PerformanceBudget createTimeBudget(double max_time_ms);
    static PerformanceBudget createMemoryBudget(size_t max_memory_mb);
    static PerformanceBudget createSpeedupBudget(double min_speedup, double min_efficiency);
    static PerformanceBudget createCompleteBudget(double max_time_ms, size_t max_memory_mb,
                                                 double min_speedup, double min_efficiency);

private:
    PerformanceBudget budget_;
    PerformanceMetrics metrics_;
    
    std::chrono::high_resolution_clock::time_point start_time_;
    std::chrono::high_resolution_clock::time_point stop_time_;
    
    size_t baseline_memory_mb_;
    size_t peak_memory_mb_;
    
    bool monitoring_active_;
    
    // Platform-specific memory measurement
    size_t measureMemoryUsage() const;
    void updatePeakMemory();
    
    // Checkpoint tracking
    std::vector<std::pair<std::string, std::chrono::high_resolution_clock::time_point>> checkpoints_;
    std::vector<std::pair<std::string, size_t>> memory_checkpoints_;
};

/**
 * @brief RAII wrapper for automatic performance monitoring
 */
class ScopedPerformanceMonitor {
public:
    explicit ScopedPerformanceMonitor(PerformanceMonitor& monitor);
    ~ScopedPerformanceMonitor();
    
    ScopedPerformanceMonitor(const ScopedPerformanceMonitor&) = delete;
    ScopedPerformanceMonitor& operator=(const ScopedPerformanceMonitor&) = delete;

private:
    PerformanceMonitor& monitor_;
};
