/**
 * @file test_optimal_path_validation.cpp
 * @brief Tests for validating path calculations using TSPLIB optimal solutions
 * 
 * This test file validates that our distance calculations and tour length
 * computations are correct by comparing with known optimal solutions.
 */

#include <gtest/gtest.h>
#include "aco/Graph.hpp"
#include "aco/Tour.hpp"
#include "aco/TSPLibReader.hpp"
#include <fstream>
#include <sstream>

class OptimalPathValidationTest : public ::testing::Test {
protected:
    struct TSPInstance {
        std::string name;
        std::string tsp_file;
        std::string opt_tour_file;
        int optimal_length;
    };

    void SetUp() override {
        // Initialize test instances with known optimal solutions
        instances = {
            {"eil51", "../../data/eil51.tsp", "../../data/eil51.opt.tour", 426},
            {"kroA100", "../../data/kroA100.tsp", "../../data/kroA100.opt.tour", 21282},
            {"ch150", "../../data/ch150.tsp", "../../data/ch150.opt.tour", 6528},
            {"gr202", "../../data/gr202.tsp", "../../data/gr202.opt.tour", 40160}
        };
    }

    /**
     * @brief Load optimal tour from .opt.tour file
     */
    std::vector<int> loadOptimalTour(const std::string& filename) {
        std::vector<int> tour;
        std::ifstream file(filename);
        
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open optimal tour file: " + filename);
        }

        std::string line;
        bool in_tour_section = false;
        
        while (std::getline(file, line)) {
            if (line == "TOUR_SECTION") {
                in_tour_section = true;
                continue;
            }
            
            if (!in_tour_section) continue;
            
            // Parse city number
            std::istringstream iss(line);
            int city;
            if (iss >> city) {
                if (city == -1) break;  // End of tour marker
                tour.push_back(city - 1);  // Convert to 0-based indexing
            }
        }
        
        return tour;
    }

    /**
     * @brief Calculate tour length using our distance calculation
     */
    double calculateTourLength(std::shared_ptr<Graph> graph, const std::vector<int>& tour) {
        if (tour.empty()) return 0.0;
        
        double total_length = 0.0;
        
        // Calculate distances for each edge in the tour
        for (size_t i = 0; i < tour.size(); ++i) {
            int from = tour[i];
            int to = tour[(i + 1) % tour.size()];  // Wrap around to first city
            total_length += graph->getDistance(from, to);
        }
        
        return total_length;
    }

    std::vector<TSPInstance> instances;
};

/**
 * @brief Test that we can load TSP instances correctly
 */
TEST_F(OptimalPathValidationTest, LoadTSPInstances) {
    for (const auto& instance : instances) {
        SCOPED_TRACE("Testing instance: " + instance.name);
        
        auto graph = Graph::fromTSPFile(instance.tsp_file);
        ASSERT_NE(graph, nullptr) << "Failed to load " << instance.name;
        EXPECT_GT(graph->size(), 0) << "Graph should have cities";
        
        std::cout << "✓ Loaded " << instance.name 
                  << " with " << graph->size() << " cities" 
                  << " using " << graph->getDistanceType() << " distance" << std::endl;
    }
}

/**
 * @brief Test that we can load optimal tour files correctly
 */
TEST_F(OptimalPathValidationTest, LoadOptimalTours) {
    for (const auto& instance : instances) {
        SCOPED_TRACE("Testing optimal tour for: " + instance.name);
        
        try {
            auto optimal_tour = loadOptimalTour(instance.opt_tour_file);
            EXPECT_GT(optimal_tour.size(), 0) << "Optimal tour should not be empty";
            
            // Load corresponding graph to check tour size
            auto graph = Graph::fromTSPFile(instance.tsp_file);
            ASSERT_NE(graph, nullptr);
            
            EXPECT_EQ(optimal_tour.size(), static_cast<size_t>(graph->size())) 
                << "Tour should visit all cities exactly once";
            
            std::cout << "✓ Loaded optimal tour for " << instance.name 
                      << " with " << optimal_tour.size() << " cities" << std::endl;
        } catch (const std::exception& e) {
            FAIL() << "Failed to load optimal tour for " << instance.name 
                   << ": " << e.what();
        }
    }
}

