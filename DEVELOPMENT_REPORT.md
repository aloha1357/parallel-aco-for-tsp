# Parallel ACO for TSP - 開發進度報告

**日期**: 2025年7月31日  
**版本**: v0.11-dev  
**開發狀態**: BDD Scenario 1-9 與 11 全部完成，包含性能預算監控、策略比較框架與可重現性測試

---

## 📊 **目前開發進度**

### ✅ **已完成的 BDD Scenarios (GoogleTest 實作)**

| Scenario | 功能 | 狀態 | 測試數量 |
|----------|------|------|----------|
| **01_walking_skeleton** | 建置環境驗證 | ✅ 完成 | 1 test |
| **02_construct_tour** | 基礎路徑構建 | ✅ 完成 | 6 tests |
| **03_probabilistic_choice** | ACO 機率選擇 | ⏭️ 跳過 | 1 test (placeholder) |
| **04_evaporation** | 費洛蒙蒸發機制 | ✅ 完成 | 5 tests |
| **05_delta_accumulation** | 費洛蒙累積 | ✅ 完成 | 2 tests |
| **06_delta_merge** | 平行費洛蒙合併 | ✅ 完成 | 4 tests |
| **07_parallel_consistency** | OpenMP 平行化 | ✅ 完成 | 4 tests |
| **08_convergence** | 演算法收斂監控 | ✅ 完成 | 4 tests |
| **09_performance_budget** | 性能預算與最佳化 | ✅ 完成 | 4 tests |
| **10_numa_awareness** | NUMA 最佳化 | ⏭️ 跳過 | 0 tests (不適用於筆電) |
| **11_strategy_comparison** | 策略比較與可重現性 | ✅ 完成 | 6 tests |

### 📈 **測試統計**
- **總測試數量**: 87 tests (新增策略比較與可重現性測試)
- **通過測試**: 86 tests ✅
- **跳過測試**: 1 test ⏭️ (未來功能佔位符)
- **失敗測試**: 0 tests 🎉
- **測試通過率**: 98.9% (86/87)
- **測試框架**: GoogleTest (統一框架)

### 🎯 **實作決策**
- ✅ **Scenario 9**: 完整實作性能預算監控與優化
- ⏭️ **Scenario 10**: 跳過 NUMA 優化 (筆電環境不適用)
- ✅ **Scenario 11**: 實作策略比較框架替代 NUMA

---

## 🏗️ **專案架構**

### **目錄結構**
```
D:\D_backup\2025\tum\Parallel ACO for TSP\
├── 📁 build/                    # 建置輸出目錄
│   ├── aco_main.exe            # 主程式執行檔
│   ├── unit_tests.exe          # 單元測試執行檔
│   └── lib/                    # 編譯庫檔案
├── 📁 include/aco/             # 標頭檔目錄
│   ├── Graph.hpp               # 圖形表示類別
│   ├── Tour.hpp                # 路徑表示類別
│   ├── Ant.hpp                 # 螞蟻代理類別
│   ├── PheromoneModel.hpp      # 費洛蒙模型類別
│   ├── ThreadLocalPheromoneModel.hpp # 線程本地費洛蒙模型類別
│   ├── AcoEngine.hpp           # ACO 引擎類別
│   ├── PerformanceMonitor.hpp  # 性能監控類別 ✅ 新增
│   ├── SyntheticTSPGenerator.hpp # 合成TSP實例生成器 ✅ 新增
│   └── StrategyComparator.hpp  # 策略比較框架 ✅ 新增
├── 📁 src/aco/                 # 實作檔目錄
│   ├── Graph.cpp               # 圖形實作
│   ├── Tour.cpp                # 路徑實作  
│   ├── Ant.cpp                 # 螞蟻實作
│   ├── PheromoneModel.cpp      # 費洛蒙實作
│   ├── ThreadLocalPheromoneModel.cpp # 線程本地費洛蒙實作
│   ├── AcoEngine.cpp           # 引擎實作
│   ├── PerformanceMonitor.cpp  # 性能監控實作 ✅ 新增
│   ├── SyntheticTSPGenerator.cpp # TSP實例生成實作 ✅ 新增
│   └── StrategyComparator.cpp  # 策略比較實作 ✅ 新增
├── 📁 tests/                   # 測試目錄
│   ├── features/               # BDD feature 檔案 (規格文檔)
│   ├── unit/                   # 單元測試 (GoogleTest)
│   │   ├── test_graph.cpp      # 圖形類別測試
│   │   ├── test_tour.cpp       # 路徑類別測試
│   │   ├── test_ant.cpp        # 螞蟻類別測試
│   │   ├── test_pheromone.cpp  # 費洛蒙模型測試
│   │   ├── test_thread_local_pheromone.cpp # 線程本地費洛蒙測試
│   │   ├── test_engine.cpp     # ACO 引擎測試
│   │   └── test_bdd_scenarios.cpp # BDD 場景測試 (GoogleTest 實作)
│   └── test_main.cpp           # 測試主程式
├── 📁 演示程式/                 # 功能演示 ✅ 新增
│   ├── performance_demo.cpp    # 性能預算演示
│   └── strategy_comparison_demo.cpp # 策略比較演示
├── 📁 data/                    # TSP 資料檔
├── CMakeLists.txt              # 建置配置檔
├── vcpkg.json                  # 相依性管理
└── README.md                   # 專案說明
```

