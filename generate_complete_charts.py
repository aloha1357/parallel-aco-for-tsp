#!/usr/bin/env python3
"""
使用合并后的数据（包含gr202）生成4实例性能图表
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import glob
from datetime import datetime

def generate_gr202_charts():
    # 查找合并后的数据文件
    merged_files = glob.glob('build/results/fixed_iterations/merged_4_instances_*.csv')
    if not merged_files:
        print("Error: No merged data file found")
        return
    
    # 使用最新的合并文件
    data_file = sorted(merged_files)[-1]
    print(f"Using data file: {data_file}")
    
    df = pd.read_csv(data_file)
    
    # 验证数据
    instances = sorted(df['instance_name'].unique())
    algorithms = sorted(df['algorithm'].unique())
    
    print("Instances:", instances)
    print("Algorithms:", algorithms)
    
    # 期望的实例名称映射（按城市数量排序）
    instance_map = {
        'eil51': '51 cities (eil51)',
        'kroA100': '100 cities (kroA100)', 
        'kroA150': '150 cities (kroA150)',
        'gr202': '202 cities (gr202)'
    }
    
    # 确保实例按正确顺序
    ordered_instances = ['eil51', 'kroA100', 'kroA150', 'gr202']
    
    # 创建4个子图
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(16, 12))
    axes = [ax1, ax2, ax3, ax4]
    
    colors = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728']  # 蓝、橙、绿、红
    thread_counts = [1, 2, 4, 8]
    
    for idx, instance in enumerate(ordered_instances):
        ax = axes[idx]
        instance_data = df[df['instance_name'] == instance]
        
        if len(instance_data) == 0:
            print(f"Warning: No data for instance {instance}")
            continue
        
        # 计算每个算法的平均执行时间
        avg_times = []
        for alg in algorithms:
            alg_data = instance_data[instance_data['algorithm'] == alg]
            avg_time = alg_data['wall_time_s'].mean()
            avg_times.append(avg_time)
        
        # 绘制柱状图
        bars = ax.bar(range(len(algorithms)), avg_times, color=colors, alpha=0.7, edgecolor='black')
        
        # 设置图表
        ax.set_title(f'{instance_map[instance]}', fontsize=14, fontweight='bold')
        ax.set_xlabel('Algorithm (Thread Count)', fontsize=12)
        ax.set_ylabel('Average Time (seconds)', fontsize=12)
        ax.set_xticks(range(len(algorithms)))
        ax.set_xticklabels([f'{t}T' for t in thread_counts])
        ax.grid(True, alpha=0.3)
        
        # 添加数值标签
        for bar, time in zip(bars, avg_times):
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2., height + height*0.01,
                   f'{time:.3f}s', ha='center', va='bottom', fontsize=10)
        
        # 设置y轴范围
        ax.set_ylim(0, max(avg_times) * 1.15)
    
    plt.tight_layout()
    plt.suptitle('Thread Performance Comparison - Complete 4 Instances\n(51, 100, 150, 202 cities)', 
                 fontsize=16, fontweight='bold', y=1.02)
    
    # 保存图表
    timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
    output_file = f'build/complete_4_instances_with_gr202_{timestamp}.png'
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"\nChart saved to: {output_file}")
    
    # 生成加速比图表
    fig2, ax = plt.subplots(1, 1, figsize=(12, 8))
    
    for idx, instance in enumerate(ordered_instances):
        instance_data = df[df['instance_name'] == instance]
        
        if len(instance_data) == 0:
            continue
        
        # 计算加速比（相对于单线程）
        serial_time = instance_data[instance_data['algorithm'] == 'Serial_ACO']['wall_time_s'].mean()
        speedups = []
        
        for alg in algorithms:
            alg_data = instance_data[instance_data['algorithm'] == alg]
            avg_time = alg_data['wall_time_s'].mean()
            speedup = serial_time / avg_time
            speedups.append(speedup)
        
        # 绘制线图
        ax.plot(thread_counts, speedups, marker='o', linewidth=2, markersize=8, 
                label=instance_map[instance], color=colors[idx])
    
    # 添加理想加速比线
    ax.plot(thread_counts, thread_counts, '--', color='gray', alpha=0.7, label='Ideal Speedup')
    
    ax.set_title('Speedup Comparison Across Instances', fontsize=16, fontweight='bold')
    ax.set_xlabel('Number of Threads', fontsize=12)
    ax.set_ylabel('Speedup (vs Serial)', fontsize=12)
    ax.set_xticks(thread_counts)
    ax.grid(True, alpha=0.3)
    ax.legend(fontsize=11)
    ax.set_ylim(0.5, max(thread_counts) + 0.5)
    
    # 保存加速比图表
    speedup_file = f'build/speedup_comparison_with_gr202_{timestamp}.png'
    plt.savefig(speedup_file, dpi=300, bbox_inches='tight')
    print(f"Speedup chart saved to: {speedup_file}")
    
    plt.show()
    
    # 打印统计信息
    print("\n=== Performance Summary ===")
    for instance in ordered_instances:
        instance_data = df[df['instance_name'] == instance]
        if len(instance_data) == 0:
            continue
        
        print(f"\n{instance_map[instance]}:")
        serial_time = instance_data[instance_data['algorithm'] == 'Serial_ACO']['wall_time_s'].mean()
        
        for alg in algorithms:
            alg_data = instance_data[instance_data['algorithm'] == alg]
            avg_time = alg_data['wall_time_s'].mean()
            speedup = serial_time / avg_time
            
            threads = alg_data['num_threads'].iloc[0]
            print(f"  {threads} threads: {avg_time:.3f}s (speedup: {speedup:.2f}x)")

if __name__ == "__main__":
    generate_gr202_charts()
