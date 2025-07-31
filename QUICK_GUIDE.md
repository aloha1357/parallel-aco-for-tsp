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

**期望結果**: 應該看到 `[  PASSED  ] 22 tests.`

## 🎯 當前開發重點

**下一個任務**: 實作 **Scenario 4 - 費洛蒙蒸發**

```cpp
// 需要在 PheromoneModel.hpp 中添加:
void evaporate(double rho);  // τ(i,j) ← (1-ρ) · τ(i,j)
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
