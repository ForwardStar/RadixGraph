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
colors = 'teal'
methods = 'SORT'

def parse_memory_file(filename="memory_log.txt"):
    """Parse memory consumption data from one file."""
    data = []
    with open(filename, "r") as f:
        for line in f:
            a, b = line.split()
            a = int(a)
            b = int(b)
            data.append((a, b / (1024 * 1024)))  # bytes → MB
    return data

def plot_memory(data, output_path):
    fig, ax = plt.subplots(figsize=(8, 4))

    # Drop the initial memory and take 10 items
    data = data[1:100]
    inserted = [x[0] for x in data]
    memory = [x[1] for x in data]
    max_inserted = max(inserted)
    x = [i for i in range(99)]
    ax.plot(
        x,
        memory,
        color=colors,
        label=methods,
        linewidth=2,
    )

    ax.scatter([0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 98],
               [memory[i] for i in [0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 98]],
               color=colors, s=100)

    ax.set_xlabel("Number of inserted vertices", fontsize=30, fontweight='bold')
    ax.set_ylabel("Memory (MB)", fontsize=30, fontweight='bold')
    ax.tick_params(axis='y', labelsize=25)
    ax.tick_params(axis='x', labelsize=25)
    ax.set_xticks(x)
    # create x tick labels 1e6, 2e6, ..., 9e6, leaving empty for other ticks
    x_labels = [f"{(i+1) // 10}e6" if (i+1) % 10 == 0 else "" for i in range(99)]
    ax.set_xticklabels(x_labels)
    # ax.legend(
    #     loc='lower center',
    #     bbox_to_anchor=(0.5, 1.02),
    #     handlelength=1.0,
    #     handletextpad=0.3,
    #     columnspacing=0.5,
    #     fontsize=25,
    #     ncol=4
    # )

    plt.tight_layout()
    plt.savefig(output_path)

if __name__ == "__main__":
    if not os.path.exists("./figures"):
        os.makedirs("./figures")
        
    data = parse_memory_file("memory_log.txt")
    plot_memory(data, "figures/memory_log.pdf")
