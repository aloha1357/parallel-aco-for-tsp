#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>

// Different distance calculation methods
int dist_euc_2d(double x1, double y1, double x2, double y2) {
    double d = std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    return static_cast<int>(std::lround(d));
}

int dist_att(double x1, double y1, double x2, double y2) {
    double rij = std::sqrt(((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)) / 10.0);
    int tij = static_cast<int>(std::lround(rij));
    return (tij < rij) ? (tij + 1) : tij;
}

// Read optimal tour
std::vector<int> readOptimalTour(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    std::vector<int> tour;
    
    // Skip header until TOUR_SECTION
    while (std::getline(file, line)) {
        if (line.find("TOUR_SECTION") != std::string::npos) {
            break;
        }
    }
    
    // Read tour
    while (std::getline(file, line)) {
        if (line == "-1" || line == "EOF") break;
        int city = std::stoi(line);
        tour.push_back(city - 1); // Convert to 0-based
    }
    
    return tour;
}

// Read coordinates
std::vector<std::pair<double, double>> readCoordinates(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    std::vector<std::pair<double, double>> coords;
    
    // Skip header until NODE_COORD_SECTION
    while (std::getline(file, line)) {
        if (line.find("NODE_COORD_SECTION") != std::string::npos) {
            break;
        }
    }
    
    // Read coordinates
    coords.resize(100);
    for (int i = 0; i < 100; ++i) {
        std::getline(file, line);
        std::istringstream iss(line);
        int id;
        double x, y;
        iss >> id >> x >> y;
        coords[id - 1] = {x, y};
    }
    
    return coords;
}

int main() {
    try {
        // Read coordinates and optimal tour
        auto coords = readCoordinates("data/kroA100.tsp");
        auto tour = readOptimalTour("data/kroA100.opt.tour");
        
        std::cout << "Testing different distance calculation methods:" << std::endl;
        
        // Test both distance calculation methods
        double total_euc = 0.0;
        double total_att = 0.0;
        
        for (size_t i = 0; i < tour.size(); ++i) {
            int from = tour[i];
            int to = tour[(i + 1) % tour.size()];
            
            double x1 = coords[from].first;
            double y1 = coords[from].second;
            double x2 = coords[to].first;
            double y2 = coords[to].second;
            
            int d_euc = dist_euc_2d(x1, y1, x2, y2);
            int d_att = dist_att(x1, y1, x2, y2);
            
            total_euc += d_euc;
            total_att += d_att;
        }
        
        std::cout << "Using EUC_2D distance: " << total_euc << std::endl;
        std::cout << "Using ATT distance: " << total_att << std::endl;
        std::cout << "Expected optimal: 21282" << std::endl;
        
        std::cout << "\nDifference from expected:" << std::endl;
        std::cout << "EUC_2D: " << (total_euc - 21282) << std::endl;
        std::cout << "ATT: " << (total_att - 21282) << std::endl;
        
        // Let's also try without rounding to see the raw distances
        double total_raw = 0.0;
        for (size_t i = 0; i < tour.size(); ++i) {
            int from = tour[i];
            int to = tour[(i + 1) % tour.size()];
            
            double x1 = coords[from].first;
            double y1 = coords[from].second;
            double x2 = coords[to].first;
            double y2 = coords[to].second;
            
            double d_raw = std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
            total_raw += d_raw;
        }
        
        std::cout << "Raw Euclidean (no rounding): " << total_raw << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