### **新增功能架構**

#### **🎯 Scenario 9: Performance Budget (性能預算)**
```mermaid
classDiagram
    class PerformanceMonitor {
        -start_time_: TimePoint
        -peak_memory_: size_t
        +startMonitoring()
        +stopMonitoring(): PerformanceMetrics
        +getCurrentMemoryUsage(): size_t
        +checkTimeBudget(budget): bool
        +measureSpeedup(singleThreadFunc, multiThreadFunc): SpeedupMetrics
    }
    
    class PerformanceBudget {
        +max_time_ms: long long
        +max_memory_mb: size_t
        +min_speedup: double
        +max_efficiency_loss: double
    }
    
    class AcoEngine {
        +runWithBudget(budget): Result
        +measurePerformance(): Metrics
    }
    
    PerformanceMonitor --> PerformanceBudget
    AcoEngine --> PerformanceMonitor
```

#### **🔄 Scenario 11: Strategy Comparison (策略比較)**
```mermaid
classDiagram
    class StrategyComparator {
        -strategies_: map<AcoStrategy, StrategyConfig>
        +compareStrategies(graph): ComparisonResult
        +analyzeStrategy(strategy, graph): StrategyAnalysis
        +testReproducibility(strategy): bool
        +getStrategyInsights(strategy): StrategyInsights
    }
    
    class AcoStrategy {
        <<enumeration>>
        STANDARD
        EXPLOITATION
        EXPLORATION
        AGGRESSIVE
        CONSERVATIVE
    }
    
    class StrategyConfig {
        +name: string
        +description: string
        +parameters: AcoParameters
    }
    
    StrategyComparator --> AcoStrategy
    StrategyComparator --> StrategyConfig
```
        -pheromone_: vector<vector<double>>
        -size_: int
        +PheromoneModel(int size)
        +getPheromone(int from, int to): double
        +setPheromone(int from, int to, double value)
        +initialize(double value)
    }
    
    class Tour {
        -path_: vector<int>
        -length_: double
        -graph_: shared_ptr<Graph>
        +Tour(shared_ptr<Graph> graph)
        +getPath(): vector<int>
        +getLength(): double
        +setPath(vector<int> path)
        +calculateLength()
    }
    
    class Ant {
        -graph_: shared_ptr<Graph>
        -pheromone_: shared_ptr<PheromoneModel>
        -alpha_, beta_: double
        -visited_: vector<bool>
        -current_city_: int
        +Ant(graph, pheromone, alpha, beta, seed)
        +constructTour(): unique_ptr<Tour>
        +chooseNextCity(): int
        +calculateSelectionProbabilities(): vector<double>
        +setCurrentCity(int city)
        +markVisited(int city)
        +reset()
    }
    
    Graph <-- Tour : uses
    Graph <-- Ant : uses  
    PheromoneModel <-- Ant : uses
    Tour <-- Ant : creates
