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
Average insertion time for forward star: 0.272606s
Average insertion time for spruce: 1.29543s

Average deletion time for forward star: 0.000392822s
Average deletion time for spruce: 0.000997957s

Average update time for forward star: 0.000610192s
Average update time for spruce: 0.00125707s

Average get neighbours time for forward star: 0.0260862s
Average get neighbours time for spruce: 0.00987412s

Average BFS time for forward star: 0.0272415s
Average BFS time for spruce: 0.0374093s
n = 10000, m = 2560000
Average insertion time for forward star: 0.682434s
Average insertion time for spruce: 2.01927s

Average deletion time for forward star: 0.00102073s
Average deletion time for spruce: 0.00156315s

Average update time for forward star: 0.00117139s
Average update time for spruce: 0.00172978s

Average get neighbours time for forward star: 0.0465517s
Average get neighbours time for spruce: 0.0263994s

Average BFS time for forward star: 0.0428691s
Average BFS time for spruce: 0.10483s
n = 100000, m = 2560000
Average insertion time for forward star: 2.18146s
Average insertion time for spruce: 9.54315s

Average deletion time for forward star: 0.00279569s
Average deletion time for spruce: 0.00182987s

Average update time for forward star: 0.00121493s
Average update time for spruce: 0.000910138s

Average get neighbours time for forward star: 0.224827s
Average get neighbours time for spruce: 0.12838s

Average BFS time for forward star: 0.107466s
Average BFS time for spruce: 0.310493s
```