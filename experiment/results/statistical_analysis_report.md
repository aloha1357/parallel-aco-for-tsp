# ACO Parallel Performance Analysis Report

Generated on: 2025-08-19 01:41:03

## Dataset Overview

- **Total experiments**: 80
- **TSP instances**: eil51, kroA100, ch150, gr202
- **Thread configurations**: 1, 2, 4, 8
- **Rounds per configuration**: 5
- **Iterations per run**: 50

## Performance Summary by Instance

### ch150

- **Cities**: 150
- **Best known solution**: 6528.0

| Threads | Avg Time (ms) | Std Dev | Speedup | Efficiency (%) | Avg Ratio |
|---------|---------------|---------|---------|----------------|-----------|\n| 1 | 5279 | 347 | 0.94 | 94.0 | 2.78 |
| 2 | 7037 | 189 | 0.70 | 35.2 | 2.79 |
| 4 | 5442 | 25 | 0.91 | 22.7 | 2.77 |
| 8 | 5026 | 33 | 0.98 | 12.3 | 2.77 |

### eil51

- **Cities**: 51
- **Best known solution**: 426.0

| Threads | Avg Time (ms) | Std Dev | Speedup | Efficiency (%) | Avg Ratio |
|---------|---------------|---------|---------|----------------|-----------|\n| 1 | 656 | 60 | 0.94 | 94.0 | 1.75 |
| 2 | 855 | 26 | 0.72 | 35.9 | 1.75 |
| 4 | 660 | 10 | 0.93 | 23.2 | 1.77 |
| 8 | 659 | 8 | 0.93 | 11.6 | 1.75 |

### gr202

- **Cities**: 202
- **Best known solution**: 40160.0

| Threads | Avg Time (ms) | Std Dev | Speedup | Efficiency (%) | Avg Ratio |
|---------|---------------|---------|---------|----------------|-----------|\n| 1 | 8955 | 140 | 0.99 | 98.8 | 2.55 |
| 2 | 13172 | 169 | 0.67 | 33.6 | 2.58 |
| 4 | 10257 | 279 | 0.86 | 21.6 | 2.55 |
| 8 | 10184 | 266 | 0.87 | 10.9 | 2.57 |

### kroA100

- **Cities**: 100
- **Best known solution**: 21282.0

| Threads | Avg Time (ms) | Std Dev | Speedup | Efficiency (%) | Avg Ratio |
|---------|---------------|---------|---------|----------------|-----------|\n| 1 | 2226 | 52 | 0.98 | 98.2 | 2.25 |
| 2 | 3209 | 48 | 0.68 | 34.1 | 2.26 |
| 4 | 2378 | 11 | 0.92 | 23.0 | 2.25 |
| 8 | 2274 | 8 | 0.96 | 12.0 | 2.20 |

## Key Findings

### Best Efficiency by Instance

- **ch150**: 100.0% (with 1 thread)
- **eil51**: 100.0% (with 1 thread)
- **gr202**: 100.0% (with 1 thread)
- **kroA100**: 100.0% (with 1 thread)

### Scalability Analysis

- **Single-thread efficiency**: 96.2%
- **8-thread efficiency**: 11.7%
- **Efficiency drop**: 84.5 percentage points

### Solution Quality Analysis

| Threads | Avg Ratio | Std Dev | Quality Impact |
|---------|-----------|---------|----------------|
| 1 | 2.332 | 0.395 | Variable |
| 2 | 2.342 | 0.406 | Variable |
| 4 | 2.334 | 0.384 | Variable |
| 8 | 2.326 | 0.401 | Variable |

### Recommendations

Based on the analysis:

1. **Single-thread performance** is generally most efficient for these problem sizes
2. **Parallel overhead** dominates the benefits for small to medium TSP instances
3. **Solution quality** remains consistent across thread configurations
4. **Larger problem instances** may benefit more from parallelization
5. **Algorithm tuning** may be needed to improve parallel efficiency
