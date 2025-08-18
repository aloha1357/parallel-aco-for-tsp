# Four Instances Parallel ACO Experiment (Python Implementation)

## Overview

This Python-based experiment framework evaluates the performance of parallel ACO algorithms on four TSP instances of different sizes (50, 100, 150, 202 cities). The experiment compares solution quality, execution time, speedup, and efficiency across different thread configurations.

## Experiment Design

### Test Instances

| Instance | Cities | Optimal Solution | Category | Description |
|----------|--------|------------------|----------|-------------|
| eil51    | 51     | 426             | Small    | 51-city instance (approximating 50 cities) |
| kroA100  | 100    | 21282           | Medium   | 100-city Krolak problem A |
| kroA150  | 150    | 26524           | Large    | 150-city Krolak problem A |
| gr202    | 202    | 40160           | Extra Large | 202-city geographical instance |

### Thread Configurations

Tests are performed with 1, 2, 4, and 8 threads to evaluate parallelization effects.

### Experiment Types

#### 1. Fixed Iterations Experiment
- **Objective**: Compare solution quality under the same iteration budget
- **Configuration**: 100 iterations per run
- **Repetitions**: 100 runs per configuration
- **Metrics**: 
  - Solution quality ratio (`tour_length / optimal_solution`)
  - Execution time
  - Speedup ratio
  - Parallel efficiency

#### 2. Time Budget Experiment  
- **Objective**: Compare solution quality under the same time budget
- **Time Budget**: Based on 1-thread baseline execution time
- **Repetitions**: 100 runs per configuration
- **Metrics**:
  - All metrics from Fixed Iterations Experiment
  - Actual iterations completed
  - Iterations per second

### Execution Modes

#### Development Mode (Default)
```bash
python parallel_aco_experiment.py
```
- 2 iterations, 5 runs per configuration
- Quick validation of experiment setup
- ~5 minutes execution time

#### Production Mode
```bash
python parallel_aco_experiment.py --production
```
- 100 iterations, 100 runs per configuration  
- Full research-quality experiments
- Several hours execution time

## Quick Start

### Prerequisites

1. **Python 3.7+** with pip
2. **Required packages** (installed automatically):
   ```
   pandas, numpy, matplotlib, seaborn, scipy
   ```
3. **TSP instance files** in `../data/` directory:
   - `eil51.tsp`
   - `kroA100.tsp` 
   - `kroA150.tsp`
   - `gr202.tsp`

### Running Experiments

#### Windows
```cmd
cd experiment
run_python_experiment.bat
```

#### Linux/Unix
```bash
cd experiment
chmod +x run_python_experiment.sh
./run_python_experiment.sh
```

#### Direct Python Execution
```bash
# Development mode (quick test)
python parallel_aco_experiment.py

# Production mode (full experiment)
python parallel_aco_experiment.py --production

# Analysis only (on existing results)
python parallel_aco_experiment.py --analysis-only results/experiment_20250818_143022
```

## Output Structure

All results are stored with timestamps in the following structure:

```
experiment/results/experiment_YYYYMMDD_HHMMSS/
├── fixed_iterations/           # Fixed iterations experiment data
│   └── fixed_iterations_YYYYMMDD_HHMMSS.csv
├── time_budget/               # Time budget experiment data  
│   └── time_budget_YYYYMMDD_HHMMSS.csv
├── baseline_measurements/     # Baseline timing measurements
│   └── baseline_times_YYYYMMDD_HHMMSS.csv
├── analysis/                  # Statistical analysis results
│   ├── experiment_summary.txt
│   └── detailed_statistics.csv
└── plots/                     # Generated visualizations
    ├── solution_quality_comparison.png
    ├── speedup_analysis.png
    └── time_budget_analysis.png
```

## Generated Analysis

The framework automatically generates:

### Statistical Reports
- **Summary Report**: Text-based overview of key findings
- **Detailed Statistics**: CSV with comprehensive metrics
- **Performance Metrics**: Speedup and efficiency calculations

### Visualizations
- **Solution Quality Comparison**: Box plots showing quality distribution
- **Speedup Analysis**: Speedup and efficiency curves  
- **Time Budget Analysis**: Iterations completed and throughput comparison

### Key Metrics

| Metric | Description | Formula |
|--------|-------------|---------|
| Quality Ratio | Solution quality vs optimal | `tour_length / optimal_solution` |
| Speedup Ratio | Parallelization speedup | `serial_time / parallel_time` |
| Efficiency | Parallel efficiency | `speedup_ratio / num_threads` |
| Iterations/sec | Algorithm throughput | `iterations / execution_time` |

## ACO Parameters

```json
{
  "alpha": 1.0,      // Pheromone importance
  "beta": 2.0,       // Heuristic information importance  
  "rho": 0.1,        // Pheromone evaporation rate
  "num_ants": 50,    // Number of ants
  "q0": 0.9          // Exploitation vs exploration parameter
}
```

## Features

### 🎯 Comprehensive Evaluation
- Four different problem sizes
- Multiple thread configurations
- Statistical significance testing
- Automated visualization

### 📊 Rich Analysis
- Solution quality assessment
- Performance scaling analysis
- Time efficiency evaluation
- Comparative statistics

### 🔧 Flexible Execution
- Development/Production modes
- Configurable parameters
- Timestamp-based result storage
- Resume/analysis-only options

### 📈 Professional Visualization
- Publication-ready plots
- Multiple output formats (PNG, PDF)
- Statistical error bars
- Comprehensive legends

## Implementation Notes

The current implementation uses **simulated ACO execution** for demonstration purposes. In a real deployment, the `simulate_aco_experiment()` function would be replaced with calls to the actual C++ ACO engine.

The simulation provides realistic results based on:
- Problem size scaling
- Thread count effects
- Randomized solution quality
- Execution time modeling

## Expected Results

### Solution Quality
- All configurations should achieve ratios close to 1.0 (optimal)
- Parallel versions may show slight quality improvements
- Larger instances typically have higher variance

### Performance Scaling
- Near-linear speedup for 2-4 threads
- Diminishing returns with 8 threads
- Efficiency decreases with thread count

### Time Budget Analysis
- Higher thread counts complete more iterations
- Iterations per second should increase with parallelization
- Optimal thread count varies by instance size

## Contributing

To extend this framework:

1. **Add new instances**: Update `instances` dictionary in the script
2. **Modify ACO parameters**: Adjust `aco_params` configuration
3. **Add metrics**: Extend result dictionaries and analysis functions
4. **Custom analysis**: Add new plotting functions to `ExperimentAnalyzer`

## Citation

If you use this experiment framework in research, please cite:

```
Four Instances Parallel ACO Experiment Framework
TSP Parallel ACO Research Team, 2025
```
