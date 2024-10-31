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
log(u) = 40
```
Trie settings:
```cpp
std::vector<int> d = {4, 4, 4, 4};
std::vector<std::vector<int>> a = {
    {17, 8, 8, 7},
    {20, 7, 7, 6},
    {22, 6, 6, 6},
    {25, 5, 5, 5}
};
```

# Experimental results (single-thread)
```
n = 1000, m = 2560000
Average insertion time for forward star: 0.317191s
Average insertion time for spruce: 1.555s

Average deletion time for forward star: 0.000667146s
Average deletion time for spruce: 0.00260704s

Average update time for forward star: 0.00317112s
Average update time for spruce: 0.00156392s

Average get neighbours time for forward star: 0.0347924s
Average get neighbours time for spruce: 0.0131592s

Average BFS time for forward star: 0.0295231s
Average BFS time for spruce: 0.0454799s
n = 10000, m = 2560000
Average insertion time for forward star: 0.927163s
Average insertion time for spruce: 2.06268s

Average deletion time for forward star: 0.000983596s
Average deletion time for spruce: 0.00137892s

Average update time for forward star: 0.00116425s
Average update time for spruce: 0.00174766s

Average get neighbours time for forward star: 0.032982s
Average get neighbours time for spruce: 0.0241093s

Average BFS time for forward star: 0.0325782s
Average BFS time for spruce: 0.11545s
n = 100000, m = 2560000
Average insertion time for forward star: 1.84588s
Average insertion time for spruce: 8.38354s

Average deletion time for forward star: 0.00249724s
Average deletion time for spruce: 0.00156386s

Average update time for forward star: 0.0014854s
Average update time for spruce: 0.000832649s

Average get neighbours time for forward star: 0.14688s
Average get neighbours time for spruce: 0.125032s

Average BFS time for forward star: 0.058927s
Average BFS time for spruce: 0.27481s
```