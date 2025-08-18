import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
from pathlib import Path
import glob
import os

class ACOExperimentAnalyzer:
    def __init__(self, results_dir="experiment/results"):
        self.results_dir = results_dir
        self.df = None
        self.load_latest_results()
        
    def load_latest_results(self):
        """Load the most recent experiment results"""
        csv_files = glob.glob(f"{self.results_dir}/*.csv")
        if not csv_files:
            raise FileNotFoundError(f"No CSV files found in {self.results_dir}")
        
        latest_file = max(csv_files, key=os.path.getctime)
        print(f"Loading results from: {latest_file}")
        self.df = pd.read_csv(latest_file)
        
        # Basic data info
        print(f"Dataset shape: {self.df.shape}")
        print(f"Instances: {self.df['instance_name'].unique()}")
        print(f"Thread counts: {sorted(self.df['threads'].unique())}")
        print(f"Rounds per configuration: {self.df['round'].max()}")
        
    def generate_analysis_report(self):
        """Generate comprehensive analysis report with visualizations"""
        # Set up matplotlib style
        plt.style.use('seaborn-v0_8')
        sns.set_palette("husl")
        
        # Create figure with subplots
        fig = plt.figure(figsize=(20, 16))
        
        # 1. Execution Time Analysis
        self._plot_execution_time(fig, 1)
        
        # 2. Speedup Analysis
        self._plot_speedup(fig, 2)
        
        # 3. Efficiency Analysis
        self._plot_efficiency(fig, 3)
        
        # 4. Solution Quality Analysis
        self._plot_solution_quality(fig, 4)
        
        # 5. Scalability Heat Map
        self._plot_scalability_heatmap(fig, 5)
        
        # 6. Statistical Summary
        self._plot_statistical_summary(fig, 6)
        
        plt.tight_layout(pad=3.0)
        
        # Save the complete analysis
        output_file = f"{self.results_dir}/aco_analysis_report.png"
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        print(f"Analysis report saved to: {output_file}")
        plt.show()
        
        # Generate detailed statistical report
        self._generate_statistical_report()
        
    def _plot_execution_time(self, fig, pos):
        """Plot execution time comparison"""
        ax = plt.subplot(2, 3, pos)
        
        # Calculate average execution time
        avg_time = self.df.groupby(['instance_name', 'threads'])['execution_time_ms'].mean().reset_index()
        
        for instance in avg_time['instance_name'].unique():
            instance_data = avg_time[avg_time['instance_name'] == instance]
            plt.plot(instance_data['threads'], instance_data['execution_time_ms'], 
                    marker='o', linewidth=2, label=instance)
        
        plt.xlabel('Number of Threads')
        plt.ylabel('Execution Time (ms)')
        plt.title('Execution Time vs Thread Count')
        plt.legend()
        plt.grid(True, alpha=0.3)
        plt.yscale('log')
        
    def _plot_speedup(self, fig, pos):
        """Plot speedup analysis"""
        ax = plt.subplot(2, 3, pos)
        
        # Calculate average speedup
        avg_speedup = self.df.groupby(['instance_name', 'threads'])['speedup'].mean().reset_index()
        
        for instance in avg_speedup['instance_name'].unique():
            instance_data = avg_speedup[avg_speedup['instance_name'] == instance]
            plt.plot(instance_data['threads'], instance_data['speedup'], 
                    marker='s', linewidth=2, label=instance)
        
        # Add ideal speedup line
        threads = sorted(self.df['threads'].unique())
        plt.plot(threads, threads, 'k--', alpha=0.7, label='Ideal Speedup')
        
        plt.xlabel('Number of Threads')
        plt.ylabel('Speedup')
        plt.title('Parallel Speedup')
        plt.legend()
        plt.grid(True, alpha=0.3)
        
    def _plot_efficiency(self, fig, pos):
        """Plot efficiency analysis"""
        ax = plt.subplot(2, 3, pos)
        
        # Calculate average efficiency
        avg_efficiency = self.df.groupby(['instance_name', 'threads'])['efficiency'].mean().reset_index()
        
        for instance in avg_efficiency['instance_name'].unique():
            instance_data = avg_efficiency[avg_efficiency['instance_name'] == instance]
            plt.plot(instance_data['threads'], instance_data['efficiency'], 
                    marker='^', linewidth=2, label=instance)
        
        # Add 100% efficiency line
        threads = sorted(self.df['threads'].unique())
        plt.plot(threads, [100]*len(threads), 'k--', alpha=0.7, label='Perfect Efficiency')
        
        plt.xlabel('Number of Threads')
        plt.ylabel('Efficiency (%)')
        plt.title('Parallel Efficiency')
        plt.legend()
        plt.grid(True, alpha=0.3)
        plt.ylim(0, 110)
        
    def _plot_solution_quality(self, fig, pos):
        """Plot solution quality (ratio to best known)"""
        ax = plt.subplot(2, 3, pos)
        
        # Box plot showing distribution of solution quality
        sns.boxplot(data=self.df, x='instance_name', y='ratio_to_best', hue='threads')
        plt.xlabel('TSP Instance')
        plt.ylabel('Ratio to Best Known Solution')
        plt.title('Solution Quality Distribution')
        plt.xticks(rotation=45)
        plt.legend(title='Threads', bbox_to_anchor=(1.05, 1), loc='upper left')
        
    def _plot_scalability_heatmap(self, fig, pos):
        """Plot scalability heatmap"""
        ax = plt.subplot(2, 3, pos)
        
        # Create pivot table for heatmap
        heatmap_data = self.df.groupby(['instance_name', 'threads'])['efficiency'].mean().unstack()
        
        sns.heatmap(heatmap_data, annot=True, fmt='.1f', cmap='RdYlGn', 
                   center=50, cbar_kws={'label': 'Efficiency (%)'})
        plt.title('Efficiency Heatmap')
        plt.xlabel('Number of Threads')
        plt.ylabel('TSP Instance')
        
    def _plot_statistical_summary(self, fig, pos):
        """Plot statistical summary"""
        ax = plt.subplot(2, 3, pos)
        
        # Calculate summary statistics
        summary_stats = []
        for instance in self.df['instance_name'].unique():
            for threads in sorted(self.df['threads'].unique()):
                subset = self.df[(self.df['instance_name'] == instance) & 
                               (self.df['threads'] == threads)]
                if not subset.empty:
                    stats = {
                        'Instance': instance,
                        'Threads': threads,
                        'Avg_Time': subset['execution_time_ms'].mean(),
                        'Std_Time': subset['execution_time_ms'].std(),
                        'Avg_Ratio': subset['ratio_to_best'].mean(),
                        'Efficiency': subset['efficiency'].mean()
                    }
                    summary_stats.append(stats)
        
        summary_df = pd.DataFrame(summary_stats)
        
        # Plot coefficient of variation for execution time
        summary_df['CV'] = summary_df['Std_Time'] / summary_df['Avg_Time'] * 100
        
        for instance in summary_df['Instance'].unique():
            instance_data = summary_df[summary_df['Instance'] == instance]
            plt.plot(instance_data['Threads'], instance_data['CV'], 
                    marker='D', linewidth=2, label=instance)
        
        plt.xlabel('Number of Threads')
        plt.ylabel('Coefficient of Variation (%)')
        plt.title('Execution Time Variability')
        plt.legend()
        plt.grid(True, alpha=0.3)
        
    def _generate_statistical_report(self):
        """Generate detailed statistical report"""
        report_file = f"{self.results_dir}/statistical_analysis_report.md"
        
        with open(report_file, 'w', encoding='utf-8') as f:
            f.write("# ACO Parallel Performance Analysis Report\n\n")
            f.write(f"Generated on: {pd.Timestamp.now().strftime('%Y-%m-%d %H:%M:%S')}\n\n")
            
            # Dataset overview
            f.write("## Dataset Overview\n\n")
            f.write(f"- **Total experiments**: {len(self.df)}\n")
            f.write(f"- **TSP instances**: {', '.join(self.df['instance_name'].unique())}\n")
            f.write(f"- **Thread configurations**: {', '.join(map(str, sorted(self.df['threads'].unique())))}\n")
            f.write(f"- **Rounds per configuration**: {self.df['round'].max()}\n")
            f.write(f"- **Iterations per run**: {self.df['iterations'].iloc[0]}\n\n")
            
            # Performance summary by instance
            f.write("## Performance Summary by Instance\n\n")
            
            for instance in sorted(self.df['instance_name'].unique()):
                instance_data = self.df[self.df['instance_name'] == instance]
                f.write(f"### {instance}\n\n")
                
                # Calculate cities and best known solution
                cities = instance_data['instance_name'].map({
                    'eil51': 51, 'kroA100': 100, 'ch150': 150, 'gr202': 202
                }).iloc[0]
                
                best_known = {
                    'eil51': 426.0, 'kroA100': 21282.0, 
                    'ch150': 6528.0, 'gr202': 40160.0
                }[instance]
                
                f.write(f"- **Cities**: {cities}\n")
                f.write(f"- **Best known solution**: {best_known}\n\n")
                
                # Performance by thread count
                f.write("| Threads | Avg Time (ms) | Std Dev | Speedup | Efficiency (%) | Avg Ratio |\n")
                f.write("|---------|---------------|---------|---------|----------------|-----------|\\n")
                
                for threads in sorted(instance_data['threads'].unique()):
                    thread_data = instance_data[instance_data['threads'] == threads]
                    avg_time = thread_data['execution_time_ms'].mean()
                    std_time = thread_data['execution_time_ms'].std()
                    avg_speedup = thread_data['speedup'].mean()
                    avg_efficiency = thread_data['efficiency'].mean()
                    avg_ratio = thread_data['ratio_to_best'].mean()
                    
                    f.write(f"| {threads} | {avg_time:.0f} | {std_time:.0f} | "
                           f"{avg_speedup:.2f} | {avg_efficiency:.1f} | {avg_ratio:.2f} |\n")
                
                f.write("\n")
            
            # Key findings
            f.write("## Key Findings\n\n")
            
            # Best efficiency by instance
            f.write("### Best Efficiency by Instance\n\n")
            best_efficiency = self.df.groupby('instance_name')['efficiency'].max()
            for instance, eff in best_efficiency.items():
                optimal_threads = self.df[
                    (self.df['instance_name'] == instance) & 
                    (self.df['efficiency'] == eff)
                ]['threads'].iloc[0]
                f.write(f"- **{instance}**: {eff:.1f}% (with {optimal_threads} thread{'s' if optimal_threads > 1 else ''})\n")
            
            f.write("\n### Scalability Analysis\n\n")
            
            # Overall scalability
            avg_efficiency_1 = self.df[self.df['threads'] == 1]['efficiency'].mean()
            avg_efficiency_8 = self.df[self.df['threads'] == 8]['efficiency'].mean()
            
            f.write(f"- **Single-thread efficiency**: {avg_efficiency_1:.1f}%\n")
            f.write(f"- **8-thread efficiency**: {avg_efficiency_8:.1f}%\n")
            f.write(f"- **Efficiency drop**: {avg_efficiency_1 - avg_efficiency_8:.1f} percentage points\n\n")
            
            # Solution quality consistency
            f.write("### Solution Quality Analysis\n\n")
            quality_stats = self.df.groupby('threads')['ratio_to_best'].agg(['mean', 'std'])
            f.write("| Threads | Avg Ratio | Std Dev | Quality Impact |\n")
            f.write("|---------|-----------|---------|----------------|\n")
            
            for threads in sorted(quality_stats.index):
                avg_ratio = quality_stats.loc[threads, 'mean']
                std_ratio = quality_stats.loc[threads, 'std']
                quality_impact = "Stable" if std_ratio < 0.1 else "Variable"
                f.write(f"| {threads} | {avg_ratio:.3f} | {std_ratio:.3f} | {quality_impact} |\n")
            
            f.write("\n### Recommendations\n\n")
            f.write("Based on the analysis:\n\n")
            f.write("1. **Single-thread performance** is generally most efficient for these problem sizes\n")
            f.write("2. **Parallel overhead** dominates the benefits for small to medium TSP instances\n")
            f.write("3. **Solution quality** remains consistent across thread configurations\n")
            f.write("4. **Larger problem instances** may benefit more from parallelization\n")
            f.write("5. **Algorithm tuning** may be needed to improve parallel efficiency\n")
        
        print(f"Statistical report saved to: {report_file}")

def main():
    try:
        analyzer = ACOExperimentAnalyzer()
        analyzer.generate_analysis_report()
        print("\n✅ Analysis completed successfully!")
        
    except Exception as e:
        print(f"❌ Error during analysis: {e}")
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())
