import requests
import os

import requests
import os

# TSPLIB 實例及其最優解文件 (重點測試 50, 100, 150, 202 規模)
tsplib_instances = [
    {
        "name": "eil51",
        "tsp_url": "https://raw.githubusercontent.com/coin-or/jorlib/master/jorlib-core/src/test/resources/tspLib/tsp/eil51.tsp",
        "opt_url": "https://raw.githubusercontent.com/coin-or/jorlib/master/jorlib-core/src/test/resources/tspLib/tsp/eil51.opt.tour",
        "optimal": 426,
        "description": "51-city problem (Christofides/Eilon) - ~50 規模"
    },
    {
        "name": "berlin52",
        "tsp_url": "https://raw.githubusercontent.com/coin-or/jorlib/master/jorlib-core/src/test/resources/tspLib/tsp/berlin52.tsp",
        "opt_url": "https://raw.githubusercontent.com/coin-or/jorlib/master/jorlib-core/src/test/resources/tspLib/tsp/berlin52.opt.tour",
        "optimal": 7542,
        "description": "52 locations in Berlin - ~50 規模"
    },
    {
        "name": "kroA100",
        "tsp_url": "https://raw.githubusercontent.com/coin-or/jorlib/master/jorlib-core/src/test/resources/tspLib/tsp/kroA100.tsp",
        "opt_url": "https://raw.githubusercontent.com/coin-or/jorlib/master/jorlib-core/src/test/resources/tspLib/tsp/kroA100.opt.tour",
        "optimal": 21282,
        "description": "100-city problem A - 100 規模"
    },
    {
        "name": "kroB100", 
        "tsp_url": "https://raw.githubusercontent.com/coin-or/jorlib/master/jorlib-core/src/test/resources/tspLib/tsp/kroB100.tsp",
        "opt_url": "https://raw.githubusercontent.com/coin-or/jorlib/master/jorlib-core/src/test/resources/tspLib/tsp/kroB100.opt.tour",
        "optimal": 22141,
        "description": "100-city problem B - 100 規模"
    },
    {
        "name": "kroA150",
        "tsp_url": "https://raw.githubusercontent.com/coin-or/jorlib/master/jorlib-core/src/test/resources/tspLib/tsp/kroA150.tsp",
        "opt_url": "https://raw.githubusercontent.com/coin-or/jorlib/master/jorlib-core/src/test/resources/tspLib/tsp/kroA150.opt.tour",
        "optimal": 26524,
        "description": "150-city problem A - 150 規模"
    },
    {
        "name": "kroB150",
        "tsp_url": "https://raw.githubusercontent.com/coin-or/jorlib/master/jorlib-core/src/test/resources/tspLib/tsp/kroB150.tsp",
        "opt_url": "https://raw.githubusercontent.com/coin-or/jorlib/master/jorlib-core/src/test/resources/tspLib/tsp/kroB150.opt.tour", 
        "optimal": 26130,
        "description": "150-city problem B - 150 規模"
    },
    {
        "name": "gr202",
        "tsp_url": "https://raw.githubusercontent.com/coin-or/jorlib/master/jorlib-core/src/test/resources/tspLib/tsp/gr202.tsp",
        "opt_url": "https://raw.githubusercontent.com/coin-or/jorlib/master/jorlib-core/src/test/resources/tspLib/tsp/gr202.opt.tour",
        "optimal": 40160,
        "description": "202-city problem - 202 規模"
    }
]

def download_file(url, filename, description):
    """下載單個文件"""
    try:
        print(f"📥 下載 {description} ...")
        response = requests.get(url, timeout=30)
        response.raise_for_status()
        
        with open(filename, "w", encoding="utf-8") as f:
            f.write(response.text)
        
        print(f"✓ {description}: 下載成功")
        return True
        
    except requests.exceptions.RequestException as e:
        print(f"✗ {description}: 下載失敗 - {e}")
        return False
    except Exception as e:
        print(f"✗ {description}: 處理失敗 - {e}")
        return False

def download_tsplib_instances():
    """下載 TSPLIB 實例和最優解文件到 data 目錄"""
    
    # 確保 data 目錄存在
    data_dir = "data"
    if not os.path.exists(data_dir):
        os.makedirs(data_dir)
        print(f"創建目錄: {data_dir}")
    
    # 創建統計文件
    with open(os.path.join(data_dir, "tsplib_optimal_solutions.txt"), "w", encoding="utf-8") as f:
        f.write("TSPLIB 實例最優解參考\n")
        f.write("=" * 50 + "\n\n")
        
        print("開始下載 TSPLIB 實例和最優解...")
        print("=" * 50)
        
        for instance in tsplib_instances:
            name = instance["name"]
            tsp_url = instance["tsp_url"]
            opt_url = instance["opt_url"]
            optimal = instance["optimal"]
            description = instance["description"]
            
            tsp_filename = os.path.join(data_dir, f"{name}.tsp")
            opt_filename = os.path.join(data_dir, f"{name}.opt.tour")
            
            print(f"\n處理實例: {name}")
            
            # 寫入統計信息
            f.write(f"實例: {name}\n")
            f.write(f"描述: {description}\n")
            f.write(f"最優解: {optimal}\n")
            f.write(f"TSP文件: {tsp_filename}\n")
            f.write(f"最優路徑: {opt_filename}\n")
            f.write("-" * 30 + "\n\n")
            
            # 下載 TSP 文件
            if os.path.exists(tsp_filename):
                print(f"✓ {name}.tsp: 文件已存在，跳過下載")
            else:
                download_file(tsp_url, tsp_filename, f"{name}.tsp")
            
            # 下載最優解文件
            if os.path.exists(opt_filename):
                print(f"✓ {name}.opt.tour: 文件已存在，跳過下載")
            else:
                download_file(opt_url, opt_filename, f"{name}.opt.tour")
    
    print("\n" + "=" * 50)
    print("下載完成！")
    print(f"檢查 {data_dir}/tsplib_optimal_solutions.txt 查看所有實例的最優解")
    print(f"TSP 文件: *.tsp")
    print(f"最優路徑: *.opt.tour")

if __name__ == "__main__":
    download_tsplib_instances()
