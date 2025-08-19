import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

def analyze_speedup_performance():
    """Analyze actual speedup vs expected speedup"""
    
    # Load data
    df = pd.read_csv("experiment/results/quick_experiment_results_20250819_012533.csv")
    
    print("🔍 並行加速比分析 - 實際 vs 理論")
    print("="*60)
    
    # Calculate average times for each configuration
    avg_times = df.groupby(['instance_name', 'threads'])['execution_time_ms'].mean().reset_index()
    
    # Analyze by instance
    results = []
    
    for instance in ['eil51', 'kroA100', 'ch150', 'gr202']:
        instance_data = avg_times[avg_times['instance_name'] == instance]
        
        print(f"\n📊 {instance.upper()} 分析:")
        print("-" * 40)
        
        # Get baseline (1 thread) time
        baseline_time = instance_data[instance_data['threads'] == 1]['execution_time_ms'].iloc[0]
        
        print(f"基準時間 (1線程): {baseline_time:.0f}ms")
        
        for threads in [1, 2, 4, 8]:
            current_time = instance_data[instance_data['threads'] == threads]['execution_time_ms'].iloc[0]
            
            # Actual speedup
            actual_speedup = baseline_time / current_time
            
            # Theoretical speedup (ideal)
            theoretical_speedup = threads
            
            # Efficiency
            efficiency = (actual_speedup / threads) * 100
            
            # Time comparison
            if threads == 1:
                time_change = "基準"
                change_pct = 0
            else:
                time_change = "更快" if current_time < baseline_time else "更慢"
                change_pct = ((current_time - baseline_time) / baseline_time) * 100
            
            print(f"{threads}線程: {current_time:.0f}ms | "
                  f"實際加速: {actual_speedup:.2f}x | "
                  f"理論加速: {theoretical_speedup}x | "
                  f"效率: {efficiency:.1f}% | "
                  f"{time_change} {abs(change_pct):.1f}%")
            
            results.append({
                'instance': instance,
                'threads': threads,
                'time_ms': current_time,
                'actual_speedup': actual_speedup,
                'theoretical_speedup': theoretical_speedup,
                'efficiency': efficiency,
                'time_change_pct': change_pct
            })
    
    # Create visualization
    create_speedup_visualization(results)
    
    # Summary analysis
    print("\n" + "="*60)
    print("🎯 關鍵發現:")
    print("="*60)
    
    # Find instances where parallel is actually faster
    faster_cases = [r for r in results if r['threads'] > 1 and r['time_change_pct'] < 0]
    slower_cases = [r for r in results if r['threads'] > 1 and r['time_change_pct'] > 0]
    
    print(f"\n✅ 並行更快的情況: {len(faster_cases)}/{len([r for r in results if r['threads'] > 1])}")
    if faster_cases:
        for case in faster_cases:
            print(f"   • {case['instance']} {case['threads']}線程: 快 {abs(case['time_change_pct']):.1f}%")
    
    print(f"\n❌ 並行更慢的情況: {len(slower_cases)}/{len([r for r in results if r['threads'] > 1])}")
    if slower_cases:
        worst_cases = sorted(slower_cases, key=lambda x: x['time_change_pct'], reverse=True)[:5]
        for case in worst_cases:
            print(f"   • {case['instance']} {case['threads']}線程: 慢 {case['time_change_pct']:.1f}%")
    
    # Best efficiency analysis
    print(f"\n🏆 最佳效率:")
    best_efficiency = sorted([r for r in results if r['threads'] > 1], 
                           key=lambda x: x['efficiency'], reverse=True)[:5]
    for case in best_efficiency:
        print(f"   • {case['instance']} {case['threads']}線程: {case['efficiency']:.1f}% 效率")
    
    return results

