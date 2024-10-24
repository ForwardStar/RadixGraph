# Forward_star
A graph data structure for optimizing graph traversals and graph OLAP tasks.

# Compile and run
```sh
git clone https://github.com/ForwardStar/forward_star.git --recurse-submodules
cmake .
make
./forward_star
```

# Trie and test data setting
Test settings:
```
n = [1000, 10000, 100000, 1000000]
m = [2560000, 2560000, 2560000, 2560000]
log(u) = 32
```
Trie settings:
```cpp
std::vector<int> d = {4, 4, 4, 4};
std::vector<std::vector<int>> a = {
    {15, 6, 6, 5},
    {18, 5, 5, 4},
    {20, 4, 4, 4},
    {23, 3, 3, 3}
};
```

# Experimental results (single-thread)
```
n = 1000, m = 2560000
Average insertion time for adjacency array list: 2.52088s
Average insertion time for forward star: 0.797079s
Average insertion time for spruce: 1.67094s

Average deletion time for adjacency array list: 0.0520439s
Average deletion time for forward star: 0.00127037s
Average deletion time for spruce: 0.00170915s

Average update time for adjacency array list: 0.0320712s
Average update time for forward star: 0.00138696s
Average update time for spruce: 0.00192409s

Average get neighbours time for adjacency array list: 0.0679188s
Average get neighbours time for forward star: 0.151369s
Average get neighbours time for spruce: 0.0289054s

Average BFS time for adjacency array list: 0.616683s
Average BFS time for forward star: 0.114798s
Average BFS time for spruce: 0.320593s
n = 10000, m = 2560000
Average insertion time for adjacency array list: 5.83998s
Average insertion time for forward star: 1.92384s
Average insertion time for spruce: 2.87243s

Average deletion time for adjacency array list: 0.0614521s
Average deletion time for forward star: 0.00271533s
Average deletion time for spruce: 0.00328981s

Average update time for adjacency array list: 0.0377461s
Average update time for forward star: 0.00282384s
Average update time for spruce: 0.00349228s

Average get neighbours time for adjacency array list: 0.147444s
Average get neighbours time for forward star: 0.280221s
Average get neighbours time for spruce: 0.079949s

Average BFS time for adjacency array list: 1.31962s
Average BFS time for forward star: 0.278035s
Average BFS time for spruce: 0.70151s
n = 100000, m = 2560000
Average insertion time for adjacency array list: 11.792s
Average insertion time for forward star: 5.14623s
Average insertion time for spruce: 4.51042s

Average deletion time for adjacency array list: 0.0646536s
Average deletion time for forward star: 0.00423784s
Average deletion time for spruce: 0.00462423s

Average update time for adjacency array list: 0.0405701s
Average update time for forward star: 0.00432614s
Average update time for spruce: 0.00409993s

Average get neighbours time for adjacency array list: 0.51531s
Average get neighbours time for forward star: 0.673643s
Average get neighbours time for spruce: 0.403761s

Average BFS time for adjacency array list: 2.39983s
Average BFS time for forward star: 0.545586s
Average BFS time for spruce: 1.67174s
n = 1000000, m = 2560000
Average insertion time for adjacency array list: 22.2915s
Average insertion time for forward star: 9.88632s
Average insertion time for spruce: 6.85038s

Average deletion time for adjacency array list: 0.0690055s
Average deletion time for forward star: 0.00668636s
Average deletion time for spruce: 0.00606856s

Average update time for adjacency array list: 0.0471526s
Average update time for forward star: 0.00706516s
Average update time for spruce: 0.00467955s

Average get neighbours time for adjacency array list: 3.10808s
Average get neighbours time for forward star: 2.16904s
Average get neighbours time for spruce: 1.37649s

Average BFS time for adjacency array list: 7.03649s
Average BFS time for forward star: 1.79484s
Average BFS time for spruce: 4.45755s
```