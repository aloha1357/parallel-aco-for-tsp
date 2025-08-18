#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <iomanip>

// 簡化的 Graph 類
class TestGraph {
private:
    std::vector<std::vector<double>> distances_;
    int size_;
    
public:
    TestGraph(int n) : size_(n) {
        distances_.resize(n);
        for (int i = 0; i < n; ++i) {
            distances_[i].resize(n, 0.0);
        }
    }
    
    void setDistance(int from, int to, double dist) {
        distances_[from][to] = dist;
        distances_[to][from] = dist;
    }
    
    double getDistance(int from, int to) const {
        return distances_[from][to];
    }
    
    int size() const { return size_; }
    
    // EUC_2D 距離計算
    static int calculateEuclideanDistance(double x1, double y1, double x2, double y2) {
        double d = std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
        return static_cast<int>(std::round(d));
    }
    
    static std::shared_ptr<TestGraph> fromTSPFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "無法打開文件: " << filename << std::endl;
            return nullptr;
        }
        
        std::string line;
        int dimension = 0;
        std::vector<std::pair<double, double>> coordinates;
        
        // 解析標頭
        while (std::getline(file, line)) {
            if (line.find("DIMENSION") != std::string::npos) {
                // 處理多種格式: "DIMENSION: 100", "DIMENSION : 100", "DIMENSION 100"
                size_t pos = line.find(":");
                if (pos != std::string::npos) {
                    dimension = std::stoi(line.substr(pos + 1));
                } else {
                    std::istringstream iss(line);
                    std::string key;
                    iss >> key >> dimension;
                }
            } else if (line.find("NODE_COORD_SECTION") != std::string::npos) {
                break;
            }
        }
        
        if (dimension <= 0) {
            std::cerr << "無效的維度: " << dimension << std::endl;
            return nullptr;
        }
        
        // 讀取座標
        coordinates.resize(dimension);
        for (int i = 0; i < dimension; ++i) {
            if (!std::getline(file, line) || line == "EOF") break;
            std::istringstream iss(line);
            int city_id;
            double x, y;
            if (iss >> city_id >> x >> y) {
                coordinates[city_id - 1] = {x, y};
            }
        }
        
        // 創建圖並計算距離
        auto graph = std::make_shared<TestGraph>(dimension);
        for (int i = 0; i < dimension; ++i) {
            for (int j = i + 1; j < dimension; ++j) {
                double x1 = coordinates[i].first, y1 = coordinates[i].second;
                double x2 = coordinates[j].first, y2 = coordinates[j].second;
                int dist = calculateEuclideanDistance(x1, y1, x2, y2);
                graph->setDistance(i, j, static_cast<double>(dist));
            }
        }
        
        return graph;
    }
};

// 修復後的 Tour 類
class FixedTour {
private:
    std::shared_ptr<TestGraph> graph_;
    std::vector<int> path_;
    double length_;
    
    void calculateLength() {
        if (path_.size() < 2) {
            length_ = 0.0;
            return;
        }
        
        length_ = 0.0;
        
        // 計算連續城市間的距離
        for (size_t i = 0; i + 1 < path_.size(); ++i) {
            length_ += graph_->getDistance(path_[i], path_[i + 1]);
        }
        
        // 修復：只有當路徑未形成完整循環時才補上返回邊
        if (path_.size() >= 2 && path_.back() != path_.front()) {
            length_ += graph_->getDistance(path_.back(), path_.front());
        }
    }
    
public:
    FixedTour(std::shared_ptr<TestGraph> graph) : graph_(graph), length_(0.0) {}
    
    void setPath(const std::vector<int>& path) {
        path_ = path;
        calculateLength();
    }
    
    double getLength() const { return length_; }
    const std::vector<int>& getPath() const { return path_; }
};

// 讀取最優解
std::vector<int> readOptimalTour(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "無法打開最優解文件: " << filename << std::endl;
        return {};
    }
    
    std::string line;
    std::vector<int> tour;
    
    // 跳過標頭直到 TOUR_SECTION
    while (std::getline(file, line)) {
        if (line.find("TOUR_SECTION") != std::string::npos) {
            break;
        }
    }
    
    // 讀取路徑
    while (std::getline(file, line)) {
        if (line == "-1" || line == "EOF") break;
        try {
            int city = std::stoi(line);
            tour.push_back(city - 1); // 轉換為 0-based
        } catch (const std::exception& e) {
            // 跳過無效行
            continue;
        }
    }
    
    return tour;
}

// 測試單個實例
struct TestResult {
    std::string instance_name;
    int city_count;
    double expected_optimal;
    double calculated_optimal;
    double tour_without_return;
    double tour_with_return;
    bool length_consistency;
    double error_percentage;
    bool is_valid;
};

