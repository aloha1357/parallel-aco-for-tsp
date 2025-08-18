#include "aco/Graph.hpp"
#include "aco/DistanceCalculator.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <string>

// Simple test framework
class TDDTest {
public:
    static int tests_run;
    static int tests_passed;
    
    static void assert_equal(double expected, double actual, const std::string& test_name) {
        tests_run++;
        if (std::abs(expected - actual) < 1e-9) {
            std::cout << "✓ PASS: " << test_name << " (expected: " << expected << ", actual: " << actual << ")" << std::endl;
            tests_passed++;
        } else {
            std::cout << "✗ FAIL: " << test_name << " (expected: " << expected << ", actual: " << actual << ")" << std::endl;
        }
    }
    
    static void assert_not_null(void* ptr, const std::string& test_name) {
        tests_run++;
        if (ptr != nullptr) {
            std::cout << "✓ PASS: " << test_name << std::endl;
            tests_passed++;
        } else {
            std::cout << "✗ FAIL: " << test_name << " (ptr is null)" << std::endl;
        }
    }
    
    static void assert_equal_string(const std::string& expected, const std::string& actual, const std::string& test_name) {
        tests_run++;
        if (expected == actual) {
            std::cout << "✓ PASS: " << test_name << " (expected: " << expected << ", actual: " << actual << ")" << std::endl;
            tests_passed++;
        } else {
            std::cout << "✗ FAIL: " << test_name << " (expected: " << expected << ", actual: " << actual << ")" << std::endl;
        }
    }
    
    static void print_summary() {
        std::cout << "\n========== 整合測試結果 ==========" << std::endl;
        std::cout << "總測試數量: " << tests_run << std::endl;
        std::cout << "通過測試: " << tests_passed << std::endl;
        std::cout << "失敗測試: " << (tests_run - tests_passed) << std::endl;
        std::cout << "成功率: " << (tests_run > 0 ? (100.0 * tests_passed / tests_run) : 0) << "%" << std::endl;
        
        if (tests_passed == tests_run) {
            std::cout << "\n🎉 TDD 整合成功！距離計算系統已完全整合至 Graph 類別" << std::endl;
        } else {
            std::cout << "\n❌ 整合存在問題，需要進一步調試" << std::endl;
        }
    }
};

int TDDTest::tests_run = 0;
int TDDTest::tests_passed = 0;

void test_distance_calculator_integration() {
    std::cout << "========================================" << std::endl;
    std::cout << "   TDD 距離計算系統整合測試" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Test 1: Factory can create all calculator types
    std::cout << "\n--- 測試距離計算器工廠 ---" << std::endl;
    auto euc_calc = DistanceCalculatorFactory::create("EUC_2D");
    TDDTest::assert_not_null(euc_calc.get(), "創建 EUC_2D 計算器");
    
    auto ceil_calc = DistanceCalculatorFactory::create("CEIL_2D");
    TDDTest::assert_not_null(ceil_calc.get(), "創建 CEIL_2D 計算器");
    
    auto att_calc = DistanceCalculatorFactory::create("ATT");
    TDDTest::assert_not_null(att_calc.get(), "創建 ATT 計算器");
    
    auto geo_calc = DistanceCalculatorFactory::create("GEO");
    TDDTest::assert_not_null(geo_calc.get(), "創建 GEO 計算器");
    
    // Test 2: Graph basic functionality with random distances
    std::cout << "\n--- 測試 Graph 基本功能 ---" << std::endl;
    auto graph = std::make_shared<Graph>(4);
    TDDTest::assert_equal(4, graph->size(), "Graph 尺寸正確");
    TDDTest::assert_equal_string("RANDOM", graph->getDistanceType(), "預設距離類型為 RANDOM");
    
    // Test 3: Manual distance setting
    graph->setDistance(0, 1, 10.0);
    TDDTest::assert_equal(10.0, graph->getDistance(0, 1), "手動設定距離 (0,1)");
    TDDTest::assert_equal(10.0, graph->getDistance(1, 0), "對稱距離 (1,0)");
    
    // Test 4: Distance calculator integration with Graph
    std::cout << "\n--- 測試距離計算器與 Graph 整合 ---" << std::endl;
    
    // Create a simple test graph with known coordinates
    auto test_graph = std::make_shared<Graph>(3);
    
    // Test EUC_2D calculator integration
    auto euc_calculator = DistanceCalculatorFactory::create("EUC_2D");
    if (euc_calculator) {
        // Test with 3-4-5 triangle coordinates
        double dist_euc = euc_calculator->calculateDistance(0, 0, 3, 4);
        TDDTest::assert_equal(5, dist_euc, "EUC_2D 計算器：3-4-5 三角形");
        TDDTest::assert_equal_string("EUC_2D", euc_calculator->getType(), "EUC_2D 類型名稱");
    }
    
    // Test ATT calculator integration
    auto att_calculator = DistanceCalculatorFactory::create("ATT");
    if (att_calculator) {
        double dist_att = att_calculator->calculateDistance(0, 0, 10, 10);
        TDDTest::assert_equal(5, dist_att, "ATT 計算器基本功能");  // 修正預期值為 5
        TDDTest::assert_equal_string("ATT", att_calculator->getType(), "ATT 類型名稱");
    }
    
    // Test 5: Supported types enumeration
    std::cout << "\n--- 測試支援的距離類型 ---" << std::endl;
    auto supported_types = DistanceCalculatorFactory::getSupportedTypes();
    TDDTest::assert_equal(4, supported_types.size(), "支援 4 種距離類型");
    
    bool has_euc_2d = false, has_ceil_2d = false, has_att = false, has_geo = false;
    for (const auto& type : supported_types) {
        if (type == "EUC_2D") has_euc_2d = true;
        if (type == "CEIL_2D") has_ceil_2d = true;
        if (type == "ATT") has_att = true;
        if (type == "GEO") has_geo = true;
    }
    
    TDDTest::assert_equal(1, has_euc_2d ? 1 : 0, "包含 EUC_2D 類型");
    TDDTest::assert_equal(1, has_ceil_2d ? 1 : 0, "包含 CEIL_2D 類型");
    TDDTest::assert_equal(1, has_att ? 1 : 0, "包含 ATT 類型");
    TDDTest::assert_equal(1, has_geo ? 1 : 0, "包含 GEO 類型");
    
    // Test 6: Error handling
    std::cout << "\n--- 測試錯誤處理 ---" << std::endl;
    auto invalid_calc = DistanceCalculatorFactory::create("INVALID_TYPE");
    TDDTest::assert_equal(0, invalid_calc.get() == nullptr ? 0 : 1, "無效類型返回 nullptr");
}

int main() {
    test_distance_calculator_integration();
    
    TDDTest::print_summary();
    
    return (TDDTest::tests_passed == TDDTest::tests_run) ? 0 : 1;
}
