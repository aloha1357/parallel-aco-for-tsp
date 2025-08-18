# 實驗目錄結構

```
experiment/
├── README.md                              # 實驗設計說明
├── four_instances_experiment_config.json  # 新的實驗配置文件
├── experiment_config.json                 # 原有配置文件(已移入)
├── experiment_config_detailed.json        # 原有詳細配置(已移入)
├── four_instances_experiment_runner.cpp   # 主要實驗執行器
├── analyze_results.py                     # 結果分析腳本
├── build_experiment.bat                   # Windows編譯腳本
├── build_experiment.sh                    # Linux編譯腳本
├── run_experiment.bat                     # Windows快速執行腳本
├── run_experiment.sh                      # Linux快速執行腳本
└── results/                               # 實驗結果目錄
    ├── fixed_iterations/                  # 固定迭代數實驗結果
    ├── time_budget/                       # 固定時間預算實驗結果
    ├── baseline_measurements/             # 基準時間測量結果
    └── analysis/                          # 分析結果和圖表
```

## 快速開始

### Windows
```cmd
cd experiment
run_experiment.bat
```

### Linux/Unix
```bash
cd experiment
chmod +x *.sh
./run_experiment.sh
```

## 實驗內容

### 測試實例
- **eil51** (51城市) - 最佳解: 426
- **kroA100** (100城市) - 最佳解: 21282  
- **kroA150** (150城市) - 最佳解: 26524
- **gr202** (202城市) - 最佳解: 40160

### 線程配置
- 1, 2, 4, 8 個線程

### 實驗類型
1. **固定迭代數實驗** - 比較相同迭代資源下的解品質
2. **固定時間預算實驗** - 比較相同時間下的解品質

### 模式選擇
- **開發模式**: 2迭代, 5次執行 (快速驗證)
- **正式模式**: 100迭代, 100次執行 (完整實驗)

## 重要注意事項

1. 確保 `data/` 目錄下有所有TSP實例檔案
2. 正式模式可能需要數小時執行時間
3. 建議先執行開發模式驗證設定
4. 需要足夠磁碟空間儲存結果檔案

## 結果分析

實驗完成後會生成：
- CSV格式的原始數據
- 統計總結報告
- 視覺化圖表 (PNG/PDF)
- 詳細分析結果

執行結果分析：
```bash
python analyze_results.py
```
