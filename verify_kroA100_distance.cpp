#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>

// EUC_2D distance calculation (from TSPLIB spec)
int dist_euc_2d(double x1, double y1, double x2, double y2) {
    double d = std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    return static_cast<int>(std::lround(d));
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
        
        std::cout << "Loaded " << coords.size() << " coordinates" << std::endl;
        std::cout << "Loaded tour with " << tour.size() << " cities" << std::endl;
        
        // Calculate tour length
        double total_length = 0.0;
        for (size_t i = 0; i < tour.size(); ++i) {
            int from = tour[i];
            int to = tour[(i + 1) % tour.size()];
            
            double x1 = coords[from].first;
            double y1 = coords[from].second;
            double x2 = coords[to].first;
            double y2 = coords[to].second;
            
            int distance = dist_euc_2d(x1, y1, x2, y2);
            total_length += distance;
            
            if (i < 5) { // Show first few distances for debugging
                std::cout << "Distance from city " << (from+1) << " to " << (to+1) 
                         << ": " << distance << std::endl;
                std::cout << "  Coords: (" << x1 << "," << y1 << ") to (" 
                         << x2 << "," << y2 << ")" << std::endl;
            }
        }
        
        std::cout << "\nCalculated optimal tour length: " << total_length << std::endl;
        std::cout << "Expected optimal tour length: 21282" << std::endl;
        std::cout << "Difference: " << (total_length - 21282) << std::endl;
        
        // Test a few specific distances
        std::cout << "\nTesting specific distances:" << std::endl;
        std::cout << "Distance from city 1 to city 47:" << std::endl;
        std::cout << "  Coords: (" << coords[0].first << "," << coords[0].second 
                  << ") to (" << coords[46].first << "," << coords[46].second << ")" << std::endl;
        int d = dist_euc_2d(coords[0].first, coords[0].second, coords[46].first, coords[46].second);
        std::cout << "  Distance: " << d << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
