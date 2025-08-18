#!/bin/bash

echo "編譯四個實例實驗執行器..."

# 檢查編譯器
if ! command -v g++ &> /dev/null; then
    echo "錯誤: 找不到 g++ 編譯器"
    echo "請安裝 g++ 編譯器"
    exit 1
fi

# 創建結果目錄
mkdir -p results/fixed_iterations
mkdir -p results/time_budget  
mkdir -p results/baseline_measurements
mkdir -p results/analysis

echo "創建輸出目錄完成"

# 編譯實驗執行器
echo "編譯中..."
g++ -std=c++17 -O3 -I../src -pthread four_instances_experiment_runner.cpp -o four_instances_experiment_runner

if [ $? -eq 0 ]; then
    echo "編譯成功!"
    echo ""
    echo "執行方式:"
    echo "  開發測試: ./four_instances_experiment_runner"
    echo "  正式實驗: ./four_instances_experiment_runner --production"
    echo ""
    
    # 設定執行權限
    chmod +x four_instances_experiment_runner
else
    echo "編譯失敗! 請檢查錯誤訊息"
    exit 1
fi
