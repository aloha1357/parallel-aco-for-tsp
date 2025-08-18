# 🎯 **實驗系統運行指南**

由於編譯環境出現了一些問題，這裡提供幾個替代方案來運行您的實驗：

## 🚀 **方案1: 手動運行現有系統**

### 1. 使用現有的comprehensive_benchmark
```bash
cd build
cmake --build . --target comprehensive_benchmark --config Release
./comprehensive_benchmark
```

這將運行基本的性能測試並生成結果。

### 2. 修改現有程序進行TSPLIB測試
編輯 `comprehensive_benchmark.cpp` 中的測試參數：

```cpp
// 修改測試規模為您的目標問題
std::vector<int> test_sizes = {52, 100, 150};  // 對應berlin52, kroA100, kroA150

// 修改執行緒測試
std::vector<int> thread_counts = {1, 2, 4, 8};
```

## 🔧 **方案2: 使用Python腳本收集數據**

創建一個Python腳本來自動運行實驗：

```python
import subprocess
import csv
import time

def run_experiment():
    # 實驗配置
    thread_counts = [1, 2, 4, 8]
    problem_sizes = [52, 100, 150]
    runs_per_config = 5
    
    results = []
    
    for size in problem_sizes:
        for threads in thread_counts:
            for run in range(runs_per_config):
                print(f"運行: 問題{size}, 執行緒{threads}, 第{run+1}次")
                
                # 運行您的程序
                start_time = time.time()
                # result = subprocess.run(['./your_program', str(size), str(threads)], 
                #                       capture_output=True, text=True)
                end_time = time.time()
                
                # 記錄結果
                execution_time = (end_time - start_time) * 1000  # ms
                # tour_length = parse_output(result.stdout)
                
                results.append({
                    'problem_size': size,
                    'thread_count': threads,
                    'run_number': run + 1,
                    'execution_time_ms': execution_time,
                    # 'tour_length': tour_length
                })
    
    # 導出到CSV
    with open('experiment_results.csv', 'w', newline='') as f:
        writer = csv.DictWriter(f, fieldnames=results[0].keys())
        writer.writeheader()
        writer.writerows(results)

if __name__ == "__main__":
    run_experiment()
```

## 📊 **方案3: 手動數據記錄**

創建一個實驗記錄表格：

| 問題 | 執行緒 | 運行次數 | 執行時間(ms) | 路徑長度 | 近似比 |
|------|--------|----------|-------------|----------|---------|
| berlin52 | 1 | 1 | ? | ? | ? |
| berlin52 | 1 | 2 | ? | ? | ? |
| ... | ... | ... | ... | ... | ... |

對於每個配置，手動運行程序並記錄結果。

## 🎯 **關鍵實驗數據**

無論使用哪種方案，您需要收集以下核心數據：

### 必需指標
1. **執行時間** (Execution Time) - 毫秒
2. **最佳路徑長度** (Best Tour Length)
3. **執行緒數量** (Thread Count)
4. **問題規模** (Problem Size)

### 計算指標
1. **加速比** = T₁ / Tₚ (單執行緒時間 / 多執行緒時間)
2. **效率** = 加速比 / 執行緒數 × 100%
3. **近似比** = 找到的解 / 最優解

### 最優解參考值
- berlin52: 7542
- kroA100: 21282  
- kroA150: 26524

## 📈 **預期結果示例**

### 加速比表格
| 問題 | 1執行緒 | 2執行緒 | 4執行緒 | 8執行緒 |
|------|--------|--------|--------|--------|
| berlin52 | 1.00x | 1.8x | 3.2x | 5.5x |
| kroA100 | 1.00x | 1.9x | 3.5x | 6.1x |
| kroA150 | 1.00x | 1.9x | 3.7x | 6.8x |

### 解質量表格  
| 問題 | 最優解 | 平均找到解 | 近似比 |
|------|--------|------------|---------|
| berlin52 | 7542 | 7685 | 1.019 |
| kroA100 | 21282 | 21850 | 1.027 |
| kroA150 | 26524 | 27200 | 1.025 |

## 🔧 **快速修復建議**

如果要修復編譯問題：

1. **檢查命名空間衝突**
   ```cpp
   // 確保main函數在全局命名空間中
   // 避免在頭文件中使用 using namespace
   ```

2. **簡化包含**
   ```cpp
   // 只包含必要的頭文件
   #include <iostream>
   #include <vector>
   #include <chrono>
   ```

3. **使用最簡實驗程序**
   ```cpp
   int main() {
       // 直接調用核心ACO算法
       // 測量時間和記錄結果
       return 0;
   }
   ```

## ✅ **實驗成功標準**

1. **完整數據**: 3問題 × 4執行緒 × 5重複 = 60個數據點
2. **合理加速比**: 4執行緒達到3x+加速比
3. **穩定解質量**: 近似比 < 1.05
4. **清晰圖表**: 加速比、效率、解質量對比圖

即使遇到編譯問題，您仍然可以通過手動運行和數據記錄完成完整的實驗評估！