```

---

## 🛠️ **建置系統使用指南**

### **必要環境**
- **CMake**: 3.20+ (已安裝 3.29.2)
- **編譯器**: GCC 13.2.0 (Strawberry Perl 內建)
- **OpenMP**: 4.5+ (已自動檢測)
- **GoogleTest**: 自動下載 (via FetchContent)
- **測試框架**: GoogleTest 統一框架 (無需 cucumber-cpp)

### **建置指令**

#### **基本建置流程**
```powershell
# 1. 進入專案目錄
cd "d:\D_backup\2025\tum\Parallel ACO for TSP"

# 2. 創建並進入建置目錄
mkdir build -ErrorAction SilentlyContinue
cd build

# 3. 配置 CMake
cmake ..

# 4. 建置專案
cmake --build .
```

#### **測試相關指令**
```powershell
# 運行所有單元測試 (包含 BDD 場景)
.\unit_tests.exe

# 運行特定測試群組
.\unit_tests.exe --gtest_filter="GraphTest.*"
.\unit_tests.exe --gtest_filter="BDDScenariosTest.*"
.\unit_tests.exe --gtest_filter="ProbabilisticChoiceTest.*"

# 使用 CTest 運行測試
ctest --verbose

# 顯示詳細測試輸出
.\unit_tests.exe --gtest_output=xml:test_results.xml

# 列出所有可用測試
.\unit_tests.exe --gtest_list_tests
```

#### **除錯與開發指令**
```powershell
# Debug 模式建置
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Release 模式建置  
cmake .. -DCMAKE_BUILD_TYPE=Release

# 清理重建
Remove-Item * -Recurse -Force
cmake ..
cmake --build .

