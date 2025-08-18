# 🚀 標準實驗快速開始指南

## 📋 實驗概述

本指南將幫助您按照學術標準運行平行化ACO TSP實驗，獲得以下核心數據：

### 實驗目標
1. **🚀 加速比分析** - 量化平行化的性能提升
2. **📊 解質量評估** - 評估與最優解的近似比
3. **📈 可擴展性研究** - 分析不同問題規模的平行化效益

### 測試配置
- **實例規模**: berlin52 (52城市), kroA100 (100城市), kroA150 (150城市)
- **執行緒數**: 1, 2, 4, 8
- **統計樣本**: 每配置運行5次
- **總實驗次數**: 60次 (3問題 × 4執行緒 × 5運行)

---

## 🛠️ 環境準備

### 1. 系統需求
```
OS: Windows 11 / Linux / macOS
CPU: 4核心以上 (建議8核心)
RAM: 8GB以上
編譯器: g++ 9.0+ 或 MSVC 2019+
依賴: OpenMP, CMake 3.20+
```

### 2. 下載TSPLIB數據
```bash
# 執行數據下載腳本
python download_tsplib_data.py
```

數據檔案將保存到 `data/` 目錄：
```
data/
├── berlin52.tsp    (7542 - 最優解)
├── kroA100.tsp     (21282 - 最優解)  
└── kroA150.tsp     (26524 - 最優解)
```

### 3. 編譯項目
```bash
# Windows (PowerShell)
mkdir build; cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release

# Linux/macOS
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

---

## 🎯 運行實驗

### 完整標準實驗
```bash
# 運行標準化實驗 (約10-15分鐘)
./tsplib_benchmark_experiment
```

實驗會自動：
1. ✅ 載入TSPLIB標準測試實例
2. ✅ 運行所有執行緒配置 (1, 2, 4, 8)
3. ✅ 每配置重複5次取統計平均
4. ✅ 記錄性能指標和解質量
5. ✅ 導出CSV數據和分析報告

### 監控實驗進度
```
=== 開始完整實驗 ===

進度: [1/3] 測試問題: berlin52 (7542)
  建立基準線 (單執行緒)... 完成 (1205.3ms)
  問題規模: 52 城市
    測試 1 執行緒... [1] [2] [3] [4] [5] 完成 (平均: 1198.2ms, 7634.5)
    測試 2 執行緒... [1] [2] [3] [4] [5] 完成 (平均: 623.1ms, 7689.1)
    測試 4 執行緒... [1] [2] [3] [4] [5] 完成 (平均: 345.7ms, 7701.8)
    測試 8 執行緒... [1] [2] [3] [4] [5] 完成 (平均: 198.4ms, 7758.2)

進度: [2/3] 測試問題: kroA100 (21282)
  ...
```

---

## 📊 結果分析

### 輸出檔案結構
```
experiment_results/
├── detailed_results.csv           # 詳細原始數據
├── aggregated_results.csv         # 統計聚合數據
├── EXPERIMENT_REPORT.md           # 分析報告
├── generate_experiment_plots.py   # 圖表生成腳本
└── partial_results_*.csv          # 中間結果備份
```

### 核心數據表 (aggregated_results.csv)
```csv
Problem_Name,Thread_Count,Mean_Time_ms,Mean_Speedup,Mean_Efficiency,Mean_Approximation_Ratio
berlin52,1,1198.2,1.000,100.0,1.012
berlin52,2,623.1,1.923,96.2,1.018
berlin52,4,345.7,3.467,86.7,1.021
berlin52,8,198.4,6.042,75.5,1.027
kroA100,1,4567.8,1.000,100.0,1.034
...
```

### 生成圖表
```bash
# 安裝Python依賴
pip install pandas matplotlib seaborn numpy