def create_speedup_visualization(results):
    """Create speedup comparison visualization"""
    
    df_results = pd.DataFrame(results)
    
    fig, axes = plt.subplots(2, 2, figsize=(15, 12))
    fig.suptitle('並行加速比分析 - 實際 vs 理論', fontsize=16, fontweight='bold')
    
    # 1. Actual vs Theoretical Speedup
    ax1 = axes[0, 0]
    for instance in ['eil51', 'kroA100', 'ch150', 'gr202']:
        instance_data = df_results[df_results['instance'] == instance]
        ax1.plot(instance_data['threads'], instance_data['actual_speedup'], 
                marker='o', linewidth=2, label=f'{instance} (實際)')
    
    # Add theoretical line
    threads = [1, 2, 4, 8]
    ax1.plot(threads, threads, 'k--', linewidth=2, alpha=0.7, label='理論加速比')
    
    ax1.set_xlabel('線程數')
    ax1.set_ylabel('加速比')
    ax1.set_title('實際 vs 理論加速比')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    
    # 2. Time Change Percentage
    ax2 = axes[0, 1]
    instances = ['eil51', 'kroA100', 'ch150', 'gr202']
    thread_counts = [2, 4, 8]
    
    x = np.arange(len(instances))
    width = 0.25
    
    for i, threads in enumerate(thread_counts):
        changes = []
        for instance in instances:
            change = df_results[(df_results['instance'] == instance) & 
                              (df_results['threads'] == threads)]['time_change_pct'].iloc[0]
            changes.append(change)
        
        ax2.bar(x + i*width, changes, width, label=f'{threads}線程')
    
    ax2.set_xlabel('TSP實例')
    ax2.set_ylabel('時間變化 (%)')
    ax2.set_title('相對於單線程的時間變化')
    ax2.set_xticks(x + width)
    ax2.set_xticklabels(instances)
    ax2.legend()
    ax2.grid(True, alpha=0.3, axis='y')
    ax2.axhline(y=0, color='red', linestyle='-', alpha=0.5)
    
    # 3. Efficiency Heatmap
    ax3 = axes[1, 0]
    efficiency_matrix = df_results.pivot(index='instance', columns='threads', values='efficiency')
    efficiency_matrix = efficiency_matrix[[1, 2, 4, 8]]  # Reorder columns
    
    sns.heatmap(efficiency_matrix, annot=True, fmt='.1f', cmap='RdYlGn', 
               center=50, ax=ax3, cbar_kws={'label': '效率 (%)'})
    ax3.set_title('並行效率熱力圖')
    ax3.set_xlabel('線程數')
    ax3.set_ylabel('TSP實例')
    
    # 4. Speedup Summary Table
    ax4 = axes[1, 1]
    ax4.axis('off')
    
    # Create summary table
    summary_data = []
    for instance in instances:
        instance_results = df_results[df_results['instance'] == instance]
        best_speedup = instance_results['actual_speedup'].max()
        best_config = instance_results.loc[instance_results['actual_speedup'].idxmax()]
        
        summary_data.append([
            instance,
            f"{best_speedup:.2f}x",
            f"{best_config['threads']}線程",
            f"{best_config['efficiency']:.1f}%"
        ])
    
    table_df = pd.DataFrame(summary_data, 
                           columns=['實例', '最佳加速比', '最佳配置', '效率'])
    
    table = ax4.table(cellText=table_df.values,
                     colLabels=table_df.columns,
                     cellLoc='center',
                     loc='center',
                     bbox=[0.1, 0.3, 0.8, 0.5])
    
    table.auto_set_font_size(False)
    table.set_fontsize(11)
    table.scale(1.2, 2)
    
    # Style table
    for i in range(len(table_df.columns)):
        table[(0, i)].set_facecolor('#E8E8E8')
        table[(0, i)].set_text_props(weight='bold')
    
    ax4.set_title('加速比總結', fontsize=14, fontweight='bold')
    
    # Add analysis text
    analysis_text = """
分析結果:
• 大部分情況下並行比單線程慢
• 最佳情況: gr202 8線程效率 10.9%
• OpenMP開銷 > 計算收益
• 需要更大問題規模才能體現優勢
    """
    
    ax4.text(0.05, 0.05, analysis_text, transform=ax4.transAxes, 
            fontsize=10, verticalalignment='bottom',
            bbox=dict(boxstyle="round,pad=0.3", facecolor='lightcoral', alpha=0.3))
    
    plt.tight_layout()
    
    # Save plot
    output_file = "experiment/results/speedup_analysis.png"
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"\n📈 加速比分析圖保存至: {output_file}")
    
    plt.show()

if __name__ == "__main__":
    results = analyze_speedup_performance()
