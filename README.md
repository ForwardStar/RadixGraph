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
Average insertion time for forward star: 1.29279s
Average insertion time for spruce: 2.15327s

Average deletion time for forward star: 0.00214593s
Average deletion time for spruce: 0.00117303s

Average update time for forward star: 0.00241535s
Average update time for spruce: 0.00370744s

Average get neighbours time for forward star: 0.0451568s
Average get neighbours time for spruce: 0.00300002s

Average BFS time for forward star: 0.0793586s
Average BFS time for spruce: 0.0805424s
n = 10000, m = 10000000
Average insertion time for forward star: 1.1991s
Average insertion time for spruce: 0.774401s

Average deletion time for forward star: 0.0023802s
Average deletion time for spruce: 0.00512009s

Average update time for forward star: 0.00337051s
Average update time for spruce: 0.0155731s

Average get neighbours time for forward star: 0.0366013s
Average get neighbours time for spruce: 0.00448644s

Average BFS time for forward star: 0.151998s
Average BFS time for spruce: 0.182724s
n = 100000, m = 10000000
Average insertion time for forward star: 1.41005s
Average insertion time for spruce: 1.41637s

Average deletion time for forward star: 0.00272023s
Average deletion time for spruce: 0.0323601s

Average update time for forward star: 0.00251565s
Average update time for spruce: 0.0300784s

Average get neighbours time for forward star: 0.0427077s
Average get neighbours time for spruce: 0.0135971s

Average BFS time for forward star: 0.305337s
Average BFS time for spruce: 0.439236s
n = 1000000, m = 10000000
Average insertion time for forward star: 3.70127s
Average insertion time for spruce: 5.23783s

Average deletion time for forward star: 0.00444836s
Average deletion time for spruce: 0.0556736s

Average update time for forward star: 0.00635591s
Average update time for spruce: 0.000757161s

Average get neighbours time for forward star: 0.309235s
Average get neighbours time for spruce: 0.068025s

Average BFS time for forward star: 1.15709s
Average BFS time for spruce: 1.55966s
```