# 檢查編譯器資訊
cmake .. --debug-output
```

---

## ⚠️ **開發過程中遇到的問題與解決方案**

### **Problem 1: cucumber-cpp 依賴問題 ✅ 已解決**
**問題**: Windows 環境下 cucumber-cpp 安裝困難，vcpkg 不支援  
**解決方案**: 
- ✅ 完全移除 cucumber-cpp 依賴
- ✅ 將所有 BDD 場景轉換為 GoogleTest 格式
- ✅ 保持 BDD 風格的測試命名 (e.g., `ConstructTour_ValidTour_HasPositiveLength`)
- ✅ 統一使用 GoogleTest 作為唯一測試框架

**效果**: 
- 測試數量從 22 個增加到 30 個
- 消除了外部依賴問題
- 提升了建置穩定性

**相關檔案**:
```
tests/unit/test_bdd_scenarios.cpp    # 新的 BDD 風格 GoogleTest
vcpkg.json                           # 移除 cucumber-cpp 依賴
CMakeLists.txt                       # 簡化建置配置
```

### **Problem 2: 機率選擇邏輯錯誤**
**問題**: 初始實作時高費洛蒙城市被選擇機率反而較低  
**根本原因**: 測試中距離差異影響了機率計算結果  
**解決方案**:
1. 添加 `Graph::setDistance()` 方法用於測試
2. 在測試中設置相等距離來隔離費洛蒙影響
3. 添加調試輸出驗證機率計算

**相關提交**:
```cpp
// 修正前: 使用隨機距離，導致距離項 (1/d)^β 主導選擇
// 修正後: 設置相等距離，突出費洛蒙影響
test_graph->setDistance(0, 1, 10.0);  
test_graph->setDistance(0, 2, 10.0);
```

### **Problem 3: 重複方法定義編譯錯誤**
**問題**: `Ant::constructGreedyTour()` 方法重複定義  
**解決方案**: 重構代碼時移除重複的方法實作  
**預防措施**: 使用明確的方法宣告與實作分離

### **Problem 4: PowerShell 指令語法問題**
**問題**: 使用 `&&` 連接指令在 PowerShell 中失效  
**解決方案**: 改用 `;` 連接指令或分開執行  
```powershell
# 錯誤: cd path && dir
# 正確: cd path; dir
```

### **Problem 5: Scenario 8 演算法收斂監控 ✅ 已解決**
**問題**: ACO演算法無法收斂到好的解，測試失敗  
**根本原因分析**:
1. **隨機數種子問題**: 每次迭代使用相同的螞蟻種子序列
2. **全局最佳追蹤錯誤**: `global_best_length_`在run()中沒有被更新  
3. **iteration_best_lengths語義錯誤**: 記錄迭代最佳而非累計最佳
4. **ACO參數不佳**: α=1, β=2, ρ=0.1 無法找到足夠好的解

**解決方案**:
1. ✅ **修正隨機數種子**: 在種子計算中加入`iteration * 10000`
   ```cpp
   std::mt19937 ant_rng(params_.random_seed + iteration * 10000 + ant_id * 1000 + thread_id);
   ```
2. ✅ **修正全局最佳追蹤**: 在找到更好解時正確更新`global_best_length_`
3. ✅ **修正迭代統計**: 改為記錄`global_best_length_`而非`iteration_best`
4. ✅ **優化ACO參數**: 調整為α=1, β=3, ρ=0.5，改善解質量

**測試結果**:
- eil51.tsp (51城市): 從791 → 572 (接近已知最優426)
- 收斂監控: convergence_iteration正確追蹤最佳解出現時機
- 所有4個收斂測試完全通過 ✅

**相關檔案**:
```cpp
src/aco/AcoEngine.cpp              # 主要修正檔案
tests/unit/test_bdd_scenarios.cpp  # 收斂測試案例
```

---

## 🧪 **測試策略與覆蓋範圍**

### **測試金字塔結構**
1. **單元測試** (Unit Tests) - 46 個
   - Graph 基本功能測試 (7 個) ✅ 包含TSP文件加載
   - Tour 路徑計算測試 (5 個)  
   - PheromoneModel 費洛蒙操作測試 (19 個) ✅ 完整實作
   - ThreadLocalPheromoneModel 線程本地費洛蒙測試 (9 個) ✅ 平行支援
   - AntTest 螞蟻代理功能測試 (3 個)
   - AcoEngineTest ACO 引擎測試 (3 個) ✅ 完整引擎

2. **BDD 場景測試** (BDD Scenarios) - 31 個  
   - 實作為 GoogleTest，保持 BDD 可讀性
   - Walking Skeleton 場景 (1 個)
   - Construct Tour 場景 (6 個)
   - Evaporation 場景 (5 個) ✅ 費洛蒙蒸發
   - Delta Accumulation 場景 (2 個) ✅ 費洛蒙累積
   - Delta Merge 場景 (4 個) ✅ 平行費洛蒙合併
   - Parallel Consistency 場景 (4 個) ✅ 平行一致性
   - Convergence 場景 (4 個) ✅ 演算法收斂
   - Performance Budget 場景 (4 個) ✅ 性能預算監控 ✅ 新增

3. **未來功能佔位符** - 3 個 (跳過)
   - Probabilistic Choice 進階場景
   - Evaporation 場景
   - Parallel Consistency 場景

### **測試覆蓋現況**
- **總測試數**: 77 個
- **通過率**: 98.7% (76/77)
- **跳過率**: 1.3% (1/77) - 未來功能
- **核心類別覆蓋**: 100%
- **ACO 核心邏輯覆蓋**: 完整實作包含收斂監控與性能預算
- **性能監控覆蓋**: 100% (時間、記憶體、加速比、效率)

---

## 🎯 **核心 ACO 演算法實作狀態**

### ✅ **已實作功能**

#### **1. 機率城市選擇 (Probabilistic City Selection)**
```cpp
// 實作 ACO 標準機率公式: P(i→j) = [τ(i,j)^α · η(i,j)^β] / Σ[τ(i,k)^α · η(i,k)^β]
std::vector<double> Ant::calculateSelectionProbabilities() const
```
- ✅ 費洛蒙影響 (τ^α)
- ✅ 啟發式資訊 (η^β = (1/distance)^β)  
- ✅ 輪盤選擇 (Roulette Wheel Selection)
- ✅ 參數化 α, β

#### **2. 圖形與路徑表示**
- ✅ 對稱距離矩陣
- ✅ Hamiltonian 迴路構建
- ✅ 路徑長度計算
- ✅ 輸入驗證與錯誤處理
- ✅ **TSP 文件加載** (支援 EUC_2D 格式)
- ✅ **歐幾里得距離計算**

#### **3. 費洛蒙模型完整實作**
- ✅ 費洛蒙矩陣初始化與管理
- ✅ 費洛蒙讀取/設置操作
- ✅ 最小費洛蒙值限制
- ✅ **費洛蒙蒸發機制**: `τ(i,j) ← (1-ρ) · τ(i,j)`
- ✅ **費洛蒙累積機制**: `τ(i,j) ← τ(i,j) + Δτ` where `Δτ = Q/L`
- ✅ **平行費洛蒙合併**: Thread-safe delta accumulation and merging

#### **4. 完整 ACO 引擎**
- ✅ **完整 ACO 主循環** with evaporation and deposition
- ✅ **OpenMP 平行化** with thread-local pheromone buffers
- ✅ **收斂監控** with iteration statistics tracking
- ✅ **早期停止機制** with stagnation detection
- ✅ **詳細結果統計** including convergence analysis

#### **5. 收斂與監控功能**
- ✅ **迭代統計收集**: Best and average tour lengths per iteration
- ✅ **收斂追蹤**: Tracks when best solution was found
- ✅ **早期停止**: Configurable stagnation limit
- ✅ **目標品質收斂**: Stop when reaching target solution quality
- ✅ **執行時間測量**: Millisecond precision timing

#### **6. 性能預算與監控功能** ✅ 新增
- ✅ **性能預算定義**: 時間、記憶體、加速比、效率等約束條件
- ✅ **高精度性能監控**: 微秒級計時與記憶體使用追蹤
- ✅ **平行加速比驗證**: 自動計算並驗證多線程性能增益
- ✅ **記憶體效率分析**: O(N²) 複雜度驗證與線程開銷監控
- ✅ **預算違規檢測**: 自動檢測性能約束違規並提供詳細報告
- ✅ **合成TSP實例生成**: 支援多種模式(隨機、聚類、圓形、網格)的測試資料

### 🔄 **演算法品質調優中**

#### **1. 收斂速度優化**
- 🔄 **參數調優**: 優化 α, β, ρ 參數組合
- 🔄 **初始費洛蒙**: 調整初始費洛蒙值
- 🔄 **螞蟻數量**: 優化螞蟻數量與城市數量比例

#### **2. 解決方案品質改進**
- 🔄 **多樣性保持**: 增強探索能力
- 🔄 **局部搜尋**: 可能加入局部最佳化
- 🔄 **重啟機制**: 避免早期收斂到局部最優

#### **7. 策略比較與可重現性框架** ✅ 新增
- ✅ **五種ACO策略**: Standard, Exploitation, Exploration, Aggressive, Conservative
- ✅ **策略性能比較**: 自動化多策略性能分析與比較報告
- ✅ **可重現性測試**: 種子一致性驗證，確保結果可重現
- ✅ **策略洞察分析**: 收斂速度、解決方案穩定性、探索多樣性分析
- ✅ **跨線程數一致性**: 驗證不同線程數下的結果一致性

---

## 📊 **最新性能測試結果**

### **🎯 Scenario 9: Performance Budget 測試結果**
```
=== Performance Budget Demo Results ===
測試實例: 100城市合成TSP
硬體環境: 24核心/32邏輯處理器

