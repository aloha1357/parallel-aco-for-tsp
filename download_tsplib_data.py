#!/usr/bin/env python3
"""
TSPLIB數據下載腳本
自動下載標準TSP測試實例
"""

import os
import requests
import zipfile
from pathlib import Path

# TSPLIB數據源
TSPLIB_BASE_URL = "http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/tsp/"

# 標準測試實例
STANDARD_INSTANCES = {
    # 小規模問題 (≤60 城市)
    "berlin52.tsp": 7542,
    "eil51.tsp": 426,
    "st70.tsp": 675,
    
    # 中規模問題 (60-120 城市)
    "eil76.tsp": 538,
    "pr76.tsp": 108159,
    "kroA100.tsp": 21282,
    "lin105.tsp": 14379,
    
    # 大規模問題 (120+ 城市)
    "pr124.tsp": 59030,
    "ch130.tsp": 6110,
    "pr144.tsp": 58537,
    "kroA150.tsp": 26524
}

def download_file(url, filename, data_dir):
    """下載單個檔案"""
    filepath = data_dir / filename
    
    if filepath.exists():
        print(f"✓ {filename} 已存在，跳過下載")
        return True
    
    try:
        print(f"下載 {filename}...", end=" ")
        response = requests.get(url, timeout=30)
        response.raise_for_status()
        
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(response.text)
        
        print("✓ 完成")
        return True
        
    except Exception as e:
        print(f"✗ 失敗: {e}")
        return False

def create_backup_data(data_dir):
    """創建備用測試數據（如果下載失敗）"""
    print("\n正在創建備用測試數據...")
    
    # 創建一個簡單的berlin52.tsp檔案
    berlin52_content = """NAME : berlin52
COMMENT : 52 locations in Berlin (Groetschel)
TYPE : TSP
DIMENSION : 52
EDGE_WEIGHT_TYPE : EUC_2D
NODE_COORD_SECTION
1 565.0 575.0
2 25.0 185.0
3 345.0 750.0
4 945.0 685.0
5 845.0 655.0
6 880.0 660.0
7 25.0 230.0
8 525.0 1000.0
9 580.0 1175.0
10 650.0 1130.0
11 1605.0 620.0
12 1220.0 580.0
13 1465.0 200.0
14 1530.0 5.0
15 845.0 680.0
16 725.0 370.0
17 145.0 665.0
18 415.0 635.0
19 510.0 875.0
20 560.0 365.0
21 300.0 465.0
22 520.0 585.0
23 480.0 415.0
24 835.0 625.0
25 975.0 580.0
26 1215.0 245.0
27 1320.0 315.0
28 1250.0 400.0
29 660.0 180.0
30 410.0 250.0
31 420.0 555.0
32 575.0 665.0
33 1150.0 1160.0
34 700.0 580.0
35 685.0 595.0
36 685.0 610.0
37 770.0 610.0
38 795.0 645.0
39 720.0 635.0
40 760.0 650.0
41 475.0 960.0
42 95.0 260.0
43 875.0 920.0
44 700.0 500.0
45 555.0 815.0
46 830.0 485.0
47 1170.0 65.0
48 830.0 610.0
49 605.0 625.0
50 595.0 360.0
51 1340.0 725.0
52 1740.0 245.0
EOF
"""
    
    filepath = data_dir / "berlin52.tsp"
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(berlin52_content)
    
    print("✓ 備用數據創建完成")

def main():
    print("=== TSPLIB數據下載器 ===")
    
    # 創建data目錄
    data_dir = Path("data")
    data_dir.mkdir(exist_ok=True)
    
    print(f"數據目錄: {data_dir.absolute()}")
    print(f"待下載檔案: {len(STANDARD_INSTANCES)} 個")
    print()
    
    success_count = 0
    
    # 下載每個檔案
    for filename, optimal in STANDARD_INSTANCES.items():
        url = f"{TSPLIB_BASE_URL}{filename}"
        if download_file(url, filename, data_dir):
            success_count += 1
    
    print(f"\n=== 下載完成 ===")
    print(f"成功: {success_count}/{len(STANDARD_INSTANCES)}")
    
    # 如果下載成功率太低，創建備用數據
    if success_count < len(STANDARD_INSTANCES) * 0.5:
        print("下載成功率較低，創建備用測試數據...")
        create_backup_data(data_dir)
    
    # 驗證檔案
    print("\n檔案驗證:")
    for filename in STANDARD_INSTANCES.keys():
        filepath = data_dir / filename
        if filepath.exists():
            size = filepath.stat().st_size
            print(f"✓ {filename} ({size} bytes)")
        else:
            print(f"✗ {filename} 缺失")
    
    print("\n使用方法:")
    print("1. 編譯項目: mkdir build && cd build && cmake .. && make")
    print("2. 運行實驗: ./tsplib_benchmark_experiment")
    print("3. 查看結果: experiment_results/目錄")

if __name__ == "__main__":
    main()
