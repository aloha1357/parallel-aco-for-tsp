#!/bin/bash

echo "============================================="
echo "   Four Instances Parallel ACO Experiment"
echo "============================================="

# Check Python installation
if ! command -v python3 &> /dev/null; then
    echo "ERROR: Python 3 is not installed or not in PATH"
    echo "Please install Python 3.7+ and try again"
    exit 1
fi

# Install required packages
echo "Installing required Python packages..."
pip3 install pandas numpy matplotlib seaborn scipy
if [ $? -ne 0 ]; then
    echo "WARNING: Some packages may not have been installed correctly"
fi

# Check data files
echo "Checking TSP instance files..."
missing_files=0

if [ ! -f "../data/eil51.tsp" ]; then
    echo "Missing file: ../data/eil51.tsp"
    missing_files=1
fi
if [ ! -f "../data/kroA100.tsp" ]; then
    echo "Missing file: ../data/kroA100.tsp"
    missing_files=1
fi
if [ ! -f "../data/kroA150.tsp" ]; then
    echo "Missing file: ../data/kroA150.tsp"
    missing_files=1
fi
if [ ! -f "../data/gr202.tsp" ]; then
    echo "Missing file: ../data/gr202.tsp"
    missing_files=1
fi

if [ $missing_files -eq 1 ]; then
    echo "ERROR: Missing required TSP instance files"
    echo "Please ensure all .tsp files are in the data/ directory"
    exit 1
fi

echo "All data files found"

# Select execution mode
echo ""
echo "Select execution mode:"
echo "[1] Development mode (2 iterations, 5 runs) - Quick test"
echo "[2] Production mode (100 iterations, 100 runs) - Full experiment"
echo "[3] Analysis only (analyze existing results)"
echo "[4] Exit"
echo ""
read -p "Enter choice (1-4): " choice

case $choice in
    1)
        echo "Running development mode experiment..."
        python3 parallel_aco_experiment.py
        ;;
    2)
        echo "Running production mode experiment..."
        echo "WARNING: Production mode may take several hours"
        read -p "Are you sure you want to continue? (y/N): " confirm
        if [[ $confirm =~ ^[Yy]$ ]]; then
            python3 parallel_aco_experiment.py --production
        else
            echo "Cancelled"
            exit 0
        fi
        ;;
    3)
        echo "Available results directories:"
        ls -d results/experiment_* 2>/dev/null || echo "No experiment results found"
        echo ""
        read -p "Enter results directory name: " results_dir
        if [ -d "results/$results_dir" ]; then
            python3 parallel_aco_experiment.py --analysis-only "results/$results_dir"
        else
            echo "ERROR: Directory not found"
        fi
        ;;
    4)
        echo "Exiting"
        exit 0
        ;;
    *)
        echo "Invalid choice"
        exit 1
        ;;
esac

echo ""
echo "Script completed"
