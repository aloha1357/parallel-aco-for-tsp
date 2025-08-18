#pragma once

#include "Graph.hpp"
#include <string>
#include <memory>
#include <vector>
#include <map>

namespace aco {

struct TSPLibInstance {
    std::string name;
    std::string comment;
    int dimension;
    std::string edge_weight_type;
    std::vector<std::pair<double, double>> coordinates;
    int optimal_solution = -1;  // 已知最優解
};

struct TSPBenchmarkInstance {
    std::string name;
    std::string filename;
    int optimal_solution;
    
    TSPBenchmarkInstance(const std::string& n, const std::string& f, int opt)
        : name(n), filename(f), optimal_solution(opt) {}
};

class TSPLibReader {
public:
    // 讀取TSPLIB格式檔案
    static std::unique_ptr<TSPLibInstance> readTSPLibFile(const std::string& filename);
    
    // 轉換為內部Graph格式
    static std::unique_ptr<Graph> convertToGraph(const TSPLibInstance& instance);
    
    // 直接從檔案載入Graph
    static std::unique_ptr<Graph> loadGraphFromTSPLib(const std::string& filename);
    
    // 驗證檔案格式
    static bool isValidTSPLibFile(const std::string& filename);
    
    // 獲取標準TSPLIB基準測試集
    static std::vector<TSPBenchmarkInstance> getStandardBenchmarks();
    
private:
    // 解析節點坐標
    static std::vector<std::pair<double, double>> parseNodeCoordSection(
        const std::vector<std::string>& lines, int start_line, int dimension);
    
    // 計算歐幾里得距離
    static double calculateEuclideanDistance(
        const std::pair<double, double>& p1, 
        const std::pair<double, double>& p2);
    
    // 讀取檔案所有行
    static std::vector<std::string> readFileLines(const std::string& filename);
    
    // 解析檔案頭部資訊
    static void parseHeader(const std::vector<std::string>& lines, TSPLibInstance& instance);
};

} // namespace aco
