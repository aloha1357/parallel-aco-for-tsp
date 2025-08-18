#include "aco/TSPLibReader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cmath>

namespace aco {

std::unique_ptr<TSPLibInstance> TSPLibReader::readTSPLibFile(const std::string& filename) {
    auto instance = std::make_unique<TSPLibInstance>();
    auto lines = readFileLines(filename);
    
    if (lines.empty()) {
        std::cerr << "無法讀取檔案: " << filename << std::endl;
        return nullptr;
    }
    
    // 解析檔案頭部
    parseHeader(lines, *instance);
    
    // 尋找 NODE_COORD_SECTION
    int coord_section_start = -1;
    for (size_t i = 0; i < lines.size(); ++i) {
        if (lines[i].find("NODE_COORD_SECTION") != std::string::npos) {
            coord_section_start = i + 1;
            break;
        }
    }
    
    if (coord_section_start == -1) {
        std::cerr << "找不到 NODE_COORD_SECTION" << std::endl;
        return nullptr;
    }
    
    // 解析座標
    instance->coordinates = parseNodeCoordSection(lines, coord_section_start, instance->dimension);
    
    if (instance->coordinates.size() != static_cast<size_t>(instance->dimension)) {
        std::cerr << "座標數量與維度不符: " << instance->coordinates.size() 
                  << " vs " << instance->dimension << std::endl;
        return nullptr;
    }
    
    return instance;
}

std::unique_ptr<Graph> TSPLibReader::convertToGraph(const TSPLibInstance& instance) {
    int n = instance.dimension;
    auto graph = std::make_unique<Graph>(n);
    
    // 計算所有城市間的距離
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                double distance = calculateEuclideanDistance(
                    instance.coordinates[i], 
                    instance.coordinates[j]
                );
                graph->setDistance(i, j, distance);
            }
        }
    }
    
    return graph;
}

std::unique_ptr<Graph> TSPLibReader::loadGraphFromTSPLib(const std::string& filename) {
    auto instance = readTSPLibFile(filename);
    if (!instance) {
        return nullptr;
    }
    
    return convertToGraph(*instance);
}

bool TSPLibReader::isValidTSPLibFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    bool hasName = false;
    bool hasType = false;
    bool hasDimension = false;
    bool hasEdgeWeightType = false;
    bool hasNodeCoordSection = false;
    
    while (std::getline(file, line)) {
        if (line.find("NAME") != std::string::npos) hasName = true;
        if (line.find("TYPE") != std::string::npos) hasType = true;
        if (line.find("DIMENSION") != std::string::npos) hasDimension = true;
        if (line.find("EDGE_WEIGHT_TYPE") != std::string::npos) hasEdgeWeightType = true;
        if (line.find("NODE_COORD_SECTION") != std::string::npos) hasNodeCoordSection = true;
    }
    
    return hasName && hasType && hasDimension && hasEdgeWeightType && hasNodeCoordSection;
}

std::vector<TSPBenchmarkInstance> TSPLibReader::getStandardBenchmarks() {
    return {
        // 小規模問題 (≤60 城市)
        {"berlin52", "data/berlin52.tsp", 7542},
        {"eil51", "data/eil51.tsp", 426},
        {"st70", "data/st70.tsp", 675},
        
        // 中規模問題 (60-120 城市)
        {"eil76", "data/eil76.tsp", 538},
        {"pr76", "data/pr76.tsp", 108159},
        {"kroA100", "data/kroA100.tsp", 21282},
        {"lin105", "data/lin105.tsp", 14379},
        
        // 大規模問題 (120+ 城市)
        {"pr124", "data/pr124.tsp", 59030},
        {"ch130", "data/ch130.tsp", 6110},
        {"pr144", "data/pr144.tsp", 58537},
        {"kroA150", "data/kroA150.tsp", 26524}
    };
}

std::vector<std::pair<double, double>> TSPLibReader::parseNodeCoordSection(
    const std::vector<std::string>& lines, int start_line, int dimension) {
    
    std::vector<std::pair<double, double>> coordinates;
    coordinates.reserve(dimension);
    
    for (int i = start_line; i < static_cast<int>(lines.size()) && coordinates.size() < static_cast<size_t>(dimension); ++i) {
        std::string line = lines[i];
        
        // 跳過空行和結束標記
        if (line.empty() || line.find("EOF") != std::string::npos) {
            continue;
        }
        
        std::istringstream iss(line);
        int node_id;
        double x, y;
        
        if (iss >> node_id >> x >> y) {
            coordinates.emplace_back(x, y);
        }
    }
    
    return coordinates;
}

double TSPLibReader::calculateEuclideanDistance(
    const std::pair<double, double>& p1, 
    const std::pair<double, double>& p2) {
    
    double dx = p1.first - p2.first;
    double dy = p1.second - p2.second;
    return std::sqrt(dx * dx + dy * dy);
}

std::vector<std::string> TSPLibReader::readFileLines(const std::string& filename) {
    std::vector<std::string> lines;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "無法開啟檔案: " << filename << std::endl;
        return lines;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // 移除行尾的空白字符
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        lines.push_back(line);
    }
    
    return lines;
}

void TSPLibReader::parseHeader(const std::vector<std::string>& lines, TSPLibInstance& instance) {
    for (const auto& line : lines) {
        if (line.find("NAME") != std::string::npos) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                instance.name = line.substr(pos + 1);
                // 去除前後空白
                instance.name.erase(0, instance.name.find_first_not_of(" \t"));
                instance.name.erase(instance.name.find_last_not_of(" \t") + 1);
            }
        }
        else if (line.find("COMMENT") != std::string::npos) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                instance.comment = line.substr(pos + 1);
                instance.comment.erase(0, instance.comment.find_first_not_of(" \t"));
                instance.comment.erase(instance.comment.find_last_not_of(" \t") + 1);
            }
        }
        else if (line.find("DIMENSION") != std::string::npos) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                std::string dim_str = line.substr(pos + 1);
                dim_str.erase(0, dim_str.find_first_not_of(" \t"));
                instance.dimension = std::stoi(dim_str);
            }
        }
        else if (line.find("EDGE_WEIGHT_TYPE") != std::string::npos) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                instance.edge_weight_type = line.substr(pos + 1);
                instance.edge_weight_type.erase(0, instance.edge_weight_type.find_first_not_of(" \t"));
                instance.edge_weight_type.erase(instance.edge_weight_type.find_last_not_of(" \t") + 1);
            }
        }
        else if (line.find("NODE_COORD_SECTION") != std::string::npos) {
            // 到達座標區段，停止解析頭部
            break;
        }
    }
}
