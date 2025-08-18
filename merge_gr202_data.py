#!/usr/bin/env python3
"""
合并gr202数据到4实例数据集
用gr202替换ch130，创建完整的50-100-150-200城市数据集
"""

import pandas as pd
import glob
from datetime import datetime

def merge_gr202_data():
    # 读取包含4个实例的旧数据（eil51, kroA100, ch130, kroA150）
    old_files = glob.glob('build/results/fixed_iterations/*.csv')
    old_4_instance_file = None
    for f in old_files:
        df = pd.read_csv(f)
        if len(df['instance_name'].unique()) == 4:
            old_4_instance_file = f
            break
    
    if not old_4_instance_file:
        print("Error: No 4-instance file found")
        return
    
    print(f"Reading old 4-instance data from: {old_4_instance_file}")
    old_data = pd.read_csv(old_4_instance_file)
    
    # 读取最新的gr202数据
    new_files = sorted(glob.glob('build/results/fixed_iterations/*.csv'))
    new_gr202_file = new_files[-1]  # 最新的文件
    
    print(f"Reading new gr202 data from: {new_gr202_file}")
    gr202_data = pd.read_csv(new_gr202_file)
    
    # 验证gr202数据
    if 'gr202' not in gr202_data['instance_name'].unique():
        print("Error: No gr202 data found in the new file")
        return
    
    # 移除ch130数据，保留eil51, kroA100, kroA150
    filtered_old_data = old_data[old_data['instance_name'] != 'ch130'].copy()
    
    print("Old data instances:", old_data['instance_name'].unique())
    print("Filtered old data instances:", filtered_old_data['instance_name'].unique())
    print("New gr202 data instances:", gr202_data['instance_name'].unique())
    
    # 合并数据
    merged_data = pd.concat([filtered_old_data, gr202_data], ignore_index=True)
    
    # 验证合并结果
    final_instances = sorted(merged_data['instance_name'].unique())
    print("Final merged instances:", final_instances)
    
    # 按实例和算法排序
    merged_data = merged_data.sort_values(['instance_name', 'algorithm', 'run_number'])
    
    # 保存合并后的数据
    timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
    output_file = f'build/results/fixed_iterations/merged_4_instances_{timestamp}.csv'
    merged_data.to_csv(output_file, index=False)
    
    print(f"\nMerged data saved to: {output_file}")
    print(f"Total records: {len(merged_data)}")
    print("Records per instance:")
    for instance in final_instances:
        count = len(merged_data[merged_data['instance_name'] == instance])
        print(f"  {instance}: {count} records")

if __name__ == "__main__":
    merge_gr202_data()
