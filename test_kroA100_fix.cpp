#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>

// 簡化的 Graph 類來重現問題
class SimpleGraphForTest {
private:
    std::vector<std::vector<double>> distances_;
    int size_;
    
public:
    SimpleGraphForTest(int n) : size_(n) {
        distances_.resize(n);
        for (int i = 0; i < n; ++i) {
            distances_[i].resize(n, 0.0);
        }
    }
    
    void setDistance(int from, int to, double dist) {
        distances_[from][to] = dist;
        distances_[to][from] = dist; // 對稱
    }
    
    double getDistance(int from, int to) const {
        return distances_[from][to];
    }
    
    int size() const { return size_; }
    
    // 從文件加載 TSP 數據
    static std::shared_ptr<SimpleGraphForTest> fromTSPFile(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;
        int dimension = 0;
        std::vector<std::pair<double, double>> coordinates;
        
        // 解析標頭
        while (std::getline(file, line)) {
            if (line.find("DIMENSION") != std::string::npos) {
                std::istringstream iss(line);
                std::string key;
                iss >> key; // "DIMENSION" 或 "DIMENSION:"
                if (key == "DIMENSION:") {
                    iss >> dimension;
                } else if (key == "DIMENSION") {
                    iss >> dimension; // 可能直接跟著數字，或者下一個是冒號
                    if (dimension == 0) { // 說明下一個token是冒號
                        std::string colon;
                        iss >> colon >> dimension;
                    }
                }
            } else if (line.find("NODE_COORD_SECTION") != std::string::npos) {
                break;
            }
        }
        
        if (dimension <= 0) {
            std::cerr << "Error: Invalid dimension in TSP file" << std::endl;
            return nullptr;
        }
        
        // 讀取座標
        coordinates.resize(dimension);
        for (int i = 0; i < dimension; ++i) {
            if (!std::getline(file, line)) break;
            std::istringstream iss(line);
            int city_id;
            double x, y;
            if (iss >> city_id >> x >> y) {
                coordinates[city_id - 1] = {x, y};
            }
        }
        
        // 創建圖並計算距離
        auto graph = std::make_shared<SimpleGraphForTest>(dimension);
        for (int i = 0; i < dimension; ++i) {
            for (int j = i + 1; j < dimension; ++j) {
                double dx = coordinates[i].first - coordinates[j].first;
                double dy = coordinates[i].second - coordinates[j].second;
                double dist = std::round(std::sqrt(dx * dx + dy * dy));
                graph->setDistance(i, j, dist);
            }
        }
        
        return graph;
    }
};

// 修復後的 Tour 類
class FixedTour {
private:
    std::shared_ptr<SimpleGraphForTest> graph_;
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
    FixedTour(std::shared_ptr<SimpleGraphForTest> graph) : graph_(graph), length_(0.0) {}
    
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
        int city = std::stoi(line);
        tour.push_back(city - 1); // 轉換為 0-based
    }
    
    return tour;
}

int main() {
    std::cout << "=== 測試修復後的 Tour 長度計算 ===" << std::endl;
    
    // 加載 kroA100
    auto graph = SimpleGraphForTest::fromTSPFile("data/kroA100.tsp");
    if (!graph) {
        std::cerr << "無法加載 kroA100.tsp" << std::endl;
        return 1;
    }
    
    // 讀取最優解
    auto optimal_tour = readOptimalTour("data/kroA100.opt.tour");
    std::cout << "加載了包含 " << optimal_tour.size() << " 個城市的最優路徑" << std::endl;
    
    // 測試1：不含顯式返回的路徑
    FixedTour tour1(graph);
    tour1.setPath(optimal_tour);
    
    std::cout << "\n測試1 - 不含顯式返回的路徑:" << std::endl;
    std::cout << "起始城市: " << optimal_tour.front() << ", 結束城市: " << optimal_tour.back() << std::endl;
    std::cout << "計算長度: " << tour1.getLength() << std::endl;
    
    // 測試2：含顯式返回的路徑（類似 Ant 構建的結果）
    std::vector<int> tour_with_return = optimal_tour;
    tour_with_return.push_back(optimal_tour.front()); // 添加返回起點
    
    FixedTour tour2(graph);
    tour2.setPath(tour_with_return);
    
    std::cout << "\n測試2 - 含顯式返回的路徑:" << std::endl;
    std::cout << "起始城市: " << tour_with_return.front() << ", 結束城市: " << tour_with_return.back() << std::endl;
    std::cout << "計算長度: " << tour2.getLength() << std::endl;
    
    std::cout << "\n結果比較:" << std::endl;
    std::cout << "兩個長度是否相等: " << (tour1.getLength() == tour2.getLength() ? "是" : "否") << std::endl;
    std::cout << "期望的最優長度: 21282" << std::endl;
    std::cout << "與期望值的差異: " << (tour1.getLength() - 21282) << std::endl;
    
    if (std::abs(tour1.getLength() - 21282) < 1.0) {
        std::cout << "✅ 修復成功！計算結果與期望值匹配。" << std::endl;
    } else {
        std::cout << "❌ 仍存在問題，需要進一步調試。" << std::endl;
    }
    
    return 0;
}
