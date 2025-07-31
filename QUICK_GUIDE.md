# 🚀 Quick Developer Guide

> **新開發者快速上手指南** - 5分鐘內開始開發

## ⚡ 快速驗證環境

```powershell
# 1. 驗證必要工具
cmake --version          # 需要 >= 3.20
where gcc               # 確認 GCC 編譯器可用

# 2. 一鍵建置測試
cd "d:\D_backup\2025\tum\Parallel ACO for TSP"
cd build
cmake .. && cmake --build . && .\unit_tests.exe
```

**期望結果**: 應該看到 `[  PASSED  ] 70 tests.`、`[  SKIPPED ] 1 test.` 和 `[  FAILED  ] 2 tests.` (Convergence調整中)

## 🧪 測試框架現況

**✅ 已實現**: 純 GoogleTest 統一方案
- **73 個測試**: 70 個通過 + 1 個未來功能佔位符 + 2 個調整中
- **零外部依賴**: 無需 cucumber-cpp，建置更穩定
- **BDD 風格**: 測試命名保持可讀性

```powershell
# 運行所有測試
.\unit_tests.exe

# 運行 BDD 場景測試
.\unit_tests.exe --gtest_filter="BDDScenariosTest.*"

# 運行 Convergence 測試 (調整中)
.\unit_tests.exe --gtest_filter="*Convergence*"

# 列出所有測試
.\unit_tests.exe --gtest_list_tests
```

## 🎯 當前開發重點

**下一個任務**: 實作 **Scenario 9 - 性能最佳化**

```cpp
// 需要實作性能監控和最佳化機制:
struct PerformanceBudget; // 性能預算定義
class PerformanceMonitor; // 性能監控器
void optimizeForPerformance(); // 性能最佳化
```

## 🔄 BDD 開發循環

### 1️⃣ 紅燈階段 (Red)
```powershell
# 建立測試，確保失敗
cd tests/unit/
# 編寫測試邏輯，運行應該失敗
.\unit_tests.exe --gtest_filter="EvaporationTest.*"
```

### 2️⃣ 綠燈階段 (Green)  
```cpp
// 實作最小功能讓測試通過
void PheromoneModel::evaporate(double rho) {
    // 最簡實作
}
```

### 3️⃣ 重構階段 (Refactor)
```cpp
// 改善代碼品質，保持測試通過
// 添加文檔註解、錯誤處理、最佳化
```

## 📊 測試分類說明

### **單元測試** (49 tests)
- `GraphTest.*` - 圖形距離矩陣功能 (7 tests) ✅ 包含TSP文件加載
- `TourTest.*` - 路徑表示與計算 (5 tests)  
- `PheromoneModelTest.*` - 費洛蒙操作 (19 tests) ✅ 完整實作
- `ThreadLocalPheromoneModelTest.*` - 線程本地費洛蒙 (9 tests) ✅ 平行支援
- `AntTest.*` - 螞蟻代理功能 (3 tests)
- `AcoEngineTest.*` - ACO 引擎 (3 tests) ✅ 完整引擎

### **BDD 場景測試** (23 tests)
- `BDDScenariosTest.WalkingSkeleton_*` - 基礎驗證 (1 test)
- `BDDScenariosTest.ConstructTour_*` - 路徑構建場景 (6 tests)
- `BDDScenariosTest.Evaporation_*` - 費洛蒙蒸發場景 (5 tests)
- `BDDScenariosTest.DeltaAccumulation_*` - 費洛蒙累積場景 (2 tests)
- `BDDScenariosTest.DeltaMerge_*` - 平行費洛蒙合併場景 (4 tests)
- `BDDScenariosTest.ParallelConsistency_*` - 平行一致性場景 (4 tests)
- `BDDScenariosTest.Convergence_*` - 演算法收斂場景 (4 tests, 2 調整中)

### **未來功能佔位符** (1 skipped)
- `BDDScenariosTest.ProbabilisticChoice_*` - 進階機率選擇佔位符

## 📁 快速檔案導航

| 需求 | 檔案位置 |
|------|----------|
| **新增標頭檔** | `include/aco/*.hpp` |
| **實作功能** | `src/aco/*.cpp` |
| **單元測試** | `tests/unit/test_*.cpp` |
| **BDD 規格** | `tests/features/*.feature` |
| **建置配置** | `CMakeLists.txt` |

## 🧪 測試命令速查

```powershell
# 運行所有測試
.\unit_tests.exe

# 運行特定測試群組
.\unit_tests.exe --gtest_filter="GraphTest.*"
.\unit_tests.exe --gtest_filter="PheromoneModelTest.*"
.\unit_tests.exe --gtest_filter="AntTest.*"

# 測試特定功能
.\unit_tests.exe --gtest_filter="*Evaporation*"
.\unit_tests.exe --gtest_filter="*Probabilistic*"

# 顯示測試詳情
.\unit_tests.exe --gtest_list_tests
```

## 🚨 常見問題快速解決

### 建置失敗
```powershell
# 清理重建
Remove-Item * -Recurse -Force
cmake ..
cmake --build .
```

### 測試失敗
```powershell
# 隔離問題測試
.\unit_tests.exe --gtest_filter="FailingTestName"
```

### 編譯錯誤
```powershell
# 檢查 CMake 配置輸出
cmake .. --debug-output
```

## 📋 提交代碼清單

```bash
# 開發完成後
git add .
git commit -m "feat: implement scenario 4 - pheromone evaporation"

# 更新測試計數
git commit -m "test: add evaporation tests (now X/X passing)"

# 提交重構
git commit -m "refactor: improve pheromone model documentation"
```

## 🔍 代碼品質檢查

```cpp
// ✅ 每個 public 方法都要有文檔註解
/**
 * @brief Brief description
 * @param param Description
 * @return Return description
 */

// ✅ 錯誤處理
if (invalid_input) {
    throw std::invalid_argument("Descriptive error message");
}

// ✅ 測試斷言要有描述訊息
EXPECT_EQ(actual, expected) << "Meaningful error message";
```

## 📞 需要幫助？

1. **查看詳細報告**: [`DEVELOPMENT_REPORT.md`](./DEVELOPMENT_REPORT.md)
2. **檢查 feature 規格**: `tests/features/`
3. **參考現有實作**: `src/aco/` 中的已完成類別

---

**💡 提示**: 遵循現有的代碼模式，保持一致的風格和錯誤處理方式。
