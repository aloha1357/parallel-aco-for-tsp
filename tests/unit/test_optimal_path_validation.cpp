#include "aco/Graph.hpp"
#include "aco/Tour.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>

// Simple test framework
class OptimalPathValidator {
public:
    static int tests_run;
    static int tests_passed;
    
    static void assert_equal(double expected, double actual, const std::string& test_name, double tolerance = 1.0) {
        tests_run++;
        if (std::abs(expected - actual) <= tolerance) {
            std::cout << "✓ PASS: " << test_name << " (expected: " << expected << ", actual: " << actual << ")" << std::endl;
            tests_passed++;
        } else {
            std::cout << "✗ FAIL: " << test_name << " (expected: " << expected << ", actual: " << actual 
                      << ", diff: " << std::abs(expected - actual) << ")" << std::endl;
        }
    }
    
    static void print_summary() {
        std::cout << "\n========== 最優路徑驗證結果 ==========" << std::endl;
        std::cout << "總測試數量: " << tests_run << std::endl;
        std::cout << "通過測試: " << tests_passed << std::endl;
        std::cout << "失敗測試: " << (tests_run - tests_passed) << std::endl;
        std::cout << "成功率: " << (tests_run > 0 ? (100.0 * tests_passed / tests_run) : 0) << "%" << std::endl;
        
        if (tests_passed == tests_run) {
            std::cout << "\n🎉 所有最優路徑驗證測試通過！距離計算系統準確無誤" << std::endl;
        } else {
            std::cout << "\n❌ 部分測試失敗，距離計算可能存在問題" << std::endl;
        }
    }
};

int OptimalPathValidator::tests_run = 0;
int OptimalPathValidator::tests_passed = 0;

struct OptimalTour {
    std::string name;
    int expected_length;
    std::vector<int> path;
    std::string tsp_file;
    std::string tour_file;
};

// 讀取 TOUR 檔案
OptimalTour loadOptimalTour(const std::string& tour_filename, const std::string& tsp_filename) {
    OptimalTour tour;
    tour.tour_file = tour_filename;
    tour.tsp_file = tsp_filename;
    
    std::ifstream file(tour_filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open tour file: " + tour_filename);
    }
    
    std::string line;
    bool in_tour_section = false;
    
    while (std::getline(file, line)) {
        if (line.find("NAME") != std::string::npos) {
            tour.name = line.substr(line.find(":") + 1);
            // 去除空白字符
            tour.name.erase(0, tour.name.find_first_not_of(" \t"));
            tour.name.erase(tour.name.find_last_not_of(" \t") + 1);
        }
        else if (line.find("COMMENT") != std::string::npos) {
            // 從註釋中提取期望長度
            std::string comment = line.substr(line.find(":") + 1);
            std::istringstream iss(comment);
            std::string word;
            while (iss >> word) {
                // 尋找括號中的數字
                if (word.find("(") != std::string::npos && word.find(")") != std::string::npos) {
                    std::string num_str = word.substr(word.find("(") + 1);
                    num_str = num_str.substr(0, num_str.find(")"));
                    tour.expected_length = std::stoi(num_str);
                    break;
                }
                // 或尋找 "Length:" 後的數字
                if (word == "Length:") {
                    if (iss >> word) {
                        word = word.substr(0, word.find(")"));
                        tour.expected_length = std::stoi(word);
                        break;
                    }
                }
                // 或直接是數字
                try {
                    tour.expected_length = std::stoi(word);
                    break;
                } catch (...) {
                    // 繼續尋找
                }
            }
        }
        else if (line.find("TOUR_SECTION") != std::string::npos) {
            in_tour_section = true;
        }
        else if (in_tour_section && line != "-1" && line != "EOF") {
            try {
                int city = std::stoi(line);
                if (city > 0) {
                    tour.path.push_back(city - 1); // 轉換為 0-based 索引
                }
            } catch (...) {
                // 忽略無效的行
            }
        }
        else if (line == "-1" || line == "EOF") {
            break;
        }
    }
    
    return tour;
}

