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
Average insertion time for adjacency array list: 2.36488s
Average insertion time for forward star: 0.853144s
Average insertion time for spruce: 1.36344s

Average deletion time for adjacency array list: 0.0330891s
Average deletion time for forward star: 0.000904424s
Average deletion time for spruce: 0.00139685s

Average update time for adjacency array list: 0.0191755s
Average update time for forward star: 0.000858497s
Average update time for spruce: 0.00146376s

Average get neighbours time for adjacency array list: 0.0560246s
Average get neighbours time for forward star: 0.0977176s
Average get neighbours time for spruce: 0.0224132s

Average BFS time for adjacency array list: 0.540819s
Average BFS time for forward star: 0.104633s
Average BFS time for spruce: 0.301675s
n = 10000, m = 2560000
Average insertion time for adjacency array list: 5.44045s
Average insertion time for forward star: 2.16357s
Average insertion time for spruce: 2.47838s

Average deletion time for adjacency array list: 0.0425889s
Average deletion time for forward star: 0.0023267s
Average deletion time for spruce: 0.00284981s

Average update time for adjacency array list: 0.0250427s
Average update time for forward star: 0.00227047s
Average update time for spruce: 0.0029819s

Average get neighbours time for adjacency array list: 0.134241s
Average get neighbours time for forward star: 0.22111s
Average get neighbours time for spruce: 0.0726453s

Average BFS time for adjacency array list: 1.15065s
Average BFS time for forward star: 0.241975s
Average BFS time for spruce: 0.667464s
n = 100000, m = 2560000
Average insertion time for adjacency array list: 10.2489s
Average insertion time for forward star: 5.15151s
Average insertion time for spruce: 3.99154s

Average deletion time for adjacency array list: 0.0456521s
Average deletion time for forward star: 0.00380396s
Average deletion time for spruce: 0.0040841s

Average update time for adjacency array list: 0.0275479s
Average update time for forward star: 0.0037414s
Average update time for spruce: 0.00355418s

Average get neighbours time for adjacency array list: 0.402207s
Average get neighbours time for forward star: 0.480203s
Average get neighbours time for spruce: 0.313739s

Average BFS time for adjacency array list: 2.09278s
Average BFS time for forward star: 0.494068s
Average BFS time for spruce: 1.31163s
n = 1000000, m = 2560000
Average insertion time for adjacency array list: 18.9969s
Average insertion time for forward star: 9.40753s
Average insertion time for spruce: 5.82777s

Average deletion time for adjacency array list: 0.0486667s
Average deletion time for forward star: 0.00577824s
Average deletion time for spruce: 0.00526983s

Average update time for adjacency array list: 0.0304336s
Average update time for forward star: 0.00566967s
Average update time for spruce: 0.00399076s

Average get neighbours time for adjacency array list: 2.52301s
Average get neighbours time for forward star: 1.7439s
Average get neighbours time for spruce: 1.09819s

Average BFS time for adjacency array list: 5.94438s
Average BFS time for forward star: 1.4929s
Average BFS time for spruce: 3.5103s
```