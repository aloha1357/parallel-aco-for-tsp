#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
ACO TSP 性能分析圖表生成腳本
"""

import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.font_manager as fm
import numpy as np
import seaborn as sns

# 設置中文字體
plt.rcParams['font.sans-serif'] = ['SimHei', 'DejaVu Sans']
plt.rcParams['axes.unicode_minus'] = False

# 設置圖表風格
sns.set_style("whitegrid")
plt.style.use('seaborn-v0_8')

def plot_scalability_analysis():
    """繪製可擴展性分析圖表"""
    df = pd.read_csv('scalability_results.csv')
    
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 12))
    
    # 1. 執行時間 vs 線程數
    ax1.plot(df['Thread_Count'], df['Execution_Time_ms'], 'bo-', linewidth=2, markersize=8)
    ax1.set_xlabel('線程數')
    ax1.set_ylabel('執行時間 (ms)')
    ax1.set_title('執行時間 vs 線程數')
    ax1.grid(True, alpha=0.3)
    
    # 2. 加速比 vs 線程數
    ax2.plot(df['Thread_Count'], df['Speedup'], 'ro-', linewidth=2, markersize=8, label='實際加速比')
    ax2.plot(df['Thread_Count'], df['Thread_Count'], 'k--', alpha=0.5, label='理想線性加速比')
    ax2.set_xlabel('線程數')
    ax2.set_ylabel('加速比')
    ax2.set_title('加速比分析')
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    
    # 3. 平行效率 vs 線程數
    ax3.plot(df['Thread_Count'], df['Efficiency'], 'go-', linewidth=2, markersize=8)
    ax3.axhline(y=80, color='r', linestyle='--', alpha=0.7, label='80%效率線')
    ax3.set_xlabel('線程數')
    ax3.set_ylabel('平行效率 (%)')
    ax3.set_title('平行效率分析')
    ax3.legend()
    ax3.grid(True, alpha=0.3)
    
    # 4. 記憶體使用 vs 線程數
    ax4.bar(df['Thread_Count'], df['Memory_MB'], alpha=0.7, color='purple')
    ax4.set_xlabel('線程數')
    ax4.set_ylabel('記憶體使用 (MB)')
    ax4.set_title('記憶體使用分析')
    ax4.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('plots/scalability_analysis.png', dpi=300, bbox_inches='tight')
    plt.show()

def plot_strategy_comparison():
    """繪製策略比較圖表"""
    df = pd.read_csv('strategy_benchmark_results.csv')
    
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 12))
    
    # 按策略分組
    strategy_groups = df.groupby('Strategy')
    
    # 1. 解品質比較 (箱型圖)
    strategies = df['Strategy'].unique()
    best_lengths = [strategy_groups.get_group(s)['Best_Length'].values for s in strategies]
    
    box_plot = ax1.boxplot(best_lengths, labels=strategies, patch_artist=True)
    colors = ['lightblue', 'lightgreen', 'lightcoral', 'lightyellow', 'lightpink']
    for patch, color in zip(box_plot['boxes'], colors):
        patch.set_facecolor(color)
    
    ax1.set_xlabel('策略')
    ax1.set_ylabel('最佳解長度')
    ax1.set_title('策略解品質比較')
    ax1.tick_params(axis='x', rotation=45)
    ax1.grid(True, alpha=0.3)
    
    # 2. 執行時間比較
    avg_times = strategy_groups['Execution_Time_ms'].mean()
    bars = ax2.bar(avg_times.index, avg_times.values, alpha=0.7, color=colors)
    ax2.set_xlabel('策略')
    ax2.set_ylabel('平均執行時間 (ms)')
    ax2.set_title('策略執行時間比較')
    ax2.tick_params(axis='x', rotation=45)
    ax2.grid(True, alpha=0.3)
    
    # 在柱狀圖上顯示數值
    for bar in bars:
        height = bar.get_height()
        ax2.text(bar.get_x() + bar.get_width()/2., height,
                f'{height:.1f}', ha='center', va='bottom')
    
    # 3. 與最優解的差距
    avg_gaps = strategy_groups['Gap_to_Optimal'].mean()
    bars = ax3.bar(avg_gaps.index, avg_gaps.values, alpha=0.7, color=colors)
    ax3.set_xlabel('策略')
    ax3.set_ylabel('與最優解差距 (%)')
    ax3.set_title('策略解品質差距分析')
    ax3.tick_params(axis='x', rotation=45)
    ax3.grid(True, alpha=0.3)
    
    # 4. 成功率比較
    avg_success = strategy_groups['Success_Rate'].mean()
    bars = ax4.bar(avg_success.index, avg_success.values, alpha=0.7, color=colors)
    ax4.set_xlabel('策略')
    ax4.set_ylabel('成功率 (%)')
    ax4.set_title('策略成功率比較')
    ax4.tick_params(axis='x', rotation=45)
    ax4.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('plots/strategy_comparison.png', dpi=300, bbox_inches='tight')
    plt.show()

def plot_performance_summary():
    """繪製性能總結圖"""
    scalability_df = pd.read_csv('scalability_results.csv')
    strategy_df = pd.read_csv('strategy_benchmark_results.csv')
    
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))
    
    # 1. 加速比與效率綜合分析
    ax1_twin = ax1.twinx()
    
    line1 = ax1.plot(scalability_df['Thread_Count'], scalability_df['Speedup'], 
                     'bo-', linewidth=2, markersize=8, label='加速比')
    line2 = ax1_twin.plot(scalability_df['Thread_Count'], scalability_df['Efficiency'], 
                          'ro-', linewidth=2, markersize=8, label='效率 (%)', color='red')
    
    ax1.set_xlabel('線程數')
    ax1.set_ylabel('加速比', color='blue')
    ax1_twin.set_ylabel('平行效率 (%)', color='red')
    ax1.set_title('平行性能綜合分析')
    
    # 合併圖例
    lines = line1 + line2
    labels = [l.get_label() for l in lines]
    ax1.legend(lines, labels, loc='upper left')
    
    ax1.grid(True, alpha=0.3)
    
    # 2. 策略性能雷達圖
    strategies = strategy_df['Strategy'].unique()
    strategy_groups = strategy_df.groupby('Strategy')
    
    # 正規化指標 (0-1範圍)
    metrics = ['Best_Length', 'Execution_Time_ms', 'Gap_to_Optimal', 'Success_Rate']
    
    angles = np.linspace(0, 2 * np.pi, len(metrics), endpoint=False).tolist()
    angles += angles[:1]  # 完成圓形
    
    ax2 = plt.subplot(122, projection='polar')
    
    for i, strategy in enumerate(strategies[:3]):  # 只顯示前3個策略避免圖表過於複雜
        group = strategy_groups.get_group(strategy)
        values = [
            1 / group['Best_Length'].mean() * 10000,  # 反轉，越小越好
            1 / group['Execution_Time_ms'].mean() * 1000,  # 反轉，越小越好
            max(0, 1 - group['Gap_to_Optimal'].mean() / 100),  # 反轉，越小越好
            group['Success_Rate'].mean() / 100  # 正向，越大越好
        ]
        values += values[:1]  # 完成圓形
        
        ax2.plot(angles, values, 'o-', linewidth=2, label=strategy)
        ax2.fill(angles, values, alpha=0.25)
    
    ax2.set_xticks(angles[:-1])
    ax2.set_xticklabels(['解品質', '執行速度', '最優性', '成功率'])
    ax2.set_title('策略性能雷達圖')
    ax2.legend(loc='upper right', bbox_to_anchor=(1.3, 1.0))
    
    plt.tight_layout()
    plt.savefig('plots/performance_summary.png', dpi=300, bbox_inches='tight')
    plt.show()

if __name__ == "__main__":
    print("生成可擴展性分析圖表...")
    plot_scalability_analysis()
    
    print("生成策略比較圖表...")
    plot_strategy_comparison()
    
    print("生成性能總結圖表...")
    plot_performance_summary()
    
    print("所有圖表已生成完成！")
