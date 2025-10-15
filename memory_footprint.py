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
colors = ['teal', 'goldenrod', 'darkcyan', 'mediumvioletred', 'slategray', 'indigo']
markers = ['X', 'P', '*', 'D', 's', 'o']
methods = ['SORT', 'vEB']
distributions = ['uniform', 'skewed']  # since power-law deleted

def parse_memory_file(filename):
    """Parse memory consumption data from one file."""
    data = []
    with open(filename, "r") as f:
        for line in f:
            if "Inserting in random order..." in line:
                continue
            match = re.search(r"Memory after (\d+) insertions: (\d+) bytes", line)
            if match:
                inserted = int(match.group(1))
                memory_mb = int(match.group(2)) / (1024 * 1024)  # bytes → MB
                data.append((inserted, memory_mb))
            else:
                break
    return data

def plot_memory(data_dict, output_path):
    fig, ax = plt.subplots(figsize=(11, 6))

    line_id = 0
    for method in methods:
        for dist in distributions:
            data = data_dict.get((method, dist), [])
            if not data:
                continue

            inserted = [x[0] for x in data]
            memory = [x[1] for x in data]
            max_inserted = max(inserted)
            progress = [int(100 * x / max_inserted) for x in inserted]

            ax.plot(
                progress,
                memory,
                color=colors[line_id % len(colors)],
                marker=markers[line_id % len(markers)],
                label=f"{method}-{dist}",
                markersize=15,
                markerfacecolor='white',
                markeredgewidth=1.5,
                linewidth=2,
            )
            line_id += 1

    ax.set_xlabel("Insertion progress (%)", fontsize=35, fontweight='bold')
    ax.set_ylabel("Memory (MB)", fontsize=35, fontweight='bold')
    ax.tick_params(axis='y', labelsize=35)
    ax.tick_params(axis='x', labelsize=35)
    ax.set_xticks([20, 40, 60, 80, 100])
    ax.legend(
        loc='lower center',
        bbox_to_anchor=(0.5, 1.02),
        handlelength=1.0,
        handletextpad=0.3,
        columnspacing=0.5,
        fontsize=25,
        ncol=4
    )

    plt.tight_layout()
    plt.savefig(output_path)

if __name__ == "__main__":
    if not os.path.exists("./figures"):
        os.makedirs("./figures")

    # Load data from six files
    workloads = {}
    for method in methods:
        for dist in distributions:
            filename = f"results_{dist}_{method}.txt"
            if os.path.exists(filename):
                workloads[(method, dist)] = parse_memory_file(filename)
            else:
                workloads[(method, dist)] = []
    
    print(workloads)

    plot_memory(workloads, "figures/memory_progress.pdf")
