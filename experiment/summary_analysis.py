import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

def create_summary_plots():
    """Create focused summary plots for ACO performance analysis"""
    
    # Load data
    df = pd.read_csv("experiment/results/quick_experiment_results_20250819_012533.csv")
    
    # Set up the plotting style
    plt.style.use('default')
    sns.set_palette("Set2")
    
    # Create figure with 2x2 subplots
    fig, axes = plt.subplots(2, 2, figsize=(15, 12))
    fig.suptitle('ACO Parallel Performance Analysis Summary', fontsize=16, fontweight='bold')
    
    # 1. Execution Time Comparison (Top Left)
    ax1 = axes[0, 0]
    avg_time = df.groupby(['instance_name', 'threads'])['execution_time_ms'].mean().reset_index()
    
    for instance in ['eil51', 'kroA100', 'ch150', 'gr202']:
        instance_data = avg_time[avg_time['instance_name'] == instance]
        ax1.plot(instance_data['threads'], instance_data['execution_time_ms'], 
                marker='o', linewidth=2.5, markersize=8, label=f'{instance}')
    
    ax1.set_xlabel('Number of Threads', fontsize=12)
    ax1.set_ylabel('Execution Time (ms)', fontsize=12)
    ax1.set_title('Average Execution Time by Thread Count', fontsize=14, fontweight='bold')
    ax1.legend(fontsize=10)
    ax1.grid(True, alpha=0.3)
    ax1.set_yscale('log')
    
    # 2. Efficiency Comparison (Top Right)
    ax2 = axes[0, 1]
    avg_efficiency = df.groupby(['instance_name', 'threads'])['efficiency'].mean().reset_index()
    
    for instance in ['eil51', 'kroA100', 'ch150', 'gr202']:
        instance_data = avg_efficiency[avg_efficiency['instance_name'] == instance]
        ax2.plot(instance_data['threads'], instance_data['efficiency'], 
                marker='s', linewidth=2.5, markersize=8, label=f'{instance}')
    
    # Add ideal efficiency line
    threads = [1, 2, 4, 8]
    ax2.plot(threads, [100]*len(threads), 'k--', alpha=0.7, linewidth=2, label='Perfect Efficiency')
    
    ax2.set_xlabel('Number of Threads', fontsize=12)
    ax2.set_ylabel('Efficiency (%)', fontsize=12)
    ax2.set_title('Parallel Efficiency by Thread Count', fontsize=14, fontweight='bold')
    ax2.legend(fontsize=10)
    ax2.grid(True, alpha=0.3)
    ax2.set_ylim(0, 110)
    
    # 3. Solution Quality Consistency (Bottom Left)
    ax3 = axes[1, 0]
    
    # Create box plot for solution quality
    df_melted = df[['instance_name', 'threads', 'ratio_to_best']].copy()
    df_melted['threads'] = df_melted['threads'].astype(str) + ' threads'
    
    sns.boxplot(data=df_melted, x='instance_name', y='ratio_to_best', hue='threads', ax=ax3)
    ax3.set_xlabel('TSP Instance', fontsize=12)
    ax3.set_ylabel('Ratio to Best Known Solution', fontsize=12)
    ax3.set_title('Solution Quality Distribution', fontsize=14, fontweight='bold')
    ax3.legend(title='Configuration', fontsize=9, title_fontsize=10)
    
    # 4. Performance Summary Table (Bottom Right)
    ax4 = axes[1, 1]
    ax4.axis('off')
    
    # Create summary statistics
    summary_data = []
    for instance in ['eil51', 'kroA100', 'ch150', 'gr202']:
        instance_df = df[df['instance_name'] == instance]
        
        # Best configuration (highest efficiency)
        best_config = instance_df.loc[instance_df['efficiency'].idxmax()]
        
        # Single thread performance
        single_thread = instance_df[instance_df['threads'] == 1].iloc[0]
        
        summary_data.append([
            instance,
            f"{single_thread['execution_time_ms']:.0f}ms",
            f"{single_thread['efficiency']:.1f}%",
            f"{single_thread['ratio_to_best']:.2f}",
            f"{best_config['threads']:.0f} ({best_config['efficiency']:.1f}%)"
        ])
    
    # Create table
    table_data = pd.DataFrame(summary_data, columns=[
        'Instance', '1-Thread Time', '1-Thread Eff.', 'Avg Ratio', 'Best Config'
    ])
    
    table = ax4.table(cellText=table_data.values,
                     colLabels=table_data.columns,
                     cellLoc='center',
                     loc='center',
                     bbox=[0.1, 0.2, 0.8, 0.6])
    
    table.auto_set_font_size(False)
    table.set_fontsize(10)
    table.scale(1.2, 2)
    
    # Style the table
    for i in range(len(table_data.columns)):
        table[(0, i)].set_facecolor('#E8E8E8')
        table[(0, i)].set_text_props(weight='bold')
    
    ax4.set_title('Performance Summary', fontsize=14, fontweight='bold', pad=20)
    
    # Add key findings text
    findings_text = """
Key Findings:
• Single-thread efficiency: ~96%
• 8-thread efficiency drops to ~12%
• Solution quality remains stable
• Parallel overhead dominates benefits
• Larger instances needed for speedup
    """
    
    ax4.text(0.05, 0.05, findings_text, transform=ax4.transAxes, 
            fontsize=10, verticalalignment='bottom',
            bbox=dict(boxstyle="round,pad=0.3", facecolor='lightblue', alpha=0.3))
    
    plt.tight_layout()
    
    # Save the plot
    output_file = "experiment/results/aco_summary_analysis.png"
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"Summary analysis saved to: {output_file}")
    
    plt.show()