/**
 * @brief Test that our distance calculations match TSPLIB optimal solutions
 */
TEST_F(OptimalPathValidationTest, ValidateOptimalSolutions) {
    for (const auto& instance : instances) {
        SCOPED_TRACE("Validating optimal solution for: " + instance.name);
        
        try {
            // Load graph and optimal tour
            auto graph = Graph::fromTSPFile(instance.tsp_file);
            ASSERT_NE(graph, nullptr) << "Failed to load graph for " << instance.name;
            
            auto optimal_tour = loadOptimalTour(instance.opt_tour_file);
            ASSERT_GT(optimal_tour.size(), 0) << "Failed to load optimal tour for " << instance.name;
            
            // Calculate tour length using our implementation
            double calculated_length = calculateTourLength(graph, optimal_tour);
            
            // Compare with known optimal length
            double tolerance = 1.0;  // Allow small numerical differences
            double difference = std::abs(calculated_length - instance.optimal_length);
            
            std::cout << "Instance: " << instance.name << std::endl;
            std::cout << "  Expected length: " << instance.optimal_length << std::endl;
            std::cout << "  Calculated length: " << calculated_length << std::endl;
            std::cout << "  Difference: " << difference << std::endl;
            std::cout << "  Distance type: " << graph->getDistanceType() << std::endl;
            
            EXPECT_LE(difference, tolerance) 
                << "Calculated tour length should match optimal solution within tolerance"
                << "\n  Instance: " << instance.name
                << "\n  Expected: " << instance.optimal_length
                << "\n  Calculated: " << calculated_length
                << "\n  Difference: " << difference
                << "\n  Distance type: " << graph->getDistanceType();
                
            if (difference <= tolerance) {
                std::cout << "✓ " << instance.name << " validation PASSED" << std::endl;
            } else {
                std::cout << "✗ " << instance.name << " validation FAILED" << std::endl;
            }
            
        } catch (const std::exception& e) {
            FAIL() << "Exception during validation of " << instance.name 
                   << ": " << e.what();
        }
        
        std::cout << "---" << std::endl;
    }
}

/**
 * @brief Test tour validation functionality
 */
TEST_F(OptimalPathValidationTest, TourValidation) {
    try {
        auto graph = Graph::fromTSPFile("../../data/eil51.tsp");
        ASSERT_NE(graph, nullptr);
        
        auto optimal_tour = loadOptimalTour("../../data/eil51.opt.tour");
        ASSERT_GT(optimal_tour.size(), 0);
        
        // Create Tour object and validate
        Tour tour(graph);
        tour.setPath(optimal_tour);
        
        EXPECT_TRUE(tour.isValid()) << "Optimal tour should be valid";
        EXPECT_EQ(tour.getPath().size(), optimal_tour.size()) << "Tour path size should match";
        
        double tour_length = tour.getLength();
        EXPECT_GT(tour_length, 0) << "Tour length should be positive";
        
        std::cout << "✓ Tour validation passed for eil51" << std::endl;
        std::cout << "  Tour length: " << tour_length << std::endl;
        
    } catch (const std::exception& e) {
        FAIL() << "Tour validation failed: " << e.what();
    }
}

/**
 * @brief Test distance calculation symmetry
 */
TEST_F(OptimalPathValidationTest, DistanceSymmetry) {
    try {
        auto graph = Graph::fromTSPFile("../../data/eil51.tsp");
        ASSERT_NE(graph, nullptr);
        
        // Test symmetry for first 10 pairs of cities
        for (int i = 0; i < std::min(10, graph->size()); ++i) {
            for (int j = i + 1; j < std::min(10, graph->size()); ++j) {
                double dist_ij = graph->getDistance(i, j);
                double dist_ji = graph->getDistance(j, i);
                
                EXPECT_DOUBLE_EQ(dist_ij, dist_ji) 
                    << "Distance should be symmetric: d(" << i << "," << j << ") = d(" << j << "," << i << ")";
            }
        }
        
        std::cout << "✓ Distance symmetry validated" << std::endl;
        
    } catch (const std::exception& e) {
        FAIL() << "Distance symmetry test failed: " << e.what();
    }
}

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
