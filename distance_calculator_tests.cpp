#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <memory>

// 前向聲明
class DistanceCalculator;

// 測試框架
class TDDTest {
private:
    static int total_tests;
    static int passed_tests;
    
public:
    static void assert_equal(double expected, double actual, const std::string& test_name, double tolerance = 0.001) {
        total_tests++;
        if (std::abs(expected - actual) <= tolerance) {
            std::cout << "✅ PASS: " << test_name << " (expected: " << expected << ", actual: " << actual << ")" << std::endl;
            passed_tests++;
        } else {
            std::cout << "❌ FAIL: " << test_name << " (expected: " << expected << ", actual: " << actual << ")" << std::endl;
        }
    }
    
    static void assert_equal(const std::string& expected, const std::string& actual, const std::string& test_name) {
        total_tests++;
        if (expected == actual) {
            std::cout << "✅ PASS: " << test_name << " (expected: " << expected << ", actual: " << actual << ")" << std::endl;
            passed_tests++;
        } else {
            std::cout << "❌ FAIL: " << test_name << " (expected: " << expected << ", actual: " << actual << ")" << std::endl;
        }
    }
    
    static void assert_true(bool condition, const std::string& test_name) {
        total_tests++;
        if (condition) {
            std::cout << "✅ PASS: " << test_name << std::endl;
            passed_tests++;
        } else {
            std::cout << "❌ FAIL: " << test_name << std::endl;
        }
    }
    
    static void print_summary() {
        std::cout << "\n========== 測試總結 ==========" << std::endl;
        std::cout << "總測試數: " << total_tests << std::endl;
        std::cout << "通過測試: " << passed_tests << std::endl;
        std::cout << "失敗測試: " << (total_tests - passed_tests) << std::endl;
        std::cout << "通過率: " << (total_tests > 0 ? (passed_tests * 100.0 / total_tests) : 0) << "%" << std::endl;
    }
};

int TDDTest::total_tests = 0;
int TDDTest::passed_tests = 0;

// 距離計算器接口 (Strategy Pattern)
class DistanceCalculator {
public:
    virtual ~DistanceCalculator() = default;
    virtual double calculateDistance(double x1, double y1, double x2, double y2) = 0;
    virtual std::string getType() const = 0;
};

// EUC_2D 距離計算器 (待實現)
class EuclideanDistanceCalculator : public DistanceCalculator {
public:
    double calculateDistance(double x1, double y1, double x2, double y2) override {
        // TODO: 實現歐幾里得距離計算
        return 0.0; // 暫時返回 0，讓測試失敗
    }
    
    std::string getType() const override {
        return "EUC_2D";
    }
};

// CEIL_2D 距離計算器 (待實現)
class CeilingDistanceCalculator : public DistanceCalculator {
public:
    double calculateDistance(double x1, double y1, double x2, double y2) override {
        // TODO: 實現向上取整的歐幾里得距離
        return 0.0; // 暫時返回 0，讓測試失敗
    }
    
    std::string getType() const override {
        return "CEIL_2D";
    }
};

// ATT 距離計算器 (待實現)
class AttDistanceCalculator : public DistanceCalculator {
public:
    double calculateDistance(double x1, double y1, double x2, double y2) override {
        // TODO: 實現偽歐幾里得距離計算
        return 0.0; // 暫時返回 0，讓測試失敗
    }
    
    std::string getType() const override {
        return "ATT";
    }
};

// GEO 距離計算器 (待實現)
class GeographicalDistanceCalculator : public DistanceCalculator {
public:
    double calculateDistance(double x1, double y1, double x2, double y2) override {
        // TODO: 實現地理距離計算
        return 0.0; // 暫時返回 0，讓測試失敗
    }
    
    std::string getType() const override {
        return "GEO";
    }
};

// 距離計算器工廠 (待實現)
class DistanceCalculatorFactory {
public:
    static std::unique_ptr<DistanceCalculator> create(const std::string& type) {
        // TODO: 實現工廠方法
        return nullptr; // 暫時返回 nullptr，讓測試失敗
    }
};

// ========================== 測試用例 ==========================

void test_euclidean_distance_calculator() {
    std::cout << "\n--- 測試 EUC_2D 距離計算器 ---" << std::endl;
    
    EuclideanDistanceCalculator calculator;
    
    // 測試1: 簡單的 3-4-5 三角形
    double dist1 = calculator.calculateDistance(0, 0, 3, 4);
    TDDTest::assert_equal(5.0, dist1, "3-4-5 三角形距離");
    
    // 測試2: 相同點的距離應該為 0
    double dist2 = calculator.calculateDistance(1, 1, 1, 1);
    TDDTest::assert_equal(0.0, dist2, "相同點距離");
    
    // 測試3: 水平距離
    double dist3 = calculator.calculateDistance(0, 0, 10, 0);
    TDDTest::assert_equal(10.0, dist3, "水平距離");
    
    // 測試4: 垂直距離
    double dist4 = calculator.calculateDistance(0, 0, 0, 8);
    TDDTest::assert_equal(8.0, dist4, "垂直距離");
    
    // 測試5: TSPLIB 標準測試案例 - 需要四捨五入到最近整數
    double dist5 = calculator.calculateDistance(181.2, 625.4, 790.0, 368.7);
    TDDTest::assert_equal(661.0, dist5, "TSPLIB kroA100 案例", 1.0);
}

