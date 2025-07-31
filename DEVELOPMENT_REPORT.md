# Parallel ACO for TSP - 開發進度報告

**日期**: 2025年7月31日  
**版本**: v0.3-dev  
**開發狀態**: BDD Scenario 1-3 完成，核心 ACO 機制實作完成，測試框架優化完成

---

## 📊 **目前開發進度**

### ✅ **已完成的 BDD Scenarios**

| Scenario | 功能 | 狀態 | 測試數量 |
|----------|------|------|----------|
| **01_walking_skeleton** | 建置環境驗證 | ✅ 完成 | 5 tests |
| **02_construct_tour** | 基礎路徑構建 | ✅ 完成 | 15 tests |
| **03_probabilistic_choice** | ACO 機率選擇 | ✅ 完成 | 22 tests |

### 🎯 **待實作的 Scenarios**
- `04_evaporation.feature` - 費洛蒙蒸發機制
- `05_delta_accumulation.feature` - 費洛蒙累積
- `06_delta_merge.feature` - 平行費洛蒙合併
- `07_parallel_consistency.feature` - OpenMP 平行化
- `08_convergence.feature` - 演算法收斂
- `09_performance_budget.feature` - 性能指標
- `10_numa_awareness.feature` - NUMA 最佳化
- `11_reproducibility.feature` - 可重現性

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
│   └── AcoEngine.hpp           # ACO 引擎類別
├── 📁 src/aco/                 # 實作檔目錄
│   ├── Graph.cpp               # 圖形實作
│   ├── Tour.cpp                # 路徑實作  
│   ├── Ant.cpp                 # 螞蟻實作
│   ├── PheromoneModel.cpp      # 費洛蒙實作
│   └── AcoEngine.cpp           # 引擎實作
├── 📁 tests/                   # 測試目錄
│   ├── features/               # BDD feature 檔案
│   ├── steps/                  # BDD step definitions
│   └── unit/                   # 單元測試
├── 📁 data/                    # TSP 資料檔
├── CMakeLists.txt              # 建置配置檔
├── vcpkg.json                  # 相依性管理
└── README.md                   # 專案說明
```

### **核心類別架構**

```mermaid
classDiagram
    class Graph {
        -distances_: vector<vector<double>>
        -size_: int
        +Graph(int size)
        +getDistance(int from, int to): double
        +setDistance(int from, int to, double distance)
        +size(): int
        +initializeRandomSymmetric(seed)
    }
    
    class PheromoneModel {
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
# 運行所有單元測試
.\unit_tests.exe

# 運行特定測試群組
.\unit_tests.exe --gtest_filter="GraphTest.*"
.\unit_tests.exe --gtest_filter="ProbabilisticChoiceTest.*"

# 使用 CTest 運行測試
ctest --verbose

# 只運行快速測試
ctest -L quick
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

### **Problem 1: cucumber-cpp 依賴問題**
**問題**: Windows 環境下 cucumber-cpp 安裝困難  
**解決方案**: 
- 暫時跳過 BDD 自動化測試
- 使用 GoogleTest 實作相同的測試邏輯
- 保留 `.feature` 檔案作為規格文檔

**相關檔案**:
```
tests/steps/probabilistic_choice_steps.cpp  # 使用 GoogleTest 而非 cucumber-cpp
CMakeLists.txt                              # BUILD_BDD_TESTS 設為 OFF
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

---

## 🧪 **測試策略與覆蓋範圍**

### **測試金字塔結構**
1. **單元測試** (Unit Tests) - 15 個
   - Graph 基本功能測試
   - Tour 路徑計算測試
   - PheromoneModel 費洛蒙操作測試
   - Ant 基本構造測試

2. **組件測試** (Component Tests) - 7 個  
   - 機率選擇邏輯測試
   - ACO 規則驗證測試

3. **整合測試** (Integration Tests) - 待實作
   - 完整 ACO 演算法測試
   - 平行化正確性測試

### **測試覆蓋現況**
- **總測試數**: 22 個
- **通過率**: 100% (22/22)
- **核心類別覆蓋**: 100%
- **ACO 核心邏輯覆蓋**: 機率選擇完成，蒸發與更新待測

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

#### **3. 費洛蒙模型基礎**
- ✅ 費洛蒙矩陣初始化
- ✅ 費洛蒙讀取/設置
- ✅ 最小費洛蒙限制

### 🔄 **待實作功能**

#### **1. 費洛蒙蒸發 (Evaporation)**
```cpp
// 待實作: τ(i,j) ← (1-ρ) · τ(i,j)
void PheromoneModel::evaporate(double rho)
```

#### **2. 費洛蒙沉積 (Deposition)**  
```cpp
// 待實作: τ(i,j) ← τ(i,j) + Δτ(i,j)
void PheromoneModel::deposit(const Tour& tour, double quality)
```

#### **3. 平行化 (OpenMP)**
```cpp
// 待實作: 平行螞蟻構建 + 執行緒安全費洛蒙更新
#pragma omp parallel for
for (int ant = 0; ant < num_ants; ++ant) {
    // 平行建構路徑
}
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
1. **cucumber-cpp 整合**: 未來需要解決 BDD 自動化問題
2. **Windows 平行化**: OpenMP 在 Windows 下的穩定性需要驗證
3. **記憶體效率**: 大型 TSP 實例的記憶體使用需要最佳化
4. **數值穩定性**: 費洛蒙極值處理需要更健壯的實作

### **⚡ 性能考量**
1. **矩陣存儲**: 目前使用 `vector<vector<double>>`，大型問題可能需要最佳化
2. **快取局部性**: 平行化時需考慮 NUMA 記憶體存取模式
3. **編譯器最佳化**: Release 建置時啟用 `-O3` 最佳化

### **🔄 下一步開發優先順序**
1. **立即任務**: 實作 Scenario 4 (費洛蒙蒸發)
2. **短期目標**: 完成 Scenarios 4-6 (基礎 ACO 循環)
3. **中期目標**: 實作 Scenario 7 (OpenMP 平行化)
4. **長期目標**: 性能調優與 NUMA 最佳化

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
