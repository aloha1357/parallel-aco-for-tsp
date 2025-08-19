#include <iostream>
#include <chrono>
#include <omp.h>
#include "../include/aco/Graph.hpp"
#include "../include/aco/AcoEngine.hpp"

void testParallelization() {
    std::cout << "=== 並行化診斷測試 ===" << std::endl;
    
    // 檢查OpenMP可用性
#ifdef _OPENMP
    std::cout << "OpenMP支援: 是" << std::endl;
    std::cout << "OpenMP版本: " << _OPENMP << std::endl;
#else
    std::cout << "OpenMP支援: 否" << std::endl;
#endif
    std::cout << "最大可用線程數: " << omp_get_max_threads() << std::endl;
    std::cout << "處理器數量: " << omp_get_num_procs() << std::endl;
    
    // 載入測試問題
    auto graph = Graph::fromTSPFile("data/eil51.tsp");
    std::cout << "\n使用測試實例: eil51 (" << graph->size() << " 個城市)" << std::endl;
    
    // 測試不同線程配置的詳細時間
    std::vector<int> thread_counts = {1, 2, 4, 8};
    
    for (int threads : thread_counts) {
        std::cout << "\n--- 測試 " << threads << " 線程 ---" << std::endl;
        
        AcoParameters params;
        params.num_ants = 20;
        params.max_iterations = 10;  // 較少迭代便於觀察
        params.num_threads = threads;
        params.random_seed = 42;
        
        AcoEngine engine(graph, params);
        
        // 手動設置線程數確認
        omp_set_num_threads(threads);
        
        std::cout << "設置線程數: " << threads << std::endl;
        std::cout << "實際使用線程數: ";
        
        // 在並行區域中檢查實際線程數
        #pragma omp parallel
        {
            #pragma omp single
            {
                std::cout << omp_get_num_threads() << std::endl;
            }
        }
        
        // 運行測試
        auto start = std::chrono::high_resolution_clock::now();
        auto results = engine.run();
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "執行時間: " << duration.count() << "ms" << std::endl;
        std::cout << "最佳解: " << results.best_tour_length << std::endl;
        std::cout << "實際迭代數: " << results.actual_iterations << std::endl;
    }
    
    // 詳細的線程活動檢查
    std::cout << "\n=== 線程活動檢查 ===" << std::endl;
    
    omp_set_num_threads(4);
    
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int num_threads = omp_get_num_threads();
        
        #pragma omp critical
        {
            std::cout << "線程 " << thread_id << "/" << num_threads 
                      << " 正在運行在處理器 " << omp_get_thread_num() << std::endl;
        }
        
        // 模擬螞蟻工作負載
        #pragma omp for schedule(static)
        for (int i = 0; i < 20; ++i) {
            #pragma omp critical
            {
                std::cout << "線程 " << thread_id << " 處理螞蟻 " << i << std::endl;
            }
            
            // 模擬計算工作
            volatile double sum = 0;
            for (int j = 0; j < 1000000; ++j) {
                sum += j * 0.001;
            }
        }
    }
}

int main() {
    try {
        testParallelization();
        std::cout << "\n✅ 並行化診斷完成" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "❌ 錯誤: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
