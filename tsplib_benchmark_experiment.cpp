#include "aco/BenchmarkAnalyzer.hpp"
#include "aco/TSPLibReader.hpp"
#include "aco/ExperimentRunner.hpp"
#include <iostream>
#include <vector>
#include <string>

using namespace aco;

int main() {
    std::cout << "=== 平行化 ACO TSP 標準實驗 ===" << std::endl;
    std::cout << "按照學術標準進行嚴格的性能與質量評估" << std::endl;
    std::cout << std::endl;
    
    // 實驗配置
    ExperimentConfig config;
    config.runs_per_configuration = 5;  // 每個配置運行5次取平均
    config.thread_counts = {1, 2, 4, 8}; // 執行緒數量
    config.output_directory = "experiment_results";
    
    // TSPLIB 標準測試集
    std::vector<TSPBenchmarkInstance> benchmark_instances = {
        {"berlin52", "data/berlin52.tsp", 7542},
        {"kroA100", "data/kroA100.tsp", 21282}, 
        {"kroA150", "data/kroA150.tsp", 26524}
    };
    
    ExperimentRunner runner(config);
    
    std::cout << "開始實驗..." << std::endl;
    std::cout << "測試實例: " << benchmark_instances.size() << " 個" << std::endl;
    std::cout << "執行緒配置: " << config.thread_counts.size() << " 種" << std::endl;
    std::cout << "每配置運行: " << config.runs_per_configuration << " 次" << std::endl;
    std::cout << "總實驗次數: " << benchmark_instances.size() * config.thread_counts.size() * config.runs_per_configuration << std::endl;
    std::cout << std::endl;
    
    // 運行完整實驗
    auto results = runner.runCompleteExperiment(benchmark_instances);
    
    // 導出結果
    runner.exportResults(results);
    
    // 生成分析報告
    runner.generateAnalysisReport(results);
    
    // 生成圖表
    runner.generateVisualization(results);
    
    std::cout << std::endl;
    std::cout << "=== 實驗完成 ===" << std::endl;
    std::cout << "結果檔案位於: " << config.output_directory << "/" << std::endl;
    
    return 0;
}
