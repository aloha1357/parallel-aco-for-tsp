#include "aco/PerformanceMonitor.hpp"
#include <algorithm>
#include <sstream>

PerformanceMonitor::PerformanceMonitor() 
    : monitoring_active_(false), baseline_memory_mb_(0), peak_memory_mb_(0) {
    baseline_memory_mb_ = measureMemoryUsage();
}

void PerformanceMonitor::setBudget(const PerformanceBudget& budget) {
    budget_ = budget;
}

void PerformanceMonitor::startMonitoring() {
    if (monitoring_active_) {
        return; // Already monitoring
    }
    
    monitoring_active_ = true;
    start_time_ = std::chrono::high_resolution_clock::now();
    baseline_memory_mb_ = measureMemoryUsage();
    peak_memory_mb_ = baseline_memory_mb_;
    
    metrics_ = PerformanceMetrics(); // Reset metrics
    metrics_.base_memory_mb = baseline_memory_mb_;
    
    checkpoints_.clear();
    memory_checkpoints_.clear();
    
    recordCheckpoint("start");
    recordMemoryCheckpoint("baseline");
}

void PerformanceMonitor::stopMonitoring() {
    if (!monitoring_active_) {
        return;
    }
    
    stop_time_ = std::chrono::high_resolution_clock::now();
    monitoring_active_ = false;
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop_time_ - start_time_);
    metrics_.execution_time_ms = duration.count() / 1000.0;
    
    updatePeakMemory();
    metrics_.peak_memory_usage_mb = peak_memory_mb_;
    
    recordCheckpoint("stop");
    recordMemoryCheckpoint("final");
    
    // Check budget violations
    validateBudget();
}

void PerformanceMonitor::recordCheckpoint(const std::string& checkpoint_name) {
    auto now = std::chrono::high_resolution_clock::now();
    checkpoints_.emplace_back(checkpoint_name, now);
    
    if (monitoring_active_) {
        updatePeakMemory();
    }
}

size_t PerformanceMonitor::getCurrentMemoryUsage() const {
    return measureMemoryUsage();
}

void PerformanceMonitor::recordMemoryCheckpoint(const std::string& label) {
    size_t current_memory = measureMemoryUsage();
    memory_checkpoints_.emplace_back(label, current_memory);
    
    if (current_memory > peak_memory_mb_) {
        peak_memory_mb_ = current_memory;
    }
}

void PerformanceMonitor::setBaselineTime(double baseline_time_ms) {
    if (baseline_time_ms > 0.0 && metrics_.execution_time_ms > 0.0) {
        metrics_.speedup_factor = baseline_time_ms / metrics_.execution_time_ms;
    } else {
        metrics_.speedup_factor = 1.0;
    }
}

void PerformanceMonitor::calculateSpeedup(double current_time_ms, int thread_count) {
    metrics_.thread_count = thread_count;
    
    if (current_time_ms > 0.0 && thread_count > 0) {
        // If we have a baseline time, use it; otherwise assume speedup is based on current execution
        if (metrics_.speedup_factor <= 1.0) {
            // No baseline set, calculate theoretical efficiency only
            metrics_.speedup_factor = 1.0; // No actual speedup data available
        }
        
        // Calculate efficiency based on actual speedup vs theoretical maximum
        double theoretical_speedup = static_cast<double>(thread_count);
        metrics_.efficiency_percent = (metrics_.speedup_factor / theoretical_speedup) * 100.0;
        
        // Cap efficiency at 100%
        metrics_.efficiency_percent = std::min(metrics_.efficiency_percent, 100.0);
        
        // Ensure efficiency is non-negative
        metrics_.efficiency_percent = std::max(metrics_.efficiency_percent, 0.0);
    } else {
        metrics_.efficiency_percent = 100.0; // Single thread case
    }
}

PerformanceMetrics PerformanceMonitor::getMetrics() const {
    return metrics_;
}