void test_ceiling_distance_calculator() {
    std::cout << "\n--- 測試 CEIL_2D 距離計算器 ---" << std::endl;
    
    CeilingDistanceCalculator calculator;
    
    // 測試1: 需要向上取整的情況
    double dist1 = calculator.calculateDistance(0, 0, 1.1, 1.1);
    // sqrt(1.1^2 + 1.1^2) = sqrt(2.42) ≈ 1.556，向上取整為 2
    TDDTest::assert_equal(2.0, dist1, "向上取整測試");
    
    // 測試2: 整數結果不變
    double dist2 = calculator.calculateDistance(0, 0, 3, 4);
    TDDTest::assert_equal(5.0, dist2, "整數結果");
}

void test_att_distance_calculator() {
    std::cout << "\n--- 測試 ATT 距離計算器 ---" << std::endl;
    
    AttDistanceCalculator calculator;
    
    // 測試 ATT 偽歐幾里得距離
    // 公式: rij = sqrt(((xi-xj)^2 + (yi-yj)^2)/10.0)
    //       tij = round(rij)
    //       if tij < rij then distance = tij + 1 else distance = tij
    
    double dist1 = calculator.calculateDistance(0, 0, 10, 0);
    // rij = sqrt(100/10) = sqrt(10) ≈ 3.162
    // tij = 3, 3 < 3.162, so distance = 4
    TDDTest::assert_equal(4.0, dist1, "ATT 距離測試");
}

void test_geographical_distance_calculator() {
    std::cout << "\n--- 測試 GEO 距離計算器 ---" << std::endl;
    
    GeographicalDistanceCalculator calculator;
    
    // 測試地理距離計算 (使用經緯度)
    // 北京 (39.9042° N, 116.4074° E) 到 上海 (31.2304° N, 121.4737° E)
    // 大約 1068 公里
    double dist1 = calculator.calculateDistance(39.9042, 116.4074, 31.2304, 121.4737);
    TDDTest::assert_equal(1068.0, dist1, "北京到上海地理距離", 50.0); // 允許 50km 誤差
}

void test_distance_calculator_factory() {
    std::cout << "\n--- 測試距離計算器工廠 ---" << std::endl;
    
    // 測試創建 EUC_2D 計算器
    auto calc1 = DistanceCalculatorFactory::create("EUC_2D");
    TDDTest::assert_true(calc1 != nullptr, "創建 EUC_2D 計算器");
    if (calc1) {
        TDDTest::assert_equal("EUC_2D", calc1->getType(), "EUC_2D 類型驗證");
    }
    
    // 測試創建 CEIL_2D 計算器
    auto calc2 = DistanceCalculatorFactory::create("CEIL_2D");
    TDDTest::assert_true(calc2 != nullptr, "創建 CEIL_2D 計算器");
    
    // 測試創建 ATT 計算器
    auto calc3 = DistanceCalculatorFactory::create("ATT");
    TDDTest::assert_true(calc3 != nullptr, "創建 ATT 計算器");
    
    // 測試創建 GEO 計算器
    auto calc4 = DistanceCalculatorFactory::create("GEO");
    TDDTest::assert_true(calc4 != nullptr, "創建 GEO 計算器");
    
    // 測試無效類型
    auto calc5 = DistanceCalculatorFactory::create("INVALID");
    TDDTest::assert_true(calc5 == nullptr, "無效類型返回 nullptr");
}

void test_integration_with_known_instances() {
    std::cout << "\n--- 測試與已知實例的集成 ---" << std::endl;
    
    // 測試 eil51 (EUC_2D)
    auto eucCalc = DistanceCalculatorFactory::create("EUC_2D");
    if (eucCalc) {
        // eil51 的前幾個城市座標和距離
        double dist = eucCalc->calculateDistance(37, 52, 49, 49);
        TDDTest::assert_true(dist > 0, "eil51 距離計算");
    }
    
    // 測試 gr202 (GEO) - 這應該與我們之前的錯誤結果不同
    auto geoCalc = DistanceCalculatorFactory::create("GEO");
    if (geoCalc) {
        double dist = geoCalc->calculateDistance(37.44, -25.40, 38.43, -9.08);
        TDDTest::assert_true(dist > 1000, "gr202 地理距離應該很大");
    }
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   TDD 距離計算器測試套件" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // 運行所有測試 (預期會失敗，因為功能尚未實現)
    test_euclidean_distance_calculator();
    test_ceiling_distance_calculator();
    test_att_distance_calculator();
    test_geographical_distance_calculator();
    test_distance_calculator_factory();
    test_integration_with_known_instances();
    
    TDDTest::print_summary();
    
    std::cout << "\n下一步: 實現各個距離計算器以使測試通過" << std::endl;
    
    return 0;
}