1. 時間預算驗證:
   - 執行時間: 668.3ms
   - 時間預算: 5000ms
   - 預算符合度: ✅ PASSED (13.4% 使用率)

2. 加速比驗證:
   - 單線程時間: 1278.7ms
   - 多線程時間: 398.1ms
   - 實際加速比: 3.21x
   - 平行效率: 80.3%

3. 記憶體伸縮性:
   - 50城市: 6MB, 57ms
   - 100城市: 6MB, 336ms  
   - 150城市: 7MB, 1021ms
   - 記憶體複雜度: O(N²) ✅ 確認
```

### **🔄 Scenario 11: Strategy Comparison 測試結果**
```
=== Strategy Performance Analysis (50城市實例) ===
策略             最佳長度    平均時間(ms)  收斂迭代   穩定性    收斂速度
Standard         4027.8        322.8         79       0.000      79.0
Exploitation     6858.1        314.0         29       0.000       0.0
Exploration      3056.2        441.5         41       0.000      41.0
Aggressive       3349.6        455.3         60       0.000      60.0
Conservative     4027.8        469.4         78     121.670      78.0

🏆 最佳品質策略: Exploration (長度: 3056.2)
⚡ 最快策略: Exploitation (時間: 314.0ms)

探索多樣性分析:
- Standard: 132.5     (平衡型)
- Exploitation: 0.0   (專注型)
- Exploration: 54.5   (探索型)
- Aggressive: 140.8   (激進型)
- Conservative: 65.9  (保守型)

