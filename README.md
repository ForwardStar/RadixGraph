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
Average insertion time for forward star: 0.256867s
Average insertion time for spruce: 1.20236s

Average deletion time for forward star: 0.000483987s
Average deletion time for spruce: 0.00111149s

Average update time for forward star: 0.000686752s
Average update time for spruce: 0.00144494s

Average get neighbours time for forward star: 0.0281567s
Average get neighbours time for spruce: 0.0129719s

Average BFS time for forward star: 0.0278151s
Average BFS time for spruce: 0.0396961s
n = 10000, m = 2560000
Average insertion time for forward star: 0.642792s
Average insertion time for spruce: 1.43411s

Average deletion time for forward star: 0.000900954s
Average deletion time for spruce: 0.0014237s

Average update time for forward star: 0.00101429s
Average update time for spruce: 0.00163901s

Average get neighbours time for forward star: 0.0466736s
Average get neighbours time for spruce: 0.0246321s

Average BFS time for forward star: 0.0440154s
Average BFS time for spruce: 0.10656s
n = 100000, m = 2560000
Average insertion time for forward star: 2.95902s
Average insertion time for spruce: 2.45845s

Average deletion time for forward star: 0.00130816s
Average deletion time for spruce: 0.00148318s

Average update time for forward star: 0.0015603s
Average update time for spruce: 0.000655202s

Average get neighbours time for forward star: 0.145161s
Average get neighbours time for spruce: 0.0851373s

Average BFS time for forward star: 0.0746698s
Average BFS time for spruce: 0.208836s
n = 1000000, m = 2560000
Average insertion time for forward star: 6.52883s
Average insertion time for spruce: 8.2779s

Average deletion time for forward star: 0.00228951s
Average deletion time for spruce: 0.00244267s

Average update time for forward star: 0.00231117s
Average update time for spruce: 0.0010021s

Average get neighbours time for forward star: 2.49249s
Average get neighbours time for spruce: 1.4729s

Average BFS time for forward star: 1.43295s
Average BFS time for spruce: 4.42865s
```