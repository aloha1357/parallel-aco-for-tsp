#!/usr/bin/env python3
"""
Simple Four-Instance Visualization
生成简单易查看的4实例性能图表
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path
import glob

# 设置中文字体
plt.rcParams['font.sans-serif'] = ['Microsoft YaHei', 'SimHei', 'Arial Unicode MS']
plt.rcParams['axes.unicode_minus'] = False

def load_latest_results():
    """加载最新的实验结果"""
    results_dir = Path('results/fixed_iterations')
    csv_files = glob.glob(str(results_dir / 'fixed_iterations_results_*.csv'))
    
    if not csv_files:
        print("No results found!")
        return None
        
    latest_file = max(csv_files, key=lambda x: Path(x).stat().st_mtime)
    print(f"Loading: {latest_file}")
    
    df = pd.read_csv(latest_file)
    # 只保留4个实例
    instances = ['eil51', 'kroA100', 'kroA150', 'gr202']
    df = df[df['instance_name'].isin(instances)]
    
    return df

def create_simple_charts(df):
    """创建简单的4个图表"""
    
    instances = ['eil51', 'kroA100', 'kroA150', 'gr202']
    algorithms = ['Serial_ACO', 'Parallel_ACO_2', 'Parallel_ACO_4', 'Parallel_ACO_8']
    
    # 创建2x2的子图
    fig, axes = plt.subplots(2, 2, figsize=(15, 12))
    fig.suptitle('四实例线程性能对比', fontsize=16, fontweight='bold')
    
    colors = ['#FF6B6B', '#4ECDC4', '#45B7D1', '#96CEB4']
    
    for i, instance in enumerate(instances):
        row = i // 2
        col = i % 2
        ax = axes[row, col]
        
        # 过滤当前实例的数据
        instance_data = df[df['instance_name'] == instance]
        
        if instance_data.empty:
            ax.text(0.5, 0.5, f'{instance}\n无数据', 
                   ha='center', va='center', transform=ax.transAxes)
            ax.set_title(f'{instance} (无数据)')
            continue
        
        # 计算每个算法的平均时间
        avg_times = []
        labels = []
        bar_colors = []
        
        for j, alg in enumerate(algorithms):
            alg_data = instance_data[instance_data['algorithm'] == alg]
            if not alg_data.empty:
                avg_time = alg_data['wall_time_s'].mean()
                avg_times.append(avg_time)
                
                # 简化标签
                if alg == 'Serial_ACO':
                    labels.append('串行')
                else:
                    thread_num = alg.split('_')[-1]
                    labels.append(f'{thread_num}线程')
                
                bar_colors.append(colors[j])
        
        if avg_times:
            # 绘制条形图
            bars = ax.bar(labels, avg_times, color=bar_colors, alpha=0.8)
            
            # 添加数值标签
            for bar, time in zip(bars, avg_times):
                height = bar.get_height()
                ax.text(bar.get_x() + bar.get_width()/2., height + height*0.01,
                       f'{time:.3f}s', ha='center', va='bottom', fontsize=9)
            
            # 计算加速比
            if len(avg_times) > 1:
                serial_time = avg_times[0]
                speedups = [serial_time / time for time in avg_times[1:]]
                
                # 在右上角显示最佳加速比
                if speedups:
                    best_speedup = max(speedups)
                    best_idx = speedups.index(best_speedup) + 1
                    ax.text(0.98, 0.98, f'最佳加速比: {best_speedup:.2f}x\n({labels[best_idx]})', 
                           transform=ax.transAxes, ha='right', va='top',
                           bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8),
                           fontsize=8)
        
        # 获取实例信息
        instance_info = {
            'eil51': {'size': 51, 'type': '小规模'},
            'kroA100': {'size': 100, 'type': '中等规模'},
            'kroA150': {'size': 150, 'type': '大规模'},
            'gr202': {'size': 202, 'type': '超大规模'}
        }
        
        info = instance_info.get(instance, {'size': '?', 'type': '未知'})
        ax.set_title(f'{instance} ({info["size"]}城市 - {info["type"]})', fontweight='bold')
        ax.set_ylabel('执行时间 (秒)')
        ax.set_xlabel('算法配置')
        ax.grid(True, alpha=0.3)
        
        # 旋转x轴标签避免重叠
        plt.setp(ax.get_xticklabels(), rotation=45, ha='right')
    
    plt.tight_layout()
    return fig

def create_speedup_chart(df):
    """创建加速比对比图"""
    instances = ['eil51', 'kroA100', 'kroA150', 'gr202']
    algorithms = ['Parallel_ACO_2', 'Parallel_ACO_4', 'Parallel_ACO_8']
    
    fig, ax = plt.subplots(figsize=(12, 8))
    
    speedup_data = []
    
    for instance in instances:
        instance_data = df[df['instance_name'] == instance]
        
        # 获取串行时间
        serial_data = instance_data[instance_data['algorithm'] == 'Serial_ACO']
        if serial_data.empty:
            continue
        serial_time = serial_data['wall_time_s'].mean()
        
        instance_speedups = [instance]
        
        for alg in algorithms:
            alg_data = instance_data[instance_data['algorithm'] == alg]
            if not alg_data.empty:
                parallel_time = alg_data['wall_time_s'].mean()
                speedup = serial_time / parallel_time
                instance_speedups.append(speedup)
            else:
                instance_speedups.append(0)
        
        speedup_data.append(instance_speedups)
    
    if speedup_data:
        # 转换为DataFrame便于绘图
        speedup_df = pd.DataFrame(speedup_data, 
                                 columns=['Instance', '2线程', '4线程', '8线程'])
        speedup_df = speedup_df.set_index('Instance')
        
        # 绘制分组条形图
        speedup_df.plot(kind='bar', ax=ax, color=['#4ECDC4', '#45B7D1', '#96CEB4'])
        
        # 添加理想加速比线
        x_pos = np.arange(len(instances))
        ax.plot(x_pos, [2]*len(instances), 'r--', alpha=0.7, label='理想2x')
        ax.plot(x_pos, [4]*len(instances), 'r--', alpha=0.7, label='理想4x') 
        ax.plot(x_pos, [8]*len(instances), 'r--', alpha=0.7, label='理想8x')
    
    ax.set_title('不同实例的线程加速比对比', fontsize=14, fontweight='bold')
    ax.set_ylabel('加速比')
    ax.set_xlabel('TSP实例')
    ax.legend()
    ax.grid(True, alpha=0.3)
    plt.xticks(rotation=0)
    
    plt.tight_layout()
    return fig

def main():
    print("=== 生成简单四实例可视化图表 ===")
    
    # 加载数据
    df = load_latest_results()
    if df is None:
        return
    
    print(f"加载了 {len(df)} 条记录")
    
    # 生成主要图表
    fig1 = create_simple_charts(df)
    fig1.savefig('four_instances_simple.png', dpi=150, bbox_inches='tight')
    print("✓ 保存: four_instances_simple.png")
    
    # 生成加速比图表
    fig2 = create_speedup_chart(df)
    fig2.savefig('speedup_comparison.png', dpi=150, bbox_inches='tight')
    print("✓ 保存: speedup_comparison.png")
    
    # 显示图片（如果可能）
    plt.show()
    
    print("\n=== 图表生成完成 ===")
    print("生成的文件:")
    print("- four_instances_simple.png (4实例性能对比)")
    print("- speedup_comparison.png (加速比对比)")

if __name__ == "__main__":
    main()
