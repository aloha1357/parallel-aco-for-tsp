#!/usr/bin/env python3
"""
四個實例平行ACO實驗結果分析腳本

分析固定迭代數和固定時間預算實驗的結果，
生成統計報告和視覺化圖表。
"""

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import glob
import os
from pathlib import Path
import argparse
from scipy import stats

plt.rcParams['font.sans-serif'] = ['SimHei', 'DejaVu Sans', 'Arial']
plt.rcParams['axes.unicode_minus'] = False

class ExperimentAnalyzer:
    def __init__(self, results_dir="results"):
        self.results_dir = Path(results_dir)
        self.output_dir = self.results_dir / "analysis"
        self.output_dir.mkdir(exist_ok=True)
        
        # 實例資訊
        self.instances_info = {
            'eil51': {'size': 51, 'optimal': 426, 'category': '小規模'},
            'kroA100': {'size': 100, 'optimal': 21282, 'category': '中規模'},
            'kroA150': {'size': 150, 'optimal': 26524, 'category': '大規模'},
            'gr202': {'size': 202, 'optimal': 40160, 'category': '超大規模'}
        }
        
    def load_experiment_data(self):
        """載入實驗數據"""
        print("載入實驗數據...")
        
        # 載入固定迭代數實驗結果
        fixed_iter_files = glob.glob(str(self.results_dir / "fixed_iterations" / "*.csv"))
        if fixed_iter_files:
            self.fixed_iter_df = pd.concat([pd.read_csv(f) for f in fixed_iter_files], ignore_index=True)
            print(f"載入固定迭代數實驗數據: {len(self.fixed_iter_df)} 筆記錄")
        else:
            print("警告: 找不到固定迭代數實驗結果")
            self.fixed_iter_df = pd.DataFrame()
        
        # 載入固定時間預算實驗結果
        time_budget_files = glob.glob(str(self.results_dir / "time_budget" / "*.csv"))
        if time_budget_files:
            self.time_budget_df = pd.concat([pd.read_csv(f) for f in time_budget_files], ignore_index=True)
            print(f"載入固定時間預算實驗數據: {len(self.time_budget_df)} 筆記錄")
        else:
            print("警告: 找不到固定時間預算實驗結果")
            self.time_budget_df = pd.DataFrame()
        
        # 載入基準時間數據
        baseline_files = glob.glob(str(self.results_dir / "baseline_measurements" / "*.csv"))
        if baseline_files:
            self.baseline_df = pd.concat([pd.read_csv(f) for f in baseline_files], ignore_index=True)
            print(f"載入基準時間數據: {len(self.baseline_df)} 筆記錄")
        else:
            print("警告: 找不到基準時間數據")
            self.baseline_df = pd.DataFrame()
    
    def generate_summary_statistics(self):
        """生成總結統計"""
        print("\n生成總結統計...")
        
        summary_report = []
        summary_report.append("=" * 60)
        summary_report.append("四個實例平行ACO實驗結果總結")
        summary_report.append("=" * 60)
        
        # 固定迭代數實驗總結
        if not self.fixed_iter_df.empty:
            summary_report.append("\n【固定迭代數實驗總結】")
            
            for instance in self.instances_info.keys():
                if instance not in self.fixed_iter_df['instance_name'].values:
                    continue
                    
                instance_data = self.fixed_iter_df[self.fixed_iter_df['instance_name'] == instance]
                optimal = self.instances_info[instance]['optimal']
                
                summary_report.append(f"\n{instance} ({self.instances_info[instance]['size']}城市):")
                summary_report.append(f"  最佳解: {optimal}")
                
                for threads in sorted(instance_data['num_threads'].unique()):
                    thread_data = instance_data[instance_data['num_threads'] == threads]
                    
                    mean_ratio = thread_data['ratio_to_best'].mean()
                    std_ratio = thread_data['ratio_to_best'].std()
                    mean_time = thread_data['wall_time_s'].mean()
                    std_time = thread_data['wall_time_s'].std()
                    
                    if threads == 1:
                        serial_time = mean_time
                    else:
                        speedup = serial_time / mean_time if 'serial_time' in locals() else thread_data['speedup_ratio'].mean()
                        efficiency = speedup / threads
                        
                        summary_report.append(f"  {threads}線程: 解品質比例={mean_ratio:.4f}±{std_ratio:.4f}, "
                                            f"時間={mean_time:.3f}±{std_time:.3f}s, "
                                            f"加速比={speedup:.2f}, 效率={efficiency:.2f}")
                    
                    if threads == 1:
                        summary_report.append(f"  {threads}線程: 解品質比例={mean_ratio:.4f}±{std_ratio:.4f}, "
                                            f"時間={mean_time:.3f}±{std_time:.3f}s (基準)")
        
        # 固定時間預算實驗總結
        if not self.time_budget_df.empty:
            summary_report.append("\n【固定時間預算實驗總結】")
            
            for instance in self.instances_info.keys():
                if instance not in self.time_budget_df['instance_name'].values:
                    continue
                    
                instance_data = self.time_budget_df[self.time_budget_df['instance_name'] == instance]
                
                summary_report.append(f"\n{instance}:")
                time_budget = instance_data['time_budget_s'].iloc[0]
                summary_report.append(f"  時間預算: {time_budget:.3f}s")
                
                for threads in sorted(instance_data['num_threads'].unique()):
                    thread_data = instance_data[instance_data['num_threads'] == threads]
                    
                    mean_ratio = thread_data['ratio_to_best'].mean()
                    std_ratio = thread_data['ratio_to_best'].std()
                    mean_iters = thread_data['actual_iterations'].mean()
                    std_iters = thread_data['actual_iterations'].std()
                    mean_ips = thread_data['iterations_per_second'].mean()
                    
                    summary_report.append(f"  {threads}線程: 解品質比例={mean_ratio:.4f}±{std_ratio:.4f}, "
                                        f"完成迭代={mean_iters:.1f}±{std_iters:.1f}, "
                                        f"迭代/秒={mean_ips:.1f}")
        
        summary_report.append("\n" + "=" * 60)
        
        # 儲存總結報告
        with open(self.output_dir / "experiment_summary.txt", 'w', encoding='utf-8') as f:
            f.write('\n'.join(summary_report))
        
        print("總結統計已儲存至:", self.output_dir / "experiment_summary.txt")
        
        # 也輸出到控制台
        for line in summary_report[:50]:  # 只輸出前50行避免過長
            print(line)
    
    def plot_solution_quality_comparison(self):
        """繪製解品質比較圖"""
        print("\n生成解品質比較圖...")
        
        if self.fixed_iter_df.empty:
            print("無固定迭代數據，跳過解品質比較圖")
            return
        
        fig, axes = plt.subplots(2, 2, figsize=(15, 12))
        axes = axes.flatten()
        
        instances = list(self.instances_info.keys())
        
        for i, instance in enumerate(instances):
            if instance not in self.fixed_iter_df['instance_name'].values:
                continue
                
            ax = axes[i]
            instance_data = self.fixed_iter_df[self.fixed_iter_df['instance_name'] == instance]
            
            # 箱線圖顯示解品質分佈
            thread_groups = []
            ratios = []
            
            for threads in sorted(instance_data['num_threads'].unique()):
                thread_data = instance_data[instance_data['num_threads'] == threads]
                thread_groups.extend([f'{threads}線程'] * len(thread_data))
                ratios.extend(thread_data['ratio_to_best'].values)
            
            df_plot = pd.DataFrame({'線程數': thread_groups, '解品質比例': ratios})
            sns.boxplot(data=df_plot, x='線程數', y='解品質比例', ax=ax)
            
            ax.set_title(f'{instance} ({self.instances_info[instance]["size"]}城市)\n解品質比較')
            ax.set_ylabel('解品質比例 (越小越好)')
            ax.axhline(y=1.0, color='red', linestyle='--', alpha=0.7, label='最佳解')
            ax.legend()
            ax.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig(self.output_dir / "solution_quality_comparison.png", dpi=300, bbox_inches='tight')
        plt.savefig(self.output_dir / "solution_quality_comparison.pdf", bbox_inches='tight')
        plt.close()
        
        print("解品質比較圖已儲存")
    
    def plot_speedup_analysis(self):
        """繪製加速比分析圖"""
        print("\n生成加速比分析圖...")
        
        if self.fixed_iter_df.empty:
            print("無固定迭代數據，跳過加速比分析圖")
            return
        
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))
        
        # 加速比圖
        for instance in self.instances_info.keys():
            if instance not in self.fixed_iter_df['instance_name'].values:
                continue
                
            instance_data = self.fixed_iter_df[self.fixed_iter_df['instance_name'] == instance]
            
            threads_list = []
            speedup_means = []
            speedup_stds = []
            
            for threads in sorted(instance_data['num_threads'].unique()):
                if threads == 1:
                    continue  # 跳過1線程
                    
                thread_data = instance_data[instance_data['num_threads'] == threads]
                threads_list.append(threads)
                speedup_means.append(thread_data['speedup_ratio'].mean())
                speedup_stds.append(thread_data['speedup_ratio'].std())
            
            ax1.errorbar(threads_list, speedup_means, yerr=speedup_stds, 
                        marker='o', label=f'{instance}', linewidth=2, markersize=6)
        
        # 理想加速比線
        ideal_threads = [2, 4, 8]
        ax1.plot(ideal_threads, ideal_threads, 'k--', label='理想加速比', alpha=0.7)
        
        ax1.set_xlabel('線程數')
        ax1.set_ylabel('加速比')
        ax1.set_title('加速比分析')
        ax1.legend()
        ax1.grid(True, alpha=0.3)
        ax1.set_xticks([2, 4, 8])
        
        # 效率圖
        for instance in self.instances_info.keys():
            if instance not in self.fixed_iter_df['instance_name'].values:
                continue
                
            instance_data = self.fixed_iter_df[self.fixed_iter_df['instance_name'] == instance]
            
            threads_list = []
            efficiency_means = []
            efficiency_stds = []
            
            for threads in sorted(instance_data['num_threads'].unique()):
                if threads == 1:
                    continue
                    
                thread_data = instance_data[instance_data['num_threads'] == threads]
                threads_list.append(threads)
                efficiency_means.append(thread_data['efficiency'].mean())
                efficiency_stds.append(thread_data['efficiency'].std())
            
            ax2.errorbar(threads_list, efficiency_means, yerr=efficiency_stds,
                        marker='s', label=f'{instance}', linewidth=2, markersize=6)
        
        ax2.axhline(y=1.0, color='red', linestyle='--', alpha=0.7, label='完美效率')
        ax2.set_xlabel('線程數')
        ax2.set_ylabel('效率')
        ax2.set_title('平行效率分析')
        ax2.legend()
        ax2.grid(True, alpha=0.3)
        ax2.set_xticks([2, 4, 8])
        ax2.set_ylim(0, 1.2)
        
        plt.tight_layout()
        plt.savefig(self.output_dir / "speedup_analysis.png", dpi=300, bbox_inches='tight')
        plt.savefig(self.output_dir / "speedup_analysis.pdf", bbox_inches='tight')
        plt.close()
        
        print("加速比分析圖已儲存")
    
    def plot_time_budget_analysis(self):
        """繪製時間預算實驗分析圖"""
        print("\n生成時間預算分析圖...")
        
        if self.time_budget_df.empty:
            print("無時間預算數據，跳過時間預算分析圖")
            return
        
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))
        
        instances = list(self.instances_info.keys())
        thread_counts = sorted(self.time_budget_df['num_threads'].unique())
        
        # 完成迭代數比較
        iteration_data = []
        for instance in instances:
            if instance not in self.time_budget_df['instance_name'].values:
                continue
                
            instance_data = self.time_budget_df[self.time_budget_df['instance_name'] == instance]
            
            for threads in thread_counts:
                thread_data = instance_data[instance_data['num_threads'] == threads]
                if not thread_data.empty:
                    mean_iters = thread_data['actual_iterations'].mean()
                    iteration_data.append({
                        'instance': instance,
                        'threads': f'{threads}線程',
                        'iterations': mean_iters
                    })
        
        if iteration_data:
            df_iter = pd.DataFrame(iteration_data)
            sns.barplot(data=df_iter, x='instance', y='iterations', hue='threads', ax=ax1)
            ax1.set_title('固定時間預算下完成的迭代數')
            ax1.set_xlabel('實例')
            ax1.set_ylabel('平均完成迭代數')
            ax1.tick_params(axis='x', rotation=45)
        
        # 每秒迭代數比較
        ips_data = []
        for instance in instances:
            if instance not in self.time_budget_df['instance_name'].values:
                continue
                
            instance_data = self.time_budget_df[self.time_budget_df['instance_name'] == instance]
            
            for threads in thread_counts:
                thread_data = instance_data[instance_data['num_threads'] == threads]
                if not thread_data.empty:
                    mean_ips = thread_data['iterations_per_second'].mean()
                    ips_data.append({
                        'instance': instance,
                        'threads': f'{threads}線程',
                        'ips': mean_ips
                    })
        
        if ips_data:
            df_ips = pd.DataFrame(ips_data)
            sns.barplot(data=df_ips, x='instance', y='ips', hue='threads', ax=ax2)
            ax2.set_title('每秒迭代數比較')
            ax2.set_xlabel('實例')
            ax2.set_ylabel('每秒迭代數')
            ax2.tick_params(axis='x', rotation=45)
        
        plt.tight_layout()
        plt.savefig(self.output_dir / "time_budget_analysis.png", dpi=300, bbox_inches='tight')
        plt.savefig(self.output_dir / "time_budget_analysis.pdf", bbox_inches='tight')
        plt.close()
        
        print("時間預算分析圖已儲存")
    
    def generate_detailed_csv_summary(self):
        """生成詳細的CSV統計總結"""
        print("\n生成詳細統計總結...")
        
        summary_data = []
        
        # 處理固定迭代數實驗
        if not self.fixed_iter_df.empty:
            for instance in self.instances_info.keys():
                if instance not in self.fixed_iter_df['instance_name'].values:
                    continue
                    
                instance_data = self.fixed_iter_df[self.fixed_iter_df['instance_name'] == instance]
                
                for threads in sorted(instance_data['num_threads'].unique()):
                    thread_data = instance_data[instance_data['num_threads'] == threads]
                    
                    summary_data.append({
                        'experiment_type': 'fixed_iterations',
                        'instance': instance,
                        'instance_size': self.instances_info[instance]['size'],
                        'optimal_solution': self.instances_info[instance]['optimal'],
                        'num_threads': threads,
                        'num_runs': len(thread_data),
                        'mean_ratio_to_best': thread_data['ratio_to_best'].mean(),
                        'std_ratio_to_best': thread_data['ratio_to_best'].std(),
                        'mean_wall_time_s': thread_data['wall_time_s'].mean(),
                        'std_wall_time_s': thread_data['wall_time_s'].std(),
                        'mean_speedup': thread_data['speedup_ratio'].mean() if 'speedup_ratio' in thread_data.columns else 1.0,
                        'mean_efficiency': thread_data['efficiency'].mean() if 'efficiency' in thread_data.columns else 1.0,
                        'best_solution_found': thread_data['tour_length'].min(),
                        'worst_solution_found': thread_data['tour_length'].max(),
                        'mean_solution': thread_data['tour_length'].mean()
                    })
        
        # 處理固定時間預算實驗
        if not self.time_budget_df.empty:
            for instance in self.instances_info.keys():
                if instance not in self.time_budget_df['instance_name'].values:
                    continue
                    
                instance_data = self.time_budget_df[self.time_budget_df['instance_name'] == instance]
                
                for threads in sorted(instance_data['num_threads'].unique()):
                    thread_data = instance_data[instance_data['num_threads'] == threads]
                    
                    summary_data.append({
                        'experiment_type': 'time_budget',
                        'instance': instance,
                        'instance_size': self.instances_info[instance]['size'],
                        'optimal_solution': self.instances_info[instance]['optimal'],
                        'num_threads': threads,
                        'num_runs': len(thread_data),
                        'time_budget_s': thread_data['time_budget_s'].iloc[0] if not thread_data.empty else 0,
                        'mean_actual_iterations': thread_data['actual_iterations'].mean(),
                        'std_actual_iterations': thread_data['actual_iterations'].std(),
                        'mean_ratio_to_best': thread_data['ratio_to_best'].mean(),
                        'std_ratio_to_best': thread_data['ratio_to_best'].std(),
                        'mean_iterations_per_second': thread_data['iterations_per_second'].mean(),
                        'mean_speedup': thread_data['speedup_ratio'].mean() if 'speedup_ratio' in thread_data.columns else 1.0,
                        'mean_efficiency': thread_data['efficiency'].mean() if 'efficiency' in thread_data.columns else 1.0,
                        'best_solution_found': thread_data['tour_length'].min(),
                        'worst_solution_found': thread_data['tour_length'].max(),
                        'mean_solution': thread_data['tour_length'].mean()
                    })
        
        if summary_data:
            summary_df = pd.DataFrame(summary_data)
            summary_df.to_csv(self.output_dir / "detailed_summary.csv", index=False)
            print("詳細統計總結已儲存至:", self.output_dir / "detailed_summary.csv")
    
    def run_complete_analysis(self):
        """執行完整分析"""
        print("開始實驗結果分析...")
        
        self.load_experiment_data()
        
        if self.fixed_iter_df.empty and self.time_budget_df.empty:
            print("錯誤: 找不到任何實驗數據")
            return
        
        self.generate_summary_statistics()
        self.plot_solution_quality_comparison()
        self.plot_speedup_analysis()
        self.plot_time_budget_analysis()
        self.generate_detailed_csv_summary()
        
        print(f"\n分析完成! 所有結果已儲存至: {self.output_dir}")

def main():
    parser = argparse.ArgumentParser(description='分析四個實例平行ACO實驗結果')
    parser.add_argument('--results-dir', default='results', 
                       help='實驗結果目錄 (預設: results)')
    
    args = parser.parse_args()
    
    analyzer = ExperimentAnalyzer(args.results_dir)
    analyzer.run_complete_analysis()

if __name__ == "__main__":
    main()
