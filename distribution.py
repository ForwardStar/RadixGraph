import sys
import matplotlib.pyplot as plt
import numpy as np

def process_large_file(file_path, chunk_size=100000):
    counts = {}
    total_numbers = 0
    
    with open(file_path, 'r') as f:
        while True:
            # Read a chunk of lines
            lines = f.readlines(chunk_size)
            if not lines:
                break
            
            # Count occurrences in this chunk
            for line in lines:
                try:
                    number = int(line.strip())
                    counts[number] = counts.get(number, 0) + 1
                    total_numbers += 1
                except ValueError:
                    continue
    
    return counts, total_numbers

def main():
    if len(sys.argv) < 2:
        print("Usage: python distribution.py <filename>")
        return
    
    file_path = sys.argv[-1]

    try:
        # Process the file efficiently
        print("Processing the file...")
        counts, total_numbers = process_large_file(file_path)

        # Convert to NumPy arrays for binning
        numbers = np.array(list(counts.keys()))
        frequencies = np.array([count / total_numbers for count in counts.values()])

        # Bin the data for better visualization
        print("Binning data for optimized visualization...")
        bin_edges = np.linspace(numbers.min(), numbers.max(), num=100)  # Adjust bin count for desired resolution
        binned_frequencies, _ = np.histogram(numbers, bins=bin_edges, weights=frequencies)

        # Calculate bin centers for plotting
        bin_centers = (bin_edges[:-1] + bin_edges[1:]) / 2

        # Plot the histogram
        print("Generating the plot...")
        plt.figure(figsize=(10, 6))
        plt.bar(bin_centers, binned_frequencies, width=(bin_edges[1] - bin_edges[0]), color='blue', alpha=0.7, edgecolor='black')
        plt.title("Number Distribution (Binned)")
        plt.xlabel("Value")
        plt.ylabel("Relative Frequency")
        plt.grid(axis='y', linestyle='--', linewidth=0.7, alpha=0.7)
        plt.tight_layout()

        # Display and save the plot
        plt.savefig("binned_distribution.jpg")
        print("Histogram saved as 'binned_distribution.jpg'")
        plt.show()
    
    except FileNotFoundError:
        print(f"Error: File '{file_path}' not found.")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")

if __name__ == "__main__":
    main()
