#!/bin/bash

echo "========================================"
echo "   四個實例平行ACO實驗快速執行腳本"
echo "========================================"

# 檢查是否編譯過
if [ ! -f "four_instances_experiment_runner" ]; then
    echo "實驗程式未編譯，正在編譯..."
    ./build_experiment.sh
    if [ $? -ne 0 ]; then
        echo "編譯失敗，退出"
        exit 1
    fi
fi

# 檢查數據檔案
echo "檢查實驗數據檔案..."
missing_files=0

if [ ! -f "../data/eil51.tsp" ]; then
    echo "缺少檔案: ../data/eil51.tsp"
    missing_files=1
fi
if [ ! -f "../data/kroA100.tsp" ]; then
    echo "缺少檔案: ../data/kroA100.tsp"
    missing_files=1
fi
if [ ! -f "../data/kroA150.tsp" ]; then
    echo "缺少檔案: ../data/kroA150.tsp"
    missing_files=1
fi
if [ ! -f "../data/gr202.tsp" ]; then
    echo "缺少檔案: ../data/gr202.tsp"
    missing_files=1
fi

if [ $missing_files -eq 1 ]; then
    echo "錯誤: 缺少必要的TSP數據檔案"
    echo "請確保data目錄下有所有需要的.tsp檔案"
    exit 1
fi

echo "所有數據檔案檢查完成"

# 詢問執行模式
echo ""
echo "選擇執行模式:"
echo "[1] 開發模式 (2 iterations, 5 runs) - 快速測試"
echo "[2] 正式模式 (100 iterations, 100 runs) - 完整實驗"
echo "[3] 退出"
echo ""
read -p "請輸入選擇 (1-3): " choice

case $choice in
    1)
        echo "執行開發模式實驗..."
        ./four_instances_experiment_runner
        ;;
    2)
        echo "執行正式模式實驗..."
        echo "警告: 正式模式可能需要較長時間執行"
        read -p "確定要繼續嗎? (y/N): " confirm
        if [[ $confirm =~ ^[Yy]$ ]]; then
            ./four_instances_experiment_runner --production
        else
            echo "取消執行"
            exit 0
        fi
        ;;
    3)
        echo "退出"
        exit 0
        ;;
    *)
        echo "無效選擇"
        exit 1
        ;;
esac

# 檢查是否有Python用於結果分析
echo ""
echo "實驗執行完成，檢查是否可以進行結果分析..."
if command -v python3 &> /dev/null; then
    read -p "發現Python，是否執行結果分析? (y/N): " analyze
    if [[ $analyze =~ ^[Yy]$ ]]; then
        echo "安裝所需的Python套件..."
        pip3 install pandas matplotlib seaborn scipy numpy
        echo "執行結果分析..."
        python3 analyze_results.py
        echo "分析完成! 請查看 results/analysis/ 目錄中的結果"
    fi
else
    echo "Python未安裝，跳過自動分析"
    echo "如需分析結果，請安裝Python後執行: python3 analyze_results.py"
fi

echo ""
echo "腳本執行完成"