# 生成實驗圖表
cd experiment_results
python generate_experiment_plots.py
```

生成的圖表：
- **experiment_analysis.png** - 包含4個子圖：
  1. 🚀 加速比 vs 執行緒數
  2. ⚡ 平行效率 vs 執行緒數  
  3. 🎯 近似比 vs 執行緒數
  4. 📊 成功率 vs 執行緒數

---

## 📈 預期結果

### 典型性能指標
| 問題 | 執行緒 | 加速比 | 效率 | 近似比 |
|------|--------|--------|------|--------|
| berlin52 | 1 | 1.00x | 100% | 1.012 |
| berlin52 | 2 | 1.92x | 96% | 1.018 |
| berlin52 | 4 | 3.47x | 87% | 1.021 |
| berlin52 | 8 | 6.04x | 76% | 1.027 |

### 關鍵結論
1. **📈 顯著加速**: 4執行緒達到3.5x+加速比
2. **✅ 保持質量**: 近似比保持在1.03以下
3. **⚖️ 最佳配置**: 4執行緒平衡速度與效率

---

## 🔧 自定義實驗

### 修改實驗參數
編輯 `experiment_config.json`:
```json
{
  "thread_configurations": [1, 2, 4, 8, 16],
  "experiment_settings": {
    "runs_per_configuration": 10,
    "timeout_seconds": 600
  },
  "aco_parameters": {
    "max_iterations": 200,
    "alpha": 1.0,
    "beta": 2.0
  }
}
```

### 添加新測試實例
1. 下載新的TSPLIB檔案到 `data/`
2. 在 `TSPLibReader::getStandardBenchmarks()` 中添加：
```cpp
{"new_instance", "data/new_instance.tsp", optimal_value}
```

### 測試更多執行緒
修改 `ExperimentConfig::thread_counts`:
```cpp
std::vector<int> thread_counts = {1, 2, 4, 8, 16, 32};
```

---

## 📊 學術報告建議

### 實驗設計章節
```markdown
### 4.1 實驗設置
- **測試平台**: Intel Core i7-XXXX, 8核16執行緒, 32GB RAM
- **測試實例**: TSPLIB標準問題集 (berlin52, kroA100, kroA150)
- **測試配置**: 1-8執行緒, 每配置5次重複實驗
- **評估指標**: 加速比、平行效率、近似比

### 4.2 結果分析
根據實驗數據，平行化ACO展現以下特性：
1. 在4執行緒配置下達到最佳性價比
2. 加速比隨問題規模增長而改善
3. 解質量在平行化後保持穩定...
```

### 圖表標準
- **圖4.1**: 加速比與執行緒數關係
- **圖4.2**: 平行效率分析  
- **圖4.3**: 解質量對比
- **表4.1**: 性能指標摘要

---

## 🆘 常見問題

### Q: 編譯失敗，找不到OpenMP
```bash
# Ubuntu/Debian
sudo apt-get install libomp-dev

# Windows MSVC
# 項目屬性 -> C/C++ -> 語言 -> OpenMP支援: 是

# macOS
brew install libomp
```

### Q: TSPLIB數據下載失敗
手動下載並放置到 `data/` 目錄，或使用備用數據：
```bash
python download_tsplib_data.py
# 腳本會自動創建備用測試數據
```

### Q: 實驗時間過長
減少重複次數或問題規模：
```cpp
config.runs_per_configuration = 3;  // 原為5
config.thread_counts = {1, 2, 4};   // 原為{1,2,4,8}
```

### Q: 記憶體不足
測試更小的問題或減少執行緒數：
```cpp
// 只測試小規模問題
{"berlin52", "data/berlin52.tsp", 7542}
```

---

## 📞 技術支援

如遇到問題，請檢查：
1. 📋 系統需求和依賴安裝
2. 🔧 CMake配置和編譯選項
3. 📁 數據檔案完整性
4. 💻 硬體資源充足性

**實驗成功標誌**: 生成完整的CSV數據和圖表，報告顯示合理的加速比和近似比數值。