// 計算路徑的總長度
double calculatePathLength(const std::vector<int>& path, std::shared_ptr<Graph> graph) {
    if (path.empty()) return 0.0;
    
    double total_length = 0.0;
    
    // 計算相鄰城市間的距離
    for (size_t i = 0; i < path.size() - 1; ++i) {
        total_length += graph->getDistance(path[i], path[i + 1]);
    }
    
    // 加上回到起始城市的距離
    if (path.size() > 1) {
        total_length += graph->getDistance(path.back(), path.front());
    }
    
    return total_length;
}

void test_optimal_path_validation() {
    std::cout << "========================================" << std::endl;
    std::cout << "   最優路徑距離計算驗證測試" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // 測試案例定義
    std::vector<std::pair<std::string, std::string>> test_cases = {
        {"../../data/eil51.tsp", "../../data/eil51.opt.tour"},
        {"../../data/kroA100.tsp", "../../data/kroA100.opt.tour"},
        {"../../data/ch150.tsp", "../../data/ch150.opt.tour"},
        {"../../data/gr202.tsp", "../../data/gr202.opt.tour"}
    };
    
    for (const auto& test_case : test_cases) {
        std::string tsp_file = test_case.first;
        std::string tour_file = test_case.second;
        
        try {
            std::cout << "\n--- 測試 " << tsp_file << " ---" << std::endl;
            
            // 載入圖
            auto graph = Graph::fromTSPFile(tsp_file);
            std::cout << "圖載入成功，城市數量: " << graph->size() 
                      << "，距離類型: " << graph->getDistanceType() << std::endl;
            
            // 載入最優路徑
            OptimalTour optimal_tour = loadOptimalTour(tour_file, tsp_file);
            std::cout << "最優路徑載入成功，期望長度: " << optimal_tour.expected_length 
                      << "，路徑城市數: " << optimal_tour.path.size() << std::endl;
            
            // 計算路徑長度
            double calculated_length = calculatePathLength(optimal_tour.path, graph);
            std::cout << "計算得到路徑長度: " << calculated_length << std::endl;
            
            // 驗證長度是否匹配
            std::string test_name = "最優路徑長度驗證 - " + optimal_tour.name;
            OptimalPathValidator::assert_equal(optimal_tour.expected_length, calculated_length, test_name);
            
            // 額外驗證：檢查路徑是否包含所有城市
            if (optimal_tour.path.size() == static_cast<size_t>(graph->size())) {
                std::cout << "✓ 路徑包含所有城市" << std::endl;
            } else {
                std::cout << "✗ 路徑城市數不匹配，期望: " << graph->size() 
                          << "，實際: " << optimal_tour.path.size() << std::endl;
            }
            
        } catch (const std::exception& e) {
            std::cout << "✗ 測試失敗: " << e.what() << std::endl;
            OptimalPathValidator::tests_run++;
        }
    }
}

void test_tour_class_integration() {
    std::cout << "\n--- 測試 Tour 類別整合 ---" << std::endl;
    
    try {
        // 載入 eil51 進行 Tour 類別測試
        auto graph = Graph::fromTSPFile("../../data/eil51.tsp");
        OptimalTour optimal_tour = loadOptimalTour("../../data/eil51.opt.tour", "../../data/eil51.tsp");
        
        // 使用 Tour 類別計算長度
        Tour tour(graph);
        tour.setPath(optimal_tour.path);
        
        double tour_length = tour.getLength();
        
        std::cout << "Tour 類別計算長度: " << tour_length << std::endl;
        std::cout << "期望長度: " << optimal_tour.expected_length << std::endl;
        
        OptimalPathValidator::assert_equal(optimal_tour.expected_length, tour_length, 
                                         "Tour 類別長度計算驗證 - eil51");
        
    } catch (const std::exception& e) {
        std::cout << "✗ Tour 類別測試失敗: " << e.what() << std::endl;
        OptimalPathValidator::tests_run++;
    }
}

int main() {
    test_optimal_path_validation();
    test_tour_class_integration();
    
    OptimalPathValidator::print_summary();
    
    return (OptimalPathValidator::tests_passed == OptimalPathValidator::tests_run) ? 0 : 1;
}