TestResult testInstance(const std::string& instance_name, double expected_optimal) {
    TestResult result;
    result.instance_name = instance_name;
    result.expected_optimal = expected_optimal;
    result.is_valid = false;
    
    std::string tsp_file = "data/" + instance_name + ".tsp";
    std::string opt_file = "data/" + instance_name + ".opt.tour";
    
    std::cout << "\n=== 測試 " << instance_name << " ===" << std::endl;
    
    // 加載圖
    auto graph = TestGraph::fromTSPFile(tsp_file);
    if (!graph) {
        std::cout << "❌ 無法加載 TSP 文件" << std::endl;
        return result;
    }
    
    result.city_count = graph->size();
    std::cout << "城市數量: " << result.city_count << std::endl;
    
    // 讀取最優解
    auto optimal_tour = readOptimalTour(opt_file);
    if (optimal_tour.empty()) {
        std::cout << "❌ 無法讀取最優解文件" << std::endl;
        return result;
    }
    
    std::cout << "最優路徑包含 " << optimal_tour.size() << " 個城市" << std::endl;
    
    // 測試1：不含顯式返回的路徑
    FixedTour tour1(graph);
    tour1.setPath(optimal_tour);
    result.tour_without_return = tour1.getLength();
    
    // 測試2：含顯式返回的路徑
    std::vector<int> tour_with_return = optimal_tour;
    tour_with_return.push_back(optimal_tour.front());
    
    FixedTour tour2(graph);
    tour2.setPath(tour_with_return);
    result.tour_with_return = tour2.getLength();
    
    // 檢查一致性
    result.length_consistency = (std::abs(result.tour_without_return - result.tour_with_return) < 0.001);
    result.calculated_optimal = result.tour_without_return;
    result.error_percentage = std::abs(result.calculated_optimal - result.expected_optimal) / result.expected_optimal * 100.0;
    result.is_valid = true;
    
    std::cout << "期望最優長度: " << std::fixed << std::setprecision(0) << result.expected_optimal << std::endl;
    std::cout << "計算最優長度: " << std::fixed << std::setprecision(0) << result.calculated_optimal << std::endl;
    std::cout << "不含返回路徑: " << std::fixed << std::setprecision(0) << result.tour_without_return << std::endl;
    std::cout << "含返回路徑:   " << std::fixed << std::setprecision(0) << result.tour_with_return << std::endl;
    std::cout << "長度一致性:   " << (result.length_consistency ? "✅ 是" : "❌ 否") << std::endl;
    std::cout << "誤差百分比:   " << std::fixed << std::setprecision(2) << result.error_percentage << "%" << std::endl;
    
    if (result.error_percentage < 0.1) {
        std::cout << "✅ 結果正確！" << std::endl;
    } else {
        std::cout << "⚠️  結果有誤差" << std::endl;
    }
    
    return result;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  TSP 修復驗證 - 四個標準測試實例" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // 測試實例及其期望最優值
    std::vector<std::pair<std::string, double>> test_instances = {
        {"eil51", 426},
        {"kroA100", 21282},
        {"gr202", 40160},
        {"ch150", 6528}
    };
    
    std::vector<TestResult> results;
    
    for (const auto& instance : test_instances) {
        TestResult result = testInstance(instance.first, instance.second);
        results.push_back(result);
    }
    
    // 總結報告
    std::cout << "\n========================================" << std::endl;
    std::cout << "              總結報告" << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::cout << std::left << std::setw(12) << "實例名稱" 
              << std::setw(8) << "城市數" 
              << std::setw(12) << "期望值" 
              << std::setw(12) << "計算值" 
              << std::setw(8) << "誤差%" 
              << std::setw(10) << "一致性" 
              << "狀態" << std::endl;
    std::cout << "------------------------------------------------------------------------" << std::endl;
    
    int valid_count = 0;
    int consistent_count = 0;
    int accurate_count = 0;
    
    for (const auto& result : results) {
        if (result.is_valid) {
            valid_count++;
            if (result.length_consistency) consistent_count++;
            if (result.error_percentage < 0.1) accurate_count++;
            
            std::cout << std::left << std::setw(12) << result.instance_name
                      << std::setw(8) << result.city_count
                      << std::setw(12) << std::fixed << std::setprecision(0) << result.expected_optimal
                      << std::setw(12) << std::fixed << std::setprecision(0) << result.calculated_optimal
                      << std::setw(8) << std::fixed << std::setprecision(2) << result.error_percentage
                      << std::setw(10) << (result.length_consistency ? "✅" : "❌")
                      << (result.error_percentage < 0.1 ? "✅ 正確" : "⚠️ 有誤差") << std::endl;
        } else {
            std::cout << std::left << std::setw(12) << result.instance_name
                      << std::setw(50) << "❌ 測試失敗" << std::endl;
        }
    }
    
    std::cout << "\n修復效果評估:" << std::endl;
    std::cout << "- 成功測試的實例: " << valid_count << "/" << test_instances.size() << std::endl;
    std::cout << "- 長度計算一致性: " << consistent_count << "/" << valid_count << std::endl;
    std::cout << "- 結果準確性:     " << accurate_count << "/" << valid_count << std::endl;
    
    if (consistent_count == valid_count) {
        std::cout << "\n🎉 修復完全成功！所有測試實例的長度計算都保持一致。" << std::endl;
    } else {
        std::cout << "\n⚠️  仍有部分問題需要解決。" << std::endl;
    }
    
    return 0;
}
