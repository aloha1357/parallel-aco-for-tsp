#!/usr/bin/env python3
"""
Enhanced Experiment Analysis with 4-Instance Configuration
分析四组实例的线程性能：eil51, kroA100, kroA150, gr202
"""

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from pathlib import Path
import glob
from datetime import datetime
from scipy import stats
import warnings
warnings.filterwarnings('ignore')

plt.rcParams['font.family'] = ['DejaVu Sans', 'SimHei', 'Liberation Sans']
plt.rcParams['axes.unicode_minus'] = False

class FourInstanceAnalyzer:
    def __init__(self):
        self.instance_info = {
            'eil51': {'size': 51, 'optimal': 426, 'type': 'Small'},
            'kroA100': {'size': 100, 'optimal': 21282, 'type': 'Medium'},
            'kroA150': {'size': 150, 'optimal': 26524, 'type': 'Large'},
            'gr202': {'size': 202, 'optimal': 40160, 'type': 'X-Large'}
        }
        
        # 设置颜色方案
        self.colors = {
            'Serial_ACO': '#FF6B6B',
            'Parallel_ACO_2': '#4ECDC4', 
            'Parallel_ACO_4': '#45B7D1',
            'Parallel_ACO_8': '#96CEB4'
        }
        
    def load_latest_results(self, experiment_type='fixed_iterations'):
        """加载最新的实验结果"""
        results_dir = Path(f'results/{experiment_type}')
        if not results_dir.exists():
            print(f"Warning: {results_dir} not found")
            return None
            
        csv_files = glob.glob(str(results_dir / f'{experiment_type}_results_*.csv'))
        if not csv_files:
            print(f"No CSV files found in {results_dir}")
            return None
            
        latest_file = max(csv_files, key=lambda x: Path(x).stat().st_mtime)
        print(f"Loading: {latest_file}")
        
        df = pd.read_csv(latest_file)
        # 过滤只保留4个实例
        df = df[df['instance_name'].isin(self.instance_info.keys())]
        
        return df
    
    def calculate_speedup(self, df):
        """计算加速比"""
        speedup_data = []
        
        for instance in self.instance_info.keys():
            instance_data = df[df['instance_name'] == instance]
            
            # 获取串行基准时间
            serial_data = instance_data[instance_data['algorithm'] == 'Serial_ACO']
            if serial_data.empty:
                continue
                
            serial_time = serial_data['wall_time_s'].mean()
            
            for algorithm in ['Parallel_ACO_2', 'Parallel_ACO_4', 'Parallel_ACO_8']:
                parallel_data = instance_data[instance_data['algorithm'] == algorithm]
                if not parallel_data.empty:
                    parallel_time = parallel_data['wall_time_s'].mean()
                    speedup = serial_time / parallel_time
                    
                    speedup_data.append({
                        'instance': instance,
                        'algorithm': algorithm,
                        'threads': int(algorithm.split('_')[-1]),
                        'speedup': speedup,
                        'efficiency': speedup / int(algorithm.split('_')[-1])
                    })
        
        return pd.DataFrame(speedup_data)
    
    def create_comprehensive_visualization(self, fixed_df, time_budget_df=None):
        """创建综合可视化分析"""
        fig = plt.figure(figsize=(20, 16))
        
        # 1. 执行时间对比 (2x2子图)
        for i, instance in enumerate(self.instance_info.keys()):
            ax = plt.subplot(4, 4, i + 1)
            instance_data = fixed_df[fixed_df['instance_name'] == instance]
            
            # 箱线图显示时间分布
            algorithms = ['Serial_ACO', 'Parallel_ACO_2', 'Parallel_ACO_4', 'Parallel_ACO_8']
            times_data = []
            labels = []
            
            for alg in algorithms:
                alg_data = instance_data[instance_data['algorithm'] == alg]
                if not alg_data.empty:
                    times_data.append(alg_data['wall_time_s'].values)
                    labels.append(f"{alg.replace('Parallel_ACO_', '')}T")
            
            if times_data:
                bp = ax.boxplot(times_data, labels=labels, patch_artist=True)
                for patch, alg in zip(bp['boxes'], algorithms[:len(times_data)]):
                    patch.set_facecolor(self.colors.get(alg, '#CCCCCC'))
                    patch.set_alpha(0.7)
            
            ax.set_title(f'{instance}\\n({self.instance_info[instance]["size"]} cities)', 
                        fontsize=10, fontweight='bold')
            ax.set_ylabel('Time (s)', fontsize=9)
            ax.grid(True, alpha=0.3)
        
        # 2. 加速比分析 (第二行)
        speedup_df = self.calculate_speedup(fixed_df)
        
        # 加速比热力图
        ax = plt.subplot(4, 4, 5)
        speedup_matrix = speedup_df.pivot(index='instance', columns='threads', values='speedup')
        speedup_matrix = speedup_matrix.reindex(self.instance_info.keys())
        
        sns.heatmap(speedup_matrix, annot=True, fmt='.2f', cmap='RdYlBu_r', 
                   ax=ax, cbar_kws={'label': 'Speedup'})
        ax.set_title('Speedup Matrix', fontweight='bold')
        ax.set_xlabel('Threads')
        ax.set_ylabel('Instance')
        
        # 效率热力图
        ax = plt.subplot(4, 4, 6)
        efficiency_matrix = speedup_df.pivot(index='instance', columns='threads', values='efficiency')
        efficiency_matrix = efficiency_matrix.reindex(self.instance_info.keys())
        
        sns.heatmap(efficiency_matrix, annot=True, fmt='.2f', cmap='RdYlGn', 
                   ax=ax, cbar_kws={'label': 'Efficiency'})
        ax.set_title('Parallel Efficiency', fontweight='bold')
        ax.set_xlabel('Threads')
        ax.set_ylabel('Instance')
        
        # 加速比曲线
        ax = plt.subplot(4, 4, 7)
        for instance in self.instance_info.keys():
            instance_speedup = speedup_df[speedup_df['instance'] == instance]
            if not instance_speedup.empty:
                ax.plot(instance_speedup['threads'], instance_speedup['speedup'], 
                       'o-', label=instance, linewidth=2, markersize=6)
        
        # 理想加速比线
        threads = [2, 4, 8]
        ax.plot(threads, threads, 'k--', alpha=0.5, label='Ideal')
        
        ax.set_xlabel('Number of Threads')
        ax.set_ylabel('Speedup')
        ax.set_title('Speedup vs Threads', fontweight='bold')
        ax.legend()
        ax.grid(True, alpha=0.3)
        
        # 效率曲线
        ax = plt.subplot(4, 4, 8)
        for instance in self.instance_info.keys():
            instance_speedup = speedup_df[speedup_df['instance'] == instance]
            if not instance_speedup.empty:
                ax.plot(instance_speedup['threads'], instance_speedup['efficiency'], 
                       's-', label=instance, linewidth=2, markersize=6)
        
        ax.axhline(y=1.0, color='k', linestyle='--', alpha=0.5, label='Perfect Efficiency')
        ax.set_xlabel('Number of Threads')
        ax.set_ylabel('Efficiency')
        ax.set_title('Parallel Efficiency vs Threads', fontweight='bold')
        ax.legend()
        ax.grid(True, alpha=0.3)
        
        # 3. 解质量分析 (第三行)
        ax = plt.subplot(4, 4, 9)
        quality_data = []
        for instance in self.instance_info.keys():
            instance_data = fixed_df[fixed_df['instance_name'] == instance]
            optimal = self.instance_info[instance]['optimal']
            
            for alg in ['Serial_ACO', 'Parallel_ACO_2', 'Parallel_ACO_4', 'Parallel_ACO_8']:
                alg_data = instance_data[instance_data['algorithm'] == alg]
                if not alg_data.empty:
                    avg_ratio = alg_data['ratio_to_best'].mean()
                    quality_data.append({
                        'instance': instance,
                        'algorithm': alg,
                        'quality_ratio': avg_ratio
                    })
        
        quality_df = pd.DataFrame(quality_data)
        quality_pivot = quality_df.pivot(index='instance', columns='algorithm', values='quality_ratio')
        quality_pivot = quality_pivot.reindex(self.instance_info.keys())
        
        sns.heatmap(quality_pivot, annot=True, fmt='.3f', cmap='RdYlGn_r', ax=ax)
        ax.set_title('Solution Quality (Tour/Optimal)', fontweight='bold')
        ax.set_xlabel('Algorithm')
        ax.set_ylabel('Instance')
        
        # 4. 时间预算实验分析 (如果有数据)
        if time_budget_df is not None:
            # 迭代次数对比
            ax = plt.subplot(4, 4, 10)
            iterations_data = []
            
            for instance in self.instance_info.keys():
                instance_data = time_budget_df[time_budget_df['instance_name'] == instance]
                for alg in ['Serial_ACO', 'Parallel_ACO_2', 'Parallel_ACO_4', 'Parallel_ACO_8']:
                    alg_data = instance_data[instance_data['algorithm'] == alg]
                    if not alg_data.empty:
                        iterations_data.append(alg_data['actual_iterations'].values)
                    else:
                        iterations_data.append([])
            
            if any(len(data) > 0 for data in iterations_data):
                # 创建迭代次数对比图
                x_pos = np.arange(len(self.instance_info))
                width = 0.2
                
                for i, alg in enumerate(['Serial_ACO', 'Parallel_ACO_2', 'Parallel_ACO_4', 'Parallel_ACO_8']):
                    means = []
                    for instance in self.instance_info.keys():
                        instance_data = time_budget_df[
                            (time_budget_df['instance_name'] == instance) & 
                            (time_budget_df['algorithm'] == alg)
                        ]
                        if not instance_data.empty:
                            means.append(instance_data['actual_iterations'].mean())
                        else:
                            means.append(0)
                    
                    ax.bar(x_pos + i * width, means, width, 
                          label=alg.replace('Parallel_ACO_', ''), 
                          color=self.colors.get(alg, '#CCCCCC'), alpha=0.8)
                
                ax.set_xlabel('Instance')
                ax.set_ylabel('Iterations Completed')
                ax.set_title('Iterations within Time Budget', fontweight='bold')
                ax.set_xticks(x_pos + width * 1.5)
                ax.set_xticklabels(list(self.instance_info.keys()))
                ax.legend()
                ax.grid(True, alpha=0.3)
        
        # 5. 统计分析总结 (第四行)
        ax = plt.subplot(4, 4, 13)
        ax.axis('off')
        
        # 创建统计总结
        summary_text = self.generate_statistical_summary(fixed_df, speedup_df)
        ax.text(0.05, 0.95, summary_text, transform=ax.transAxes, 
               fontsize=9, verticalalignment='top', fontfamily='monospace')
        
        # 6. 实例规模对比
        ax = plt.subplot(4, 4, 14)
        instances = list(self.instance_info.keys())
        sizes = [self.instance_info[inst]['size'] for inst in instances]
        
        bars = ax.bar(instances, sizes, color=['#FF6B6B', '#4ECDC4', '#45B7D1', '#96CEB4'])
        ax.set_ylabel('Number of Cities')
        ax.set_title('Instance Sizes', fontweight='bold')
        ax.grid(True, alpha=0.3)
        
        # 添加数值标签
        for bar, size in zip(bars, sizes):
            ax.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 3,
                   str(size), ha='center', va='bottom', fontweight='bold')
        
        plt.tight_layout()
        return fig
    
    def generate_statistical_summary(self, df, speedup_df):
        """生成统计分析总结"""
        summary = "STATISTICAL SUMMARY\\n" + "="*30 + "\\n\\n"
        
        # 最佳加速比
        best_speedup = speedup_df.loc[speedup_df['speedup'].idxmax()]
        summary += f"Best Speedup: {best_speedup['speedup']:.2f}x\\n"
        summary += f"Instance: {best_speedup['instance']}\\n"
        summary += f"Threads: {best_speedup['threads']}\\n\\n"
        
        # 平均效率
        avg_efficiency = speedup_df['efficiency'].mean()
        summary += f"Average Efficiency: {avg_efficiency:.3f}\\n\\n"
        
        # 每个实例的最佳算法
        summary += "Best Algorithm per Instance:\\n"
        for instance in self.instance_info.keys():
            instance_data = df[df['instance_name'] == instance]
            if not instance_data.empty:
                best_alg = instance_data.groupby('algorithm')['wall_time_s'].mean().idxmin()
                best_time = instance_data.groupby('algorithm')['wall_time_s'].mean().min()
                summary += f"{instance}: {best_alg} ({best_time:.3f}s)\\n"
        
        return summary
    
    def export_analysis_report(self, fixed_df, speedup_df):
        """导出详细分析报告"""
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        report_file = f"four_instance_analysis_report_{timestamp}.md"
        
        with open(report_file, 'w', encoding='utf-8') as f:
            f.write("# Four-Instance Thread Performance Analysis\\n\\n")
            f.write(f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\\n\\n")
            
            f.write("## Instance Configuration\\n\\n")
            f.write("| Instance | Cities | Optimal | Type |\\n")
            f.write("|----------|--------|---------|------|\\n")
            for name, info in self.instance_info.items():
                f.write(f"| {name} | {info['size']} | {info['optimal']} | {info['type']} |\\n")
            
            f.write("\\n## Performance Summary\\n\\n")
            
            # 加速比表格
            f.write("### Speedup Results\\n\\n")
            f.write("| Instance | 2 Threads | 4 Threads | 8 Threads |\\n")
            f.write("|----------|-----------|-----------|-----------|\\n")
            
            speedup_pivot = speedup_df.pivot(index='instance', columns='threads', values='speedup')
            for instance in self.instance_info.keys():
                if instance in speedup_pivot.index:
                    row = speedup_pivot.loc[instance]
                    f.write(f"| {instance} |")
                    for threads in [2, 4, 8]:
                        if threads in row.index:
                            f.write(f" {row[threads]:.2f}x |")
                        else:
                            f.write(" N/A |")
                    f.write("\\n")
            
            # 效率表格
            f.write("\\n### Parallel Efficiency\\n\\n")
            f.write("| Instance | 2 Threads | 4 Threads | 8 Threads |\\n")
            f.write("|----------|-----------|-----------|-----------|\\n")
            
            efficiency_pivot = speedup_df.pivot(index='instance', columns='threads', values='efficiency')
            for instance in self.instance_info.keys():
                if instance in efficiency_pivot.index:
                    row = efficiency_pivot.loc[instance]
                    f.write(f"| {instance} |")
                    for threads in [2, 4, 8]:
                        if threads in row.index:
                            f.write(f" {row[threads]:.3f} |")
                        else:
                            f.write(" N/A |")
                    f.write("\\n")
            
        print(f"Analysis report exported to: {report_file}")

def main():
    analyzer = FourInstanceAnalyzer()
    
    print("=== Four-Instance Thread Performance Analysis ===")
    
    # 加载固定迭代实验数据
    fixed_df = analyzer.load_latest_results('fixed_iterations')
    if fixed_df is None:
        print("No fixed iterations data found")
        return
    
    print(f"Loaded {len(fixed_df)} fixed iteration records")
    
    # 加载时间预算实验数据
    time_budget_df = analyzer.load_latest_results('time_budget')
    if time_budget_df is not None:
        print(f"Loaded {len(time_budget_df)} time budget records")
    
    # 计算加速比
    speedup_df = analyzer.calculate_speedup(fixed_df)
    print(f"Calculated speedup for {len(speedup_df)} configurations")
    
    # 创建综合可视化
    fig = analyzer.create_comprehensive_visualization(fixed_df, time_budget_df)
    
    # 保存图表
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    chart_file = f"four_instance_analysis_{timestamp}.png"
    fig.savefig(chart_file, dpi=300, bbox_inches='tight')
    print(f"Visualization saved to: {chart_file}")
    
    # 导出分析报告
    analyzer.export_analysis_report(fixed_df, speedup_df)
    
    print("\\n=== Analysis Complete ===")

if __name__ == "__main__":
    main()
