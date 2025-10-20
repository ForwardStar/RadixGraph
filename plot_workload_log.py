import re
import os
import matplotlib.pyplot as plt
import matplotlib.font_manager as fm
import numpy as np
from matplotlib import rcParams

# === Font setup (Times/Libertine if available) ===
plt.rcParams.update({'font.family': 'Times New Roman'})
for font in fm.findSystemFonts(fontpaths=None, fontext='ttf'):
    if "Libertine_R" in font:
        font_prop = fm.FontProperties(fname=font)
        font_name = font_prop.get_name()
        plt.rcParams.update({'font.family': font_name})
        rcParams['font.family'] = font_name
        rcParams['mathtext.fontset'] = 'custom'
        rcParams['mathtext.rm'] = font_name

# === Style setup ===
colors = ['teal', 'orange']
methods = ['SORT', 'vEB']
markers = ['o', 's']
memory_u = [
    [],
    []
]
memory_s = [
    [],
    []
]
memory_h = [
    [],
    []
]

def parse_memory_file():
    global memory_u, memory_s, memory_h
    for filename in ["workload_u_log.txt", "workload_s_log.txt", "workload_h_log.txt"]:
        with open(filename, "r") as f:
            for line in f:
                a, b, c = line.split()
                a = int(a)
                b = int(b)
                c = int(c)
                if filename == "workload_u_log.txt":
                    memory_u[0].append(b / (1024 * 1024))
                    memory_u[1].append(c / (1024 * 1024))
                elif filename == "workload_s_log.txt":
                    memory_s[0].append(b / (1024 * 1024))
                    memory_s[1].append(c / (1024 * 1024))
                else:
                    memory_h[0].append(b / (1024 * 1024))
                    memory_h[1].append(c / (1024 * 1024))

def plot_memory(ax, data):
    # Drop the initial memory and take 10 items
    for i in range(len(data)):
        x = ['10%', '20%', '30%', '40%', '50%', '60%', '70%', '80%', '90%', '100%']
        y = data[i]
        ax.plot(
            x,
            y,
            color=colors[i],
            label=methods[i],
            marker=markers[i],
            markersize=15,
            markerfacecolor='white',
            markeredgewidth=1.5,
            linewidth=2,
        )
    ax.set_ylabel("Memory (MB)", fontsize=35, fontweight='bold')
    ax.tick_params(axis='y', labelsize=30)
    ax.tick_params(axis='x', labelsize=30)
    x_ticks = ['', '20%', '', '40%', '', '60%', '', '80%', '', '100%']
    ax.set_xticks(np.arange(len(x_ticks)))
    ax.set_xticklabels(x_ticks, fontsize=30)
    ax.tick_params(axis='y', labelsize=30)

if __name__ == "__main__":
    if not os.path.exists("./figures"):
        os.makedirs("./figures")
    
    parse_memory_file()
    fig, axes = plt.subplots(1, 3, figsize=(25, 7), sharey=False)
    axes = axes.flatten()
    plot_memory(axes[0], memory_u)
    axes[0].set_xlabel("(a) Uniform", fontsize=35, fontweight='bold')
    plot_memory(axes[1], memory_s)
    axes[1].set_xlabel("(b) Skewed", fontsize=35, fontweight='bold')
    plot_memory(axes[2], memory_h)
    axes[2].set_xlabel("(c) Heavy-tailed", fontsize=35, fontweight='bold')
    handles, labels = axes[0].get_legend_handles_labels()
    fig.legend(handles, labels,
            loc="upper center",           # center horizontally
            ncol=len(methods), fontsize=40)
    plt.tight_layout(rect=[0, 0, 1, 0.8])  # leave space for the legend
    plt.savefig("./figures/workload_log.pdf", bbox_inches='tight')
