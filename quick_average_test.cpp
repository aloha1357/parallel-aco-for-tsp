#include "aco/BenchmarkAnalyzer.hpp"
#include "aco/TSPLibReader.hpp"
#include <iostream>
#include <vector>

using namespace aco;

int main() {
    std::cout << "=== 快速平均測試驗證 ===" << std::endl;
    std::cout << "快速驗證平均測試功能（減少重複次數）" << std::endl;
    std::cout << std::endl;

    BenchmarkAnalyzer analyzer;
    
    // 選擇一個小的測試問題進行快速驗證
    std::vector<TSPBenchmark> test_benchmarks = {
        {"berlin52", "../data/berlin52.tsp", 7542, 52}  // 只測試一個中等規模問題，使用相對路徑
    };
    
    // 配置測試參數 - 減少重複次數用於快速測試
    int runs_per_configuration = 3;  // 快速測試只運行3次
    
    std::cout << "=== 快速測試配置 ===" << std::endl;
    std::cout << "重複次數: " << runs_per_configuration << " 次/配置 (快速驗證)" << std::endl;
    std::cout << "測試問題: " << test_benchmarks.size() << " 個 (berlin52)" << std::endl;
    std::cout << "預計線程配置: 1, 2, 4, 8" << std::endl;
    std::cout << "預計策略: 5 種 ACO 策略" << std::endl;
    std::cout << "預計總時間: 約 3-5 分鐘" << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== 開始快速平均測試 ===" << std::endl;
    
    try {
        analyzer.runDetailedAverageAnalysis(
            test_benchmarks, 
            runs_per_configuration, 
            "quick_average_test"
        );
        
        std::cout << "\n=== 快速測試完成！===" << std::endl;
        std::cout << "\n📊 生成的文件：" << std::endl;
        std::cout << "• quick_average_test_scalability_detailed.csv" << std::endl;
        std::cout << "• quick_average_test_strategy_detailed.csv" << std::endl;
        std::cout << "• quick_average_test_statistical_analysis.md" << std::endl;
        
        std::cout << "\n✅ 平均測試功能驗證成功！" << std::endl;
        std::cout << "\n💡 如需完整測試，請運行：" << std::endl;
        std::cout << "   ./enhanced_average_test.exe" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "快速測試失敗: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
