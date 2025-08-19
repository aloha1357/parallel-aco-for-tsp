# 🐜 Parallel ACO for TSP

[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![OpenMP](https://img.shields.io/badge/OpenMP-Parallel-green.svg)](https://www.openmp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.20+-orange.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A high-performance **Parallel Ant Colony Optimization (ACO)** implementation for solving the **Traveling Salesman Problem (TSP)** using C++17 and OpenMP. This project demonstrates advanced software engineering practices including TDD/BDD, performance optimization, and comprehensive benchmarking.

> ## 🎯 **Quick Start for New Users**
> 
> **👉 [Go directly to the User Guide](experiment/USER_GUIDE.md)** for step-by-step instructions!
> 
> The User Guide provides:
> - ✅ **5-minute setup** instructions
> - 🧪 **Ready-to-run experiments** 
> - 📊 **Result interpretation** guidance
> - ⚙️ **Parameter customization** examples
> - 🔧 **Troubleshooting** solutions

## 🚀 Key Features

- **🔥 High-Performance Parallel Computing**: OpenMP-based multi-threading with optimized load balancing
- **📊 Comprehensive Benchmarking**: TSPLIB standard dataset support with detailed performance analysis
- **🧪 Test-Driven Development**: Complete unit test coverage with BDD scenarios using Google Test
- **⚡ Multiple Distance Calculations**: Support for EUC_2D, CEIL_2D, ATT, and GEO distance types
- **🔧 Highly Configurable**: Flexible parameter tuning for different optimization strategies
- **📈 Performance Monitoring**: Real-time metrics collection and analysis tools

## 🏗️ Architecture Overview

```mermaid
graph TB
    subgraph "Core Engine"
        AE[AcoEngine]
        PM[PheromoneModel]
        TL[ThreadLocalPheromoneModel]
        A[Ant]
    end
    
    subgraph "Data Layer"
        G[Graph]
        T[Tour]
        DC[DistanceCalculator]
        TSP[TSPLibReader]
    end
    
    subgraph "Performance & Analysis"
        PerfM[PerformanceMonitor]
        BA[BenchmarkAnalyzer]
        ER[ExperimentRunner]
        SC[StrategyComparator]
    end
    
    subgraph "Parallel Execution"
        OMP[OpenMP Threads]
        WL[Workload Distribution]
        TS[Thread Synchronization]
    end
    
    AE --> PM
    AE --> A
    A --> G
    A --> T
    G --> DC
    G --> TSP
    AE --> PerfM
    PM --> TL
    ER --> BA
    AE --> OMP
    OMP --> WL
    OMP --> TS
    
    classDef core fill:#e1f5fe
    classDef data fill:#f3e5f5
    classDef perf fill:#e8f5e8
    classDef parallel fill:#fff3e0
    
    class AE,PM,TL,A core
    class G,T,DC,TSP data
    class PerfM,BA,ER,SC perf
    class OMP,WL,TS parallel
```

## 🔄 Algorithm Flow

```mermaid
flowchart TD
    Start([Start]) --> Init[Initialize Parameters]
    Init --> CreateGraph[Create Graph from TSP File]
    CreateGraph --> InitPheromone[Initialize Pheromone Matrix]
    InitPheromone --> SetThreads[Set OpenMP Threads]
    
    SetThreads --> MainLoop{Iteration < MaxIter?}
    MainLoop -->|Yes| ParallelAnts[Parallel Ant Construction]
    
    subgraph "Parallel Execution"
        ParallelAnts --> Ant1[Thread 1: Ants 1-N/T]
        ParallelAnts --> Ant2[Thread 2: Ants N/T+1-2N/T]
        ParallelAnts --> AntN[Thread T: Ants ...]
        
        Ant1 --> LocalPheromone1[Local Pheromone Δ1]
        Ant2 --> LocalPheromone2[Local Pheromone Δ2]
        AntN --> LocalPheromoneN[Local Pheromone ΔN]
    end
    
    LocalPheromone1 --> Merge[Merge Thread Results]
    LocalPheromone2 --> Merge
    LocalPheromoneN --> Merge
    
    Merge --> UpdateBest[Update Global Best]
    UpdateBest --> Evaporate[Pheromone Evaporation]
    Evaporate --> Deposit[Pheromone Deposition]
    Deposit --> CheckConvergence{Converged?}
    
    CheckConvergence -->|No| MainLoop
    CheckConvergence -->|Yes| Results[Return Best Solution]
    MainLoop -->|No| Results
    
    Results --> End([End])
    
    classDef startEnd fill:#4caf50,color:#fff
    classDef process fill:#2196f3,color:#fff
    classDef decision fill:#ff9800,color:#fff
    classDef parallel fill:#9c27b0,color:#fff
    
    class Start,End startEnd
    class Init,CreateGraph,InitPheromone,SetThreads,UpdateBest,Evaporate,Deposit,Results process
    class MainLoop,CheckConvergence decision
    class ParallelAnts,Ant1,Ant2,AntN,LocalPheromone1,LocalPheromone2,LocalPheromoneN,Merge parallel
```

## 📊 Performance Results

Our benchmark results on Intel 24-core/32-thread processor:

| Threads | Execution Time (ms) | Speedup | Efficiency (%) | Memory (MB) |
|---------|-------------------|---------|----------------|-------------|
| 1       | 1198.9           | 0.78×   | 78.3%          | 5           |
| 2       | 463.2            | 2.03×   | 101.3%         | 5           |
| 4       | 318.6            | 2.95×   | 73.6%          | 5           |
| 8       | 252.4            | 3.72×   | 46.5%          | 6           |

**💡 Optimal Configuration**: 2 threads provide the best efficiency at 111.9%

## 🛠️ Prerequisites

- **C++ Compiler**: GCC 9.0+ or MSVC 2019+ with C++17 support
- **CMake**: Version 3.20 or higher
- **OpenMP**: For parallel execution
- **Python 3.8+**: For experiment scripts and analysis (optional)

### Windows (Recommended)
```bash
# Install via Chocolatey
choco install cmake ninja
# Or download from official websites
```

### Linux
```bash
# Ubuntu/Debian
sudo apt-get install cmake build-essential libomp-dev python3-pip

# CentOS/RHEL
sudo yum install cmake gcc-c++ openmp-devel python3-pip
```

### macOS
```bash
# Using Homebrew
brew install cmake llvm python3
```

## 🚀 Quick Start

> 🎯 **New User?** Check out our comprehensive **[User Guide](experiment/USER_GUIDE.md)** for step-by-step instructions and experiment walkthroughs!

### 1. Clone the Repository
```bash
git clone https://github.com/aloha1357/parallel-aco-for-tsp.git
cd parallel-aco-for-tsp
```

### 2. Build the Project
```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build (adjust -j based on your CPU cores)
cmake --build . --config Release -j8
```

### 3. Run Your First Experiment
```bash
# Quick validation (30 seconds)
./quick_aco_experiment

# Comprehensive experiment with detailed output (2-3 minutes)
./english_fast_aco_experiment

# Thread scalability analysis (5-10 minutes)
./ant_thread_scalability_experiment
```

### 4. 📖 Detailed Instructions
For complete guidance on running experiments, understanding results, and customizing parameters:

**👉 [Go to User Guide](experiment/USER_GUIDE.md)**

The User Guide covers:
- ✅ Step-by-step experiment execution
- 📊 Result interpretation and analysis  
- ⚙️ Parameter customization
- 🔧 Troubleshooting common issues
- 🎯 Recommended experiment workflows

## 📁 Project Structure

```
parallel-aco-for-tsp/
├── 📂 src/aco/                 # Core algorithm implementation
│   ├── AcoEngine.cpp           # Main ACO algorithm engine
│   ├── Ant.cpp                 # Ant agent with probabilistic selection
│   ├── Graph.cpp               # TSP graph representation
│   ├── PheromoneModel.cpp      # Pheromone matrix management
│   └── ...                     # Other core components
├── 📂 include/aco/             # Header files with interfaces
├── 📂 tests/                   # Comprehensive test suite
│   ├── unit/                   # Unit tests for each component
│   ├── features/               # BDD test scenarios
│   └── steps/                  # Test step definitions
├── 📂 data/                    # TSPLIB test instances
│   ├── eil51.tsp              # 51-city problem
│   ├── kroA100.tsp            # 100-city problem
│   └── ...                     # Other test instances
├── 📂 build/                   # Build outputs and results
└── 📂 experiment/              # 🧪 **Experiments & User Guide**
    ├── 📖 USER_GUIDE.md        # 🎯 **Complete Quick Start Guide**
    ├── 📖 README.md            # Experiment directory overview
    ├── 🧪 *_experiment.cpp     # Ready-to-run experiments
    ├── 🐍 *.py                 # Analysis and visualization scripts
    └── 📂 results/             # Generated charts and reports
```

> 💡 **Tip:** All experiment instructions and tutorials are in the [`experiment/`](experiment/) directory!

## ⚙️ Configuration Options

### ACO Parameters
```cpp
struct AcoParameters {
    double alpha = 1.0;         // Pheromone importance factor
    double beta = 2.0;          // Distance importance factor  
    double rho = 0.1;           // Evaporation rate
    int num_ants = 50;          // Number of ants per iteration
    int max_iterations = 100;   // Maximum iterations
    int num_threads = 4;        // OpenMP thread count
    bool enable_early_stopping = false;  // Early convergence detection
};
```

### Optimization Strategies
- **Standard**: Balanced exploration and exploitation
- **Aggressive**: High exploitation, fast convergence
- **Conservative**: Balanced approach with stability
- **Exploration**: High exploration, better solution quality
- **Exploitation**: Fast execution, local optimization

## 🧪 Testing

### Run All Tests
```bash
cd build
ctest --output-on-failure
```

### Run Specific Test Categories
```bash
# Unit tests only
./unit_tests

# BDD feature tests
./bdd_tests

# Performance benchmarks
./performance_tests
```

### Test Coverage
```bash
# Generate coverage report (if gcov is available)
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
make coverage
```

## 📈 Benchmarking

### TSPLIB Standard Benchmarks
```bash
# Run on standard TSPLIB instances
./tsplib_benchmark_experiment

# Generate performance analysis
python analyze_experiments.py

# Create visualization plots
python generate_plots.py
```

### Custom Experiments
```bash
# Run scalability analysis
./scalability_analysis --min-threads 1 --max-threads 16

# Compare strategies
./strategy_benchmark --strategies all --instances ../data/*.tsp
```

## 🔧 Advanced Usage

### Custom Distance Functions
Implement your own distance calculator:
```cpp
class CustomDistanceCalculator : public DistanceCalculator {
public:
    double calculate(const Point& p1, const Point& p2) override {
        // Your custom distance calculation
        return custom_distance(p1, p2);
    }
};
```

### Performance Monitoring
```cpp
AcoEngine engine(graph, params);
engine.enablePerformanceMonitoring(true);

PerformanceBudget budget;
budget.max_execution_time_ms = 10000;
budget.max_memory_mb = 1024;
engine.setPerformanceBudget(budget);

auto results = engine.runWithBudget(budget);
```

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guide](CONTRIBUTING.md) for details.

### Development Setup
1. Fork the repository
2. Create a feature branch: `git checkout -b feature/your-feature`
3. Follow the coding standards (see `.clang-format`)
4. Add tests for new functionality
5. Ensure all tests pass
6. Submit a pull request

### Code Style
- Follow C++17 best practices
- Use meaningful variable names
- Document public APIs with Doxygen comments
- Maintain test coverage above 90%

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **TSPLIB**: For providing standard benchmark instances
- **Google Test**: For the comprehensive testing framework
- **OpenMP**: For enabling efficient parallel execution
- **Academic Research**: Based on ant colony optimization research

## 📚 References

1. Dorigo, M., & Gambardella, L. M. (1997). Ant colony system: a cooperative learning approach to the traveling salesman problem.
2. Stützle, T., & Hoos, H. H. (2000). MAX–MIN ant system.
3. López-Ibáñez, M., & Stützle, T. (2012). The automatic design of multiobjective ant colony optimization algorithms.

## 🐛 Bug Reports & Feature Requests

Please use the [GitHub Issues](https://github.com/aloha1357/parallel-aco-for-tsp/issues) page to report bugs or request new features.

---

⭐ **Star this repository if you find it useful!**

Made with ❤️ by [aloha1357](https://github.com/aloha1357)
