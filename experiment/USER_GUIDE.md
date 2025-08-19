# 🚀 Quick Start User Guide - Parallel ACO for TSP

This guide will help you quickly set up and run experiments with our Parallel Ant Colony Optimization implementation for the Traveling Salesman Problem.

## 📋 Table of Contents

1. [Quick Setup](#quick-setup)
2. [Running Your First Experiment](#running-your-first-experiment)
3. [Available Experiments](#available-experiments)
4. [Understanding Results](#understanding-results)
5. [Customizing Experiments](#customizing-experiments)
6. [Performance Analysis](#performance-analysis)
7. [Troubleshooting](#troubleshooting)

## 🏃‍♂️ Quick Setup

### Prerequisites Check
Ensure you have the following installed:
- **C++ Compiler** with C++17 support (GCC 9.0+ or MSVC 2019+)
- **CMake** 3.20+
- **OpenMP** for parallel execution

### Build Instructions

1. **Clone and navigate to the project:**
   ```bash
   git clone https://github.com/aloha1357/parallel-aco-for-tsp.git
   cd parallel-aco-for-tsp
   ```

2. **Build the project:**
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   cmake --build . --config Release -j8
   ```

3. **Verify build success:**
   ```bash
   ls *.exe  # On Windows
   ls *experiment*  # On Linux/macOS
   ```

## 🎯 Running Your First Experiment

### Basic ACO Run
Start with the simplest experiment to verify everything works:

```bash
# Navigate to build directory
cd build

# Run basic ACO algorithm
./aco_main
```

Expected output:
```
🐜 Parallel ACO for TSP - Starting...
📊 Loading TSP instance: eil51.tsp
⚙️  Parameters: 50 ants, 100 iterations, 4 threads
🚀 Running optimization...
✅ Best solution found: 426.0 (Distance)
⏱️  Execution time: 1.23 seconds
```

### Quick Performance Test
Run a quick performance comparison:

```bash
./quick_aco_experiment
```

This will:
- Test different thread counts (1, 2, 4, 8)
- Use multiple TSP instances
- Generate a summary report

## 🧪 Available Experiments

Our system provides several pre-configured experiments:

### 1. **Quick ACO Experiment** (`quick_aco_experiment.exe`)
**Purpose:** Fast performance overview
```bash
./quick_aco_experiment
```
- **Duration:** ~30 seconds
- **Output:** Quick performance summary
- **Best for:** Initial testing and validation

### 2. **English Fast ACO Experiment** (`english_fast_aco_experiment.exe`)
**Purpose:** Comprehensive experiment with English output
```bash
./english_fast_aco_experiment
```
- **Duration:** ~2-3 minutes
- **Output:** Detailed English results with CSV data
- **Best for:** Detailed analysis and reporting

### 3. **Ant Thread Scalability Experiment** (`ant_thread_scalability_experiment.exe`)
**Purpose:** Thread scalability analysis
```bash
./ant_thread_scalability_experiment
```
- **Duration:** ~5-10 minutes
- **Output:** Thread performance analysis
- **Best for:** Understanding parallel performance

### 4. **Comprehensive ACO Experiment** (`comprehensive_aco_experiment.exe`)
**Purpose:** Full algorithm evaluation
```bash
./comprehensive_aco_experiment
```
- **Duration:** ~10-15 minutes
- **Output:** Complete performance metrics
- **Best for:** Research and benchmarking

### 5. **Four Instances Experiment** (`four_instances_experiment_runner.exe`)
**Purpose:** Multi-instance comparison
```bash
./four_instances_experiment_runner
```
- **Duration:** ~15-20 minutes
- **Output:** Cross-instance analysis
- **Best for:** Algorithm robustness testing

## 📊 Understanding Results

### Output Files Location
All results are saved in:
```
build/
├── *_results.csv           # Raw experimental data
├── *_summary.md            # Human-readable summaries
└── *_report.md            # Detailed analysis reports
```

### Key Metrics Explained

1. **Execution Time (ms):** Total algorithm runtime
2. **Best Distance:** Shortest tour found
3. **Average Distance:** Mean tour length across runs
4. **Speedup:** Performance improvement with more threads
5. **Efficiency (%):** How well parallel resources are utilized

### Sample Results Interpretation
```
Threads: 4, Time: 245.6ms, Best: 426.0, Speedup: 3.2x, Efficiency: 80%
```
- Using 4 threads took 245.6 milliseconds
- Best tour found has distance 426.0
- 3.2x faster than single-threaded version
- 80% efficiency indicates good parallel scaling

## ⚙️ Customizing Experiments

### Modify Parameters
Edit experiment source files to change ACO parameters:

```cpp
// In any experiment file
AcoParameters params;
params.alpha = 1.0;          // Pheromone importance
params.beta = 2.0;           // Distance importance  
params.rho = 0.1;            // Evaporation rate
params.num_ants = 50;        // Ants per iteration
params.max_iterations = 100; // Maximum iterations
params.num_threads = 4;      // Thread count
```

### Custom TSP Instances
Add your own TSP files to the `data/` directory:

1. **Place TSP file:** Copy your `.tsp` file to `data/`
2. **Modify experiment:** Update the filename in the experiment code
3. **Rebuild:** `cmake --build . --config Release`

### Experiment Duration Control
Control experiment time vs. quality tradeoff:

```cpp
// Quick test (development)
params.max_iterations = 10;
params.num_ants = 20;

// Standard test (evaluation)  
params.max_iterations = 100;
params.num_ants = 50;

// Intensive test (benchmarking)
params.max_iterations = 500;
params.num_ants = 100;
```

## 📈 Performance Analysis

### Generate Visualization
If you have Python installed:

```bash
cd experiment
python speedup_analysis.py
python summary_analysis.py
```

This creates:
- `speedup_analysis.png` - Thread performance charts
- `aco_summary_analysis.png` - Algorithm comparison
- `aco_analysis_report.png` - Detailed metrics

### Benchmark Against TSPLIB
Compare your results with known optimal solutions:

1. **Check `data/tsplib_optimal_solutions.txt`** for optimal distances
2. **Run comprehensive experiment:** `./comprehensive_aco_experiment`
3. **Compare results:** Your best distance vs. optimal distance

### Performance Optimization Tips

1. **Thread Count:** Start with `threads = CPU_cores / 2`
2. **Ant Count:** Use `ants = 2 * num_cities` as starting point
3. **Iterations:** Increase for better quality, decrease for speed
4. **Memory:** Monitor memory usage for large instances

## 🔧 Troubleshooting

### Common Issues

**Problem:** `OpenMP not found`
```bash
# Solution: Install OpenMP development libraries
# Ubuntu/Debian:
sudo apt-get install libomp-dev
# Windows: Use Visual Studio with OpenMP support
```

**Problem:** Executable crashes immediately
```bash
# Solution: Verify TSP data files exist
ls data/*.tsp
# If missing, re-run data download scripts
```

**Problem:** Very slow performance
```bash
# Solution: Ensure Release build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

**Problem:** Memory errors with large instances
```bash
# Solution: Reduce ant count or use fewer threads
# Edit experiment parameters before building
```

### Debug Mode
For detailed debugging information:

```bash
# Build in debug mode
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug

# Run with verbose output
./aco_main --verbose
```

### Performance Issues

If experiments run slowly:

1. **Check CPU usage:** Should be ~100% during parallel execution
2. **Verify thread count:** Don't exceed physical CPU cores
3. **Monitor memory:** Large instances may need more RAM
4. **Test smaller instances:** Start with `eil51.tsp` (51 cities)

## 🎯 Recommended Experiment Workflow

For new users, we recommend this progression:

### 1. **Validation Phase** (5 minutes)
```bash
./quick_aco_experiment
```
Verify basic functionality works correctly.

### 2. **Understanding Phase** (10 minutes)
```bash
./english_fast_aco_experiment
```
Understand output format and metrics.

### 3. **Performance Phase** (15 minutes)
```bash
./ant_thread_scalability_experiment
```
Analyze parallel performance on your hardware.

### 4. **Research Phase** (30+ minutes)
```bash
./comprehensive_aco_experiment
./four_instances_experiment_runner
```
Complete evaluation for research or benchmarking.

## 🔬 Advanced Usage

### Custom Experiment Creation
Create your own experiment by copying an existing one:

```bash
# Copy template
cp experiment/quick_aco_experiment.cpp experiment/my_experiment.cpp

# Edit parameters and logic
# Add to CMakeLists.txt
# Rebuild project
```

### Integration with Other Tools
Export results for external analysis:

```csv
# Results CSV format
Thread_Count,Instance,Best_Distance,Avg_Distance,Execution_Time_ms,Speedup,Efficiency
1,eil51,426.0,438.2,956.3,1.0,100.0
2,eil51,426.0,435.8,489.1,1.96,97.8
4,eil51,428.0,441.2,245.6,3.89,97.3
```

Import into Excel, R, MATLAB, or Python for further analysis.

### Continuous Integration
Set up automated testing:

```bash
# Add to CI/CD pipeline
./quick_aco_experiment --output results.csv
python validate_results.py results.csv
```

## 📞 Getting Help

- **GitHub Issues:** [Report bugs or ask questions](https://github.com/aloha1357/parallel-aco-for-tsp/issues)
- **Documentation:** Check the main [README.md](../README.md) for detailed information
- **Code Examples:** Look at experiment source files for implementation details

## ✨ Next Steps

After mastering the basics:

1. **Experiment with parameters** to improve solution quality
2. **Add new TSP instances** to test algorithm robustness  
3. **Implement custom distance functions** for specialized problems
4. **Contribute improvements** back to the project

---

🎉 **Happy experimenting!** Your journey into parallel ant colony optimization starts here.

*For more advanced topics, see the main [README.md](../README.md) and explore the source code in the `src/` directory.*
