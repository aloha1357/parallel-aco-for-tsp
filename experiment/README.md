# 🧪 Experiment Directory

This directory contains all experimental scripts, results, and analysis tools for the Parallel ACO for TSP project.

## 📚 Documentation

### 🚀 **[USER_GUIDE.md](USER_GUIDE.md)** - **START HERE**
**Complete quick start guide for running experiments and understanding results.**

Perfect for:
- New users getting started
- Quick experiment execution  
- Result interpretation
- Parameter customization
- Troubleshooting

## 🔬 Available Experiments

### Ready-to-Run Executables (in `../build/`)

| Experiment | Duration | Purpose |
|------------|----------|---------|
| `quick_aco_experiment` | ~30 sec | Quick validation and performance overview |
| `english_fast_aco_experiment` | ~2-3 min | Comprehensive experiment with detailed English output |
| `ant_thread_scalability_experiment` | ~5-10 min | Thread scalability analysis |
| `comprehensive_aco_experiment` | ~10-15 min | Full algorithm evaluation |
| `four_instances_experiment_runner` | ~15-20 min | Multi-instance comparison |

### Source Files (for customization)

| File | Description |
|------|-------------|
| `quick_aco_experiment.cpp` | Fast performance testing |
| `english_fast_aco_experiment.cpp` | Detailed English output experiment |
| `ant_thread_scalability_experiment.cpp` | Thread performance analysis |
| `comprehensive_aco_experiment.cpp` | Complete algorithm evaluation |
| `four_instances_experiment_runner.cpp` | Multi-instance testing |

## 📊 Analysis Tools

| Tool | Purpose |
|------|---------|
| `speedup_analysis.py` | Generate speedup visualization charts |
| `summary_analysis.py` | Create summary analysis reports |
| `analyze_results.py` | Process experimental data |

## 📁 Directory Structure

```
experiment/
├── 📖 USER_GUIDE.md           # ← **START HERE** for quick start
├── 📖 README.md               # This file
├── 🧪 *_experiment.cpp        # Experiment source files
├── 🐍 *.py                    # Analysis scripts
├── 📂 results/                # Generated results and reports
│   ├── *.csv                  # Raw experimental data
│   ├── *.png                  # Visualization charts
│   └── *.md                   # Analysis reports
└── 📂 experiment/             # Additional experiment data
```

## 🎯 Quick Commands

```bash
# Navigate to build directory first
cd ../build

# Run quick validation
./quick_aco_experiment

# Run comprehensive analysis
./english_fast_aco_experiment

# Generate visualizations (requires Python)
cd ../experiment
python speedup_analysis.py
python summary_analysis.py
```

## 📞 Need Help?

1. **📖 Read the [User Guide](USER_GUIDE.md)** - Comprehensive instructions
2. **🔍 Check [Troubleshooting](USER_GUIDE.md#troubleshooting)** - Common issues and solutions
3. **💬 Open an [Issue](https://github.com/aloha1357/parallel-aco-for-tsp/issues)** - Get community help

---

🚀 **Ready to start?** Go to **[USER_GUIDE.md](USER_GUIDE.md)** for step-by-step instructions!