bool PerformanceMonitor::validateBudget() const {
    bool valid = true;
    std::vector<std::string> violations;
    
    // Check time budget
    if (budget_.max_execution_time_ms > 0.0 && 
        metrics_.execution_time_ms > budget_.max_execution_time_ms) {
        valid = false;
        violations.push_back("Execution time exceeded: " + 
            std::to_string(metrics_.execution_time_ms) + "ms > " + 
            std::to_string(budget_.max_execution_time_ms) + "ms");
    }
    
    // Check memory budget
    if (budget_.max_memory_usage_mb > 0 && 
        metrics_.peak_memory_usage_mb > budget_.max_memory_usage_mb) {
        valid = false;
        violations.push_back("Memory usage exceeded: " + 
            std::to_string(metrics_.peak_memory_usage_mb) + "MB > " + 
            std::to_string(budget_.max_memory_usage_mb) + "MB");
    }
    
    // Check speedup budget
    if (budget_.min_speedup_factor > 1.0 && 
        metrics_.speedup_factor < budget_.min_speedup_factor) {
        valid = false;
        violations.push_back("Speedup insufficient: " + 
            std::to_string(metrics_.speedup_factor) + "x < " + 
            std::to_string(budget_.min_speedup_factor) + "x");
    }
    
    // Check efficiency budget
    if (budget_.min_efficiency_percent > 0.0 && 
        metrics_.efficiency_percent < budget_.min_efficiency_percent) {
        valid = false;
        violations.push_back("Efficiency insufficient: " + 
            std::to_string(metrics_.efficiency_percent) + "% < " + 
            std::to_string(budget_.min_efficiency_percent) + "%");
    }
    
    // Update metrics with violation status
    const_cast<PerformanceMetrics&>(metrics_).budget_violated = !valid;
    if (!valid && !violations.empty()) {
        std::ostringstream oss;
        for (size_t i = 0; i < violations.size(); ++i) {
            if (i > 0) oss << "; ";
            oss << violations[i];
        }
        const_cast<PerformanceMetrics&>(metrics_).violation_reason = oss.str();
    }
    
    return valid;
}

std::vector<std::string> PerformanceMonitor::getBudgetViolations() const {
    std::vector<std::string> violations;
    
    if (budget_.max_execution_time_ms > 0.0 && 
        metrics_.execution_time_ms > budget_.max_execution_time_ms) {
        violations.push_back("Execution time budget exceeded");
    }
    
    if (budget_.max_memory_usage_mb > 0 && 
        metrics_.peak_memory_usage_mb > budget_.max_memory_usage_mb) {
        violations.push_back("Memory usage budget exceeded");
    }
    
    if (budget_.min_speedup_factor > 1.0 && 
        metrics_.speedup_factor < budget_.min_speedup_factor) {
        violations.push_back("Speedup target not met");
    }
    
    if (budget_.min_efficiency_percent > 0.0 && 
        metrics_.efficiency_percent < budget_.min_efficiency_percent) {
        violations.push_back("Efficiency target not met");
    }
    
    return violations;
}

// Static utility methods
PerformanceBudget PerformanceMonitor::createTimeBudget(double max_time_ms) {
    return PerformanceBudget(max_time_ms, 0, 1.0, 0.0);
}

PerformanceBudget PerformanceMonitor::createMemoryBudget(size_t max_memory_mb) {
    return PerformanceBudget(0.0, max_memory_mb, 1.0, 0.0);
}

PerformanceBudget PerformanceMonitor::createSpeedupBudget(double min_speedup, double min_efficiency) {
    return PerformanceBudget(0.0, 0, min_speedup, min_efficiency);
}

PerformanceBudget PerformanceMonitor::createCompleteBudget(double max_time_ms, size_t max_memory_mb,
                                                          double min_speedup, double min_efficiency) {
    return PerformanceBudget(max_time_ms, max_memory_mb, min_speedup, min_efficiency);
}

// Private helper methods
size_t PerformanceMonitor::measureMemoryUsage() const {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), 
                           reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), 
                           sizeof(pmc))) {
        return pmc.WorkingSetSize / (1024 * 1024); // Convert to MB
    }
    return 0;
#else
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        // Linux: ru_maxrss is in KB, macOS: ru_maxrss is in bytes
        #ifdef __APPLE__
        return usage.ru_maxrss / (1024 * 1024); // Convert bytes to MB
        #else
        return usage.ru_maxrss / 1024; // Convert KB to MB
        #endif
    }
    return 0;
#endif
}

void PerformanceMonitor::updatePeakMemory() {
    size_t current = measureMemoryUsage();
    if (current > peak_memory_mb_) {
        peak_memory_mb_ = current;
    }
}

// RAII wrapper implementation
ScopedPerformanceMonitor::ScopedPerformanceMonitor(PerformanceMonitor& monitor)
    : monitor_(monitor) {
    monitor_.startMonitoring();
}

ScopedPerformanceMonitor::~ScopedPerformanceMonitor() {
    monitor_.stopMonitoring();
}
