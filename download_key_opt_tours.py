import requests
import os

# 從 GitHub 下載特定實例的 .opt.tour 文件
github_opt_tours = [
    {
        "name": "eil51",
        "url": "https://raw.githubusercontent.com/coin-or/jorlib/master/jorlib-core/src/test/resources/tspLib/tsp/eil51.opt.tour",
        "optimal": 426
    },
    {
        "name": "kroA100", 
        "url": "https://raw.githubusercontent.com/coin-or/jorlib/master/jorlib-core/src/test/resources/tspLib/tsp/kroA100.opt.tour",
        "optimal": 21282
    },
    {
        "name": "kroA150",
        "url": "https://raw.githubusercontent.com/coin-or/jorlib/master/jorlib-core/src/test/resources/tspLib/tsp/kroA150.opt.tour", 
        "optimal": 26524
    },
    {
        "name": "kroA200",
        "url": "https://raw.githubusercontent.com/coin-or/jorlib/master/jorlib-core/src/test/resources/tspLib/tsp/kroA200.opt.tour",
        "optimal": 29368
    }
]

def download_opt_tour(name, url, data_dir):
    """下載單個 .opt.tour 文件"""
    filename = os.path.join(data_dir, f"{name}.opt.tour")
    
    if os.path.exists(filename):
        print(f"✓ {name}.opt.tour 已存在，跳過下載")
        return True
    
    try:
        print(f"📥 下載 {name}.opt.tour ...")
        response = requests.get(url, timeout=30)
        response.raise_for_status()
        
        with open(filename, "w", encoding="utf-8") as f:
            f.write(response.text)
        
        print(f"✓ {name}.opt.tour: 下載成功")
        return True
        
    except requests.exceptions.RequestException as e:
        print(f"✗ {name}.opt.tour: 下載失敗 - {e}")
        return False

def main():
    """下載四個重點實例的最優解文件"""
    
    data_dir = "data"
    if not os.path.exists(data_dir):
        os.makedirs(data_dir)
        print(f"創建目錄: {data_dir}")
    
    print("=" * 50)
    print("下載四個重點實例的最優解文件")
    print("eil51 (50城市), kroA100 (100城市), kroA150 (150城市), kroA200 (200城市)")
    print("=" * 50)
    
    success_count = 0
    
    for instance in github_opt_tours:
        if download_opt_tour(instance["name"], instance["url"], data_dir):
            success_count += 1
    
    print(f"\n下載完成！成功: {success_count}/{len(github_opt_tours)}")
    
    # 檢查對應的 TSP 文件是否存在
    print("\n檢查 TSP 文件和最優解配對:")
    for instance in github_opt_tours:
        name = instance["name"]
        tsp_file = os.path.join(data_dir, f"{name}.tsp")
        opt_file = os.path.join(data_dir, f"{name}.opt.tour")
        
        tsp_exists = os.path.exists(tsp_file)
        opt_exists = os.path.exists(opt_file)
        
        if tsp_exists and opt_exists:
            print(f"✓ {name}: 完整配對 (最優解: {instance['optimal']})")
        elif tsp_exists:
            print(f"△ {name}: 只有 TSP 文件，缺少最優解")
        elif opt_exists:
            print(f"△ {name}: 只有最優解文件，缺少 TSP 文件")
        else:
            print(f"✗ {name}: 缺少 TSP 和最優解文件")

if __name__ == "__main__":
    main()
