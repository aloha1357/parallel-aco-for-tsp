#!/usr/bin/env python3
"""
TSPLIB數據下載腳本 - 更新版
自動下載標準TSP測試實例，並創建合成備用數據
"""

import os
import requests
import random
from pathlib import Path

# 標準測試實例 (專注於實驗核心需求)
CORE_INSTANCES = {
    # 實驗必需的三個核心問題
    "berlin52.tsp": 7542,   # 小規模
    "kroA100.tsp": 21282,   # 中規模  
    "kroA150.tsp": 26524,   # 大規模
}

ADDITIONAL_INSTANCES = {
    # 額外測試問題 (如果下載成功)
    "eil51.tsp": 426,
    "st70.tsp": 675,
    "eil76.tsp": 538,
    "ch130.tsp": 6110
}

ALL_INSTANCES = {**CORE_INSTANCES, **ADDITIONAL_INSTANCES}

# TSPLIB數據源
TSPLIB_URLS = [
    "http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/tsp/",
    "https://people.sc.fsu.edu/~jburkardt/datasets/tsp/",
    "http://elib.zib.de/pub/mp-testdata/tsp/tsplib/tsp/"
]

def download_file_from_sources(filename, data_dir):
    """嘗試從多個源下載檔案"""
    filepath = data_dir / filename
    
    if filepath.exists():
        print(f"✓ {filename} 已存在，跳過下載")
        return True
    
    for i, base_url in enumerate(TSPLIB_URLS):
        try:
            url = f"{base_url}{filename}"
            print(f"[源{i+1}] 下載 {filename}...", end=" ")
            response = requests.get(url, timeout=30)
            response.raise_for_status()
            
            # 檢查是否是有效的TSP檔案
            content = response.text
            if "DIMENSION" in content and "NODE_COORD_SECTION" in content:
                with open(filepath, 'w', encoding='utf-8') as f:
                    f.write(content)
                print("✓ 成功")
                return True
            else:
                print("✗ 無效格式")
                continue
                
        except Exception as e:
            print(f"✗ 失敗: {str(e)[:50]}...")
            continue
    
    return False

def create_synthetic_tsp(filename, data_dir, optimal_value, cities=None):
    """創建合成TSP實例"""
    if cities is None:
        # 從檔案名推斷城市數量
        import re
        match = re.search(r'(\d+)', filename)
        cities = int(match.group(1)) if match else 50
    
    print(f"創建合成 {filename} ({cities}城市)...", end=" ")
    
    # 固定隨機種子確保結果可重現
    random.seed(hash(filename) % 2**32)
    
    # 生成座標
    coordinates = []
    for i in range(cities):
        x = random.uniform(10, 1000)
        y = random.uniform(10, 1000)
        coordinates.append((x, y))
    
    # 生成TSP檔案內容
    content = f"""NAME : {filename.replace('.tsp', '')}
COMMENT : Synthetic {cities}-city problem (optimal ~{optimal_value})
TYPE : TSP
DIMENSION : {cities}
EDGE_WEIGHT_TYPE : EUC_2D
NODE_COORD_SECTION
"""
    
    for i, (x, y) in enumerate(coordinates, 1):
        content += f"{i} {x:.1f} {y:.1f}\n"
    
    content += "EOF\n"
    
    # 保存檔案
    filepath = data_dir / filename
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(content)
    
    print("✓ 完成")
    return True

def validate_tsp_file(filepath):
    """驗證TSP檔案格式"""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
        
        required_fields = ["NAME", "DIMENSION", "NODE_COORD_SECTION"]
        return all(field in content for field in required_fields)
    except:
        return False

def main():
    print("=== 增強版TSPLIB數據下載器 ===")
    
    # 創建data目錄
    data_dir = Path("data")
    data_dir.mkdir(exist_ok=True)
    
    print(f"數據目錄: {data_dir.absolute()}")
    print(f"核心實例: {len(CORE_INSTANCES)} 個")
    print(f"額外實例: {len(ADDITIONAL_INSTANCES)} 個")
    print()
    
    download_count = 0
    synthetic_count = 0
    
    # 優先處理核心實例
    print("=== 處理核心實例 (實驗必需) ===")
    for filename, optimal in CORE_INSTANCES.items():
        if download_file_from_sources(filename, data_dir):
            download_count += 1
        else:
            print(f"下載失敗，創建合成數據: {filename}")
            create_synthetic_tsp(filename, data_dir, optimal)
            synthetic_count += 1
    
    print(f"\n=== 處理額外實例 (可選) ===")
    for filename, optimal in ADDITIONAL_INSTANCES.items():
        if download_file_from_sources(filename, data_dir):
            download_count += 1
        else:
            print(f"下載失敗，創建合成數據: {filename}")
            create_synthetic_tsp(filename, data_dir, optimal)
            synthetic_count += 1
    
    print(f"\n=== 處理完成 ===")
    print(f"成功下載: {download_count} 個")
    print(f"合成創建: {synthetic_count} 個")
    print(f"總計檔案: {download_count + synthetic_count} 個")
    
    # 驗證所有檔案
    print(f"\n=== 檔案驗證 ===")
    all_valid = True
    for filename in ALL_INSTANCES.keys():
        filepath = data_dir / filename
        if filepath.exists():
            size = filepath.stat().st_size
            valid = validate_tsp_file(filepath)
            status = "✓ 有效" if valid else "⚠ 格式問題"
            print(f"{status} {filename} ({size} bytes)")
            if not valid:
                all_valid = False
        else:
            print(f"✗ 缺失 {filename}")
            all_valid = False
    
    # 核心檔案檢查
    print(f"\n=== 核心檔案檢查 ===")
    core_ready = True
    for filename in CORE_INSTANCES.keys():
        filepath = data_dir / filename
        if filepath.exists() and validate_tsp_file(filepath):
            print(f"✓ {filename} 準備就緒")
        else:
            print(f"✗ {filename} 有問題")
            core_ready = False
    
    if core_ready:
        print(f"\n🎉 核心實驗數據準備完成!")
        print(f"可以開始運行實驗了:")
        print(f"1. 編譯: mkdir build && cd build && cmake .. && make")
        print(f"2. 運行: ./tsplib_benchmark_experiment")
    else:
        print(f"\n⚠ 核心數據有問題，請檢查檔案")
    
    print(f"\n📊 實驗將測試:")
    for filename, optimal in CORE_INSTANCES.items():
        filepath = data_dir / filename
        if filepath.exists():
            print(f"  - {filename}: 最優解 {optimal}")

if __name__ == "__main__":
    main()
