#include "aco/BenchmarkAnalyzer.hpp"
#include "aco/TSPLibReader.hpp"
#include <iostream>
#include <vector>

using namespace aco;

int main() {
    std::cout << "=== 增強版平均測試分析程序 ===" << std::endl;
    std::cout << "此程序將進行詳細的統計分析，包括：" << std::endl;
    std::cout << "✓ 多次重複運行的平均分析" << std::endl;
    std::cout << "✓ 標準差和置信區間計算" << std::endl;
    std::cout << "✓ 統計顯著性檢驗" << std::endl;
    std::cout << "✓ 詳細的性能穩定性評估" << std::endl;
    std::cout << std::endl;

    BenchmarkAnalyzer analyzer;
    
    // 選擇測試問題 - 使用不同規模的問題
    std::vector<TSPBenchmark> test_benchmarks = {
        {"eil51", "data/eil51.tsp", 426, 51},        // 小規模
        {"berlin52", "data/berlin52.tsp", 7542, 52}, // 中規模
        {"kroA100", "data/kroA100.tsp", 21282, 100}  // 大規模
    };
    
    // 配置測試參數
    int runs_per_configuration = 10;  // 每個配置運行10次以獲得統計顯著性
    
    std::cout << "=== 測試配置 ===" << std::endl;
    std::cout << "重複次數: " << runs_per_configuration << " 次/配置" << std::endl;
    std::cout << "測試問題: " << test_benchmarks.size() << " 個" << std::endl;
    std::cout << "預計總運行次數: " << test_benchmarks.size() * 4 * runs_per_configuration << " (可擴展性)" << std::endl;
    std::cout << "                + " << test_benchmarks.size() * 5 * runs_per_configuration << " (策略比較)" << std::endl;
    std::cout << "預計總時間: 約 15-25 分鐘" << std::endl;
    std::cout << std::endl;
    
    // 確認用戶是否要繼續
    std::cout << "按 Enter 鍵開始測試，或輸入 'q' 退出: ";
    std::string input;
    std::getline(std::cin, input);
    if (input == "q" || input == "Q") {
        std::cout << "測試已取消。" << std::endl;
        return 0;
    }
    
    // 開始詳細平均分析
    std::cout << "=== 開始增強版平均測試 ===" << std::endl;
    
    try {
        analyzer.runDetailedAverageAnalysis(
            test_benchmarks, 
            runs_per_configuration, 
            "enhanced_average_analysis"
        );
        
        std::cout << "\n=== 測試完成！===" << std::endl;
        std::cout << "\n📊 生成的分析文件：" << std::endl;
        std::cout << "1. enhanced_average_analysis_scalability_detailed.csv - 詳細可擴展性數據" << std::endl;
        std::cout << "2. enhanced_average_analysis_strategy_detailed.csv - 詳細策略比較數據" << std::endl;
        std::cout << "3. enhanced_average_analysis_statistical_analysis.md - 統計分析報告" << std::endl;
        std::cout << "4. generate_plots.py - 圖表生成腳本" << std::endl;
        
        std::cout << "\n📈 建議的後續步驟：" << std::endl;
        std::cout << "1. 安裝 Python 依賴:" << std::endl;
        std::cout << "   pip install pandas matplotlib seaborn numpy scipy" << std::endl;
        std::cout << "\n2. 生成視覺化圖表:" << std::endl;
        std::cout << "   python generate_plots.py" << std::endl;
        std::cout << "\n3. 查看統計分析報告:" << std::endl;
        std::cout << "   記事本打開 enhanced_average_analysis_statistical_analysis.md" << std::endl;
        
        std::cout << "\n💡 分析重點：" << std::endl;
        std::cout << "• 注意置信區間的大小 - 較小的區間表示結果更可靠" << std::endl;
        std::cout << "• 比較不同配置的變異係數 - 選擇穩定性高的配置" << std::endl;
        std::cout << "• 關注平行效率 - 尋找最佳的線程數配置" << std::endl;
        std::cout << "• 評估策略的風險回報比 - 平衡解質量和執行時間" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "測試過程中發生錯誤: " << e.what() << std::endl;
        std::cerr << "\n可能的原因：" << std::endl;
        std::cerr << "1. 缺少測試數據檔案 (data/*.tsp)" << std::endl;
        std::cerr << "2. 記憶體不足" << std::endl;
        std::cerr << "3. 磁碟空間不足" << std::endl;
        std::cerr << "\n建議解決方案：" << std::endl;
        std::cerr << "• 運行 python download_tsplib_data.py 下載測試數據" << std::endl;
        std::cerr << "• 關閉其他應用程序釋放記憶體" << std::endl;
        std::cerr << "• 檢查磁碟空間" << std::endl;
        return 1;
    }
    
    return 0;
}
