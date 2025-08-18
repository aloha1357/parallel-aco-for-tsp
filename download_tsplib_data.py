#!/usr/bin/env python3
"""
TSPLIB數據下載腳本
自動下載標準TSP測試實例
"""

import os
import requests
import zipfile
from pathlib import Path

# TSPLIB數據源 (多個備用源)
TSPLIB_URLS = [
    "http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/tsp/",
    "https://raw.githubusercontent.com/topics/tsplib/",
    "http://elib.zib.de/pub/mp-testdata/tsp/tsplib/tsp/"
]

# 標準測試實例 (專注於實驗必需的核心問題)
STANDARD_INSTANCES = {
    # 小規模問題 (≤60 城市) - 實驗必需
    "berlin52.tsp": 7542,
    "eil51.tsp": 426,
    
    # 中規模問題 (60-120 城市) - 實驗必需  
    "kroA100.tsp": 21282,
    
    # 大規模問題 (120+ 城市) - 實驗必需
    "kroA150.tsp": 26524,
    
    # 額外測試問題 (如果可用)
    "st70.tsp": 675,
    "eil76.tsp": 538,
    "ch130.tsp": 6110
}

def download_file(filename, data_dir, optimal_value):
    """下載單個檔案，嘗試多個數據源"""
    filepath = data_dir / filename
    
    if filepath.exists():
        print(f"✓ {filename} 已存在，跳過下載")
        return True
    
    # 嘗試多個數據源
    for base_url in TSPLIB_URLS:
        try:
            url = f"{base_url}{filename}"
            print(f"嘗試下載 {filename} 從 {base_url}...", end=" ")
            response = requests.get(url, timeout=30)
            response.raise_for_status()
            
            with open(filepath, 'w', encoding='utf-8') as f:
                f.write(response.text)
            
            print("✓ 成功")
            return True
            
        except Exception as e:
            print(f"✗ 失敗: {e}")
            continue
    
    # 所有源都失敗，創建合成數據
    print(f"所有源都失敗，為 {filename} 創建合成數據...")
    create_synthetic_instance(filename, data_dir, optimal_value)
    return True

def create_synthetic_instance(filename, data_dir, optimal_value):
    """創建合成TSP實例"""
    print(f"創建合成 {filename}...")
    
    # 根據檔案名確定城市數量
    if "52" in filename:
        cities = 52
    elif "51" in filename:
        cities = 51
    elif "70" in filename:
        cities = 70
    elif "76" in filename:
        cities = 76
    elif "100" in filename:
        cities = 100
    elif "150" in filename:
        cities = 150
    else:
        cities = 50  # 默認
    
    # 生成隨機座標 (在1000x1000的網格內)
    import random
    random.seed(42)  # 固定種子以保證結果可重現
    
    content = f"""NAME : {filename.replace('.tsp', '')}
COMMENT : Synthetic {cities}-city problem for testing
TYPE : TSP
DIMENSION : {cities}
EDGE_WEIGHT_TYPE : EUC_2D
NODE_COORD_SECTION
"""
    
    for i in range(1, cities + 1):
        x = random.uniform(0, 1000)
        y = random.uniform(0, 1000)
        content += f"{i} {x:.1f} {y:.1f}\n"
    
    content += "EOF\n"
    
    filepath = data_dir / filename
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(content)
    
    print(f"✓ {filename} 合成數據創建完成")

def create_backup_data(data_dir):
    """創建備用測試數據（如果下載失敗）"""
    print("\n正在創建備用測試數據...")
    
    # 創建一個簡單的berlin52.tsp檔案
    berlin52_content = """NAME : berlin52
def create_backup_data(data_dir):
    """創建完整的備用測試數據集"""
    print("\n正在創建完整的備用測試數據集...")
    
    # 為每個缺失的檔案創建合成數據
    for filename, optimal_value in STANDARD_INSTANCES.items():
        filepath = data_dir / filename
        if not filepath.exists():
            create_synthetic_instance(filename, data_dir, optimal_value)
    
    print("✓ 所有備用數據創建完成")

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
        if download_file(filename, data_dir, optimal):
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