def print_key_insights():
    """Print key insights from the analysis"""
    print("\n" + "="*60)
    print("🎯 ACO PARALLEL PERFORMANCE ANALYSIS - KEY INSIGHTS")
    print("="*60)
    
    print("\n📊 PERFORMANCE METRICS:")
    print("   • Total experiments: 80 (4 instances × 4 thread configs × 5 rounds)")
    print("   • Test instances: eil51 (51), kroA100 (100), ch150 (150), gr202 (202)")
    print("   • Thread configurations: 1, 2, 4, 8")
    print("   • Iterations per run: 50")
    
    print("\n⚡ EFFICIENCY FINDINGS:")
    print("   • Single-thread: ~96% efficiency (near optimal)")
    print("   • 2-threads: ~35% efficiency (significant overhead)")
    print("   • 4-threads: ~23% efficiency (diminishing returns)")
    print("   • 8-threads: ~12% efficiency (poor scalability)")
    
    print("\n🎯 SOLUTION QUALITY:")
    print("   • eil51: ratio ~1.75 (75% above optimal)")
    print("   • kroA100: ratio ~2.25 (125% above optimal)")
    print("   • ch150: ratio ~2.78 (178% above optimal)")
    print("   • gr202: ratio ~2.55 (155% above optimal)")
    print("   • Quality remains consistent across thread counts")
    
    print("\n⏱️ EXECUTION TIME PATTERNS:")
    print("   • Parallel versions often slower than single-thread")
    print("   • OpenMP overhead dominates small-medium problem sizes")
    print("   • No clear speedup observed in current configuration")
    
    print("\n💡 RECOMMENDATIONS:")
    print("   1. Single-thread is optimal for these problem sizes")
    print("   2. Test larger TSP instances (500+ cities) for parallel benefits")
    print("   3. Tune ACO parameters for better parallel efficiency")
    print("   4. Consider different parallelization strategies")
    print("   5. Increase iterations/ants for larger computational workload")
    
    print("\n🔍 NEXT STEPS:")
    print("   • Test larger TSPLIB instances (lin318, pcb442, rat783)")
    print("   • Experiment with different ant/iteration combinations")
    print("   • Profile code to identify bottlenecks")
    print("   • Compare with other parallel TSP algorithms")
    
    print("="*60)

if __name__ == "__main__":
    create_summary_plots()
    print_key_insights()
