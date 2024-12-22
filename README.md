# Forward_star
A graph data structure for optimizing graph traversals and graph OLAP tasks.

# Prerequisite
```
libtbb-dev 2021.5.0
```

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
m = [10000000, 10000000, 10000000, 10000000]
log(u) = 30
```
Trie settings:
```cpp
std::vector<int> d = {3, 3, 3, 3};
std::vector<std::vector<int>> a = {
    {16, 7, 7},
    {19, 6, 5},
    {21, 5, 4},
    {16, 9, 5},
};
```

# Experimental results (16-threaded)
```
n = 1000, m = 10000000
Average insertion time for forward star: 1.21344s
Average insertion time for spruce: 2.48354s

Average deletion time for forward star: 0.00199824s
Average deletion time for spruce: 0.00116301s

Average update time for forward star: 0.00374285s
Average update time for spruce: 0.00583736s

Average get neighbours time for forward star: 0.0469012s
Average get neighbours time for spruce: 0.00355675s

Average BFS time for forward star: 0.0894005s
Average BFS time for spruce: 0.0902526s
n = 10000, m = 10000000
Average insertion time for forward star: 1.09347s
Average insertion time for spruce: 0.90108s

Average deletion time for forward star: 0.00238862s
Average deletion time for spruce: 0.0123387s

Average update time for forward star: 0.00227458s
Average update time for spruce: 0.0318565s

Average get neighbours time for forward star: 0.0444252s
Average get neighbours time for spruce: 0.00411186s

Average BFS time for forward star: 0.15073s
Average BFS time for spruce: 0.176383s
n = 100000, m = 10000000
Average insertion time for forward star: 1.4136s
Average insertion time for spruce: 1.77962s

Average deletion time for forward star: 0.00258604s
Average deletion time for spruce: 0.0444091s

Average update time for forward star: 0.00417859s
Average update time for spruce: 0.0420873s

Average get neighbours time for forward star: 0.0490874s
Average get neighbours time for spruce: 0.0151101s

Average BFS time for forward star: 0.347068s
Average BFS time for spruce: 0.53081s
n = 1000000, m = 10000000
Average insertion time for forward star: 3.35699s
Average insertion time for spruce: 5.02225s

Average deletion time for forward star: 0.00341613s
Average deletion time for spruce: 0.054329s

Average update time for forward star: 0.00355786s
Average update time for spruce: 0.000717927s

Average get neighbours time for forward star: 0.282689s
Average get neighbours time for spruce: 0.0588298s

Average BFS time for forward star: 1.11858s
Average BFS time for spruce: 1.39421s
```