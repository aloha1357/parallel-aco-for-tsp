# 手動創建 kroA150 和 kroA200 的最優解文件
# 這些是從 TSPLIB 文獻中已知的最優解

import os

# kroA150 的已知最優解（簡化版本，僅用於長度驗證）
kroa150_tour = list(range(1, 151))  # 簡單的 1-150 順序，不是真正的最優路徑
kroa200_tour = list(range(1, 201))  # 簡單的 1-200 順序，不是真正的最優路徑

def create_placeholder_opt_tour(name, dimension, optimal_length):
    """創建占位符最優解文件"""
    filename = f"data/{name}.opt.tour"
    
    if os.path.exists(filename):
        print(f"✓ {name}.opt.tour 已存在")
        return
    
    with open(filename, "w") as f:
        f.write(f"NAME : {name}.opt.tour\n")
        f.write(f"COMMENT : Placeholder optimal tour for {name} (Known optimal: {optimal_length})\n")
        f.write("TYPE : TOUR\n")
        f.write(f"DIMENSION : {dimension}\n")
        f.write("TOUR_SECTION\n")
        
        # 寫入簡單的順序路徑（不是真正的最優解，但可以用來測試算法框架）
        for i in range(1, dimension + 1):
            f.write(f"{i}\n")
        
        f.write("-1\n")
        f.write("EOF\n")
    
    print(f"✓ 創建 {name}.opt.tour (占位符，已知最優: {optimal_length})")

if __name__ == "__main__":
    print("創建缺失的最優解占位符文件...")
    
    # 確保 data 目錄存在
    os.makedirs("data", exist_ok=True)
    
    # 創建占位符文件
    create_placeholder_opt_tour("kroA150", 150, 26524)
    create_placeholder_opt_tour("kroA200", 200, 29368)
    
    print("完成！")
