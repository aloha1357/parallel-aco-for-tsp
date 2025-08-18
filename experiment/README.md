# 四個實例平行ACO實驗設計

## 實驗目標

針對四個不同規模的TSP實例（50、100、150、202城市），評估平行ACO演算法的性能表現，比較與最佳解的差異，分析時間效率和加速比。

## 實驗實例

| 實例名稱 | 城市數 | 最佳解 | 分類 | 描述 |
|---------|--------|--------|------|------|
| eil51 | 51 | 426 | 小規模 | 51城市實例(接近50城市) |
| kroA100 | 100 | 21282 | 中規模 | 100城市Krolak問題A |
| kroA150 | 150 | 26524 | 大規模 | 150城市Krolak問題A |
| gr202 | 202 | 40160 | 超大規模 | 202城市地理實例 |

## 線程配置

測試1、2、4、8個線程的配置，評估平行化效果。

## 實驗設計

### 實驗1: 固定迭代數實驗

**目的**: 比較一般ACO與平行ACO在相同迭代資源下的解品質

**設定**:
- 固定迭代數: 100次迭代
- 重複次數: 每個配置重複100次
- 不設時間限制
- 測量指標:
  - `tour_length`: 最終解的路徑長度
  - `ratio_to_best`: tour_length / optimal_solution
  - `wall_time`: 整體執行時間(秒)
  - `speedup_ratio`: 相對於單線程的加速比
  - `efficiency`: 加速比/線程數

### 實驗2: 固定時間預算實驗

**目的**: 比較在相同時間預算下的解品質

**設定**:
- 時間預算: 以1線程100迭代的執行時間為基準
- 最大迭代數: 10000 (由時間預算控制停止)
- 重複次數: 每個配置重複100次
- 測量指標:
  - 同實驗1的所有指標
  - `actual_iterations`: 實際完成的迭代數
  - `iterations_per_second`: 每秒迭代數

## ACO參數設定

```json
{
  "alpha": 1.0,      // 費洛蒙重要性
  "beta": 2.0,       // 啟發式資訊重要性  
  "rho": 0.1,        // 費洛蒙蒸發率
  "num_ants": 50,    // 螞蟻數量
  "q0": 0.9          // 開發vs探索參數
}
```

## 實驗執行流程

### 步驟1: 基準時間測量
- 測量每個實例在1線程、100迭代下的執行時間
- 執行5次取平均值作為時間預算基準
- 結果儲存至 `experiment/results/baseline_measurements/`

### 步驟2: 固定迭代數實驗  
- 所有實例 × 所有線程配置 × 100次重複
- 結果儲存至 `experiment/results/fixed_iterations/`

### 步驟3: 固定時間預算實驗
- 使用步驟1的基準時間作為各配置的時間預算
- 所有實例 × 所有線程配置 × 100次重複  
- 結果儲存至 `experiment/results/time_budget/`

## 輸出格式

### CSV格式
每次執行一列，包含以下欄位:
```
timestamp,experiment_type,algorithm,instance_name,instance_size,run_number,
max_iterations,actual_iterations,time_budget_s,wall_time_s,tour_length,
optimal_solution,ratio_to_best,num_threads,convergence_iteration,
random_seed,speedup_ratio,efficiency,iterations_per_second
```

## 開發模式 vs 正式模式

### 開發模式 (預設)
```bash
./four_instances_experiment_runner
```
- 迭代數: 2
- 重複次數: 5  
- 基準測量: 3次
- 用途: 快速驗證實驗設定

### 正式模式
```bash
./four_instances_experiment_runner --production
```
- 迭代數: 100
- 重複次數: 100
- 基準測量: 5次
- 用途: 完整的實驗數據收集

## 預期分析

### 解品質分析
- 各實例的平均解品質與最佳解比較
- 不同線程數對解品質的影響
- 統計顯著性測試

### 性能分析  
- 加速比分析: speedup = T_serial / T_parallel
- 效率分析: efficiency = speedup / num_threads
- 強擴展性 vs 弱擴展性

### 時間預算分析
- 相同時間下各配置完成的迭代數
- 每秒迭代數的比較
- 時間利用效率

## 統計分析方法

- 信賴區間: 95%
- 統計檢定: t-test, ANOVA, Wilcoxon
- 報告格式: 平均值 ± 標準差

## 注意事項

1. 確保data/目錄下有所有TSP實例檔案
2. 實驗可能需要較長時間(正式模式)
3. 確保足夠的磁碟空間儲存結果
4. 建議先執行開發模式驗證設定

## 編譯與執行

```bash
# 編譯
g++ -std=c++17 -O3 -I../src -pthread four_instances_experiment_runner.cpp -o four_instances_experiment_runner

# 開發測試
./four_instances_experiment_runner

# 正式實驗  
./four_instances_experiment_runner --production
```