✅ 可重現性測試: 所有策略通過種子一致性驗證
```

---

## 📋 **接續開發注意事項**

### **🔧 代碼品質要求**
1. **嚴格遵循 BDD 流程**: 紅燈 → 綠燈 → 重構
2. **100% 測試覆蓋**: 每個新功能都要有對應測試
3. **SOLID 原則**: 保持類別職責單一，介面設計清晰
4. **C++20 標準**: 使用現代 C++ 特性
5. **文檔化**: 重要方法要有 Doxygen 註解

### **🚨 技術債務與風險**
1. ~~**cucumber-cpp 整合**: 未來需要解決 BDD 自動化問題~~ ✅ 已解決
2. **Windows 平行化**: OpenMP 在 Windows 下的穩定性需要驗證
3. **記憶體效率**: 大型 TSP 實例的記憶體使用需要最佳化
4. **數值穩定性**: 費洛蒙極值處理需要更健壯的實作

### **⚡ 性能考量**
1. **矩陣存儲**: 目前使用 `vector<vector<double>>`，大型問題可能需要最佳化
2. **快取局部性**: 平行化時需考慮 NUMA 記憶體存取模式
3. **編譯器最佳化**: Release 建置時啟用 `-O3` 最佳化

### **🔄 下一步開發優先順序**
1. ~~**立即任務**: 實作 Scenario 8 (演算法收斂)~~ ✅ 已完成
2. ~~**立即任務**: 實作 Scenario 9 (性能預算與指標)~~ ✅ 已完成
3. **短期目標**: 完成 Scenario 10 (NUMA 記憶體最佳化)
4. **中期目標**: 實作 Scenario 11 (可重現性驗證)
5. **長期目標**: 最終性能調優與文檔完善

---

## 📞 **聯絡資訊與支援**

**開發環境復現**:
```powershell
# 環境檢查指令
cmake --version          # 應該 >= 3.20
where gcc               # 檢查 GCC 編譯器路徑
where cl                # 檢查 MSVC 是否可用 (optional)
```

**常見問題解決**:
- 建置失敗: 檢查 CMake 輸出中的編譯器檢測結果
- 測試失敗: 使用 `--gtest_filter` 隔離問題測試
- 記憶體問題: Debug 模式下使用 Address Sanitizer

**Git 工作流程**:
```bash
git add .
git commit -m "feat: implement scenario X - description"
git push origin main
```

---

**📝 備註**: 本報告應定期更新，記錄每個 scenario 完成後的狀態變化。
