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
Average insertion time for forward star: 0.28321s
Average insertion time for spruce: 0.761313s

Average deletion time for forward star: 0.00156843s
Average deletion time for spruce: 0.00488254s

Average update time for forward star: 0.00184594s
Average update time for spruce: 0.00565619s

Average get neighbours time for forward star: 0.0379096s
Average get neighbours time for spruce: 0.00687603s

Average BFS time for forward star: 0.038867s
Average BFS time for spruce: 0.0283245s
n = 10000, m = 2560000
Average insertion time for forward star: 0.632821s
Average insertion time for spruce: 1.1927s

Average deletion time for forward star: 0.00551517s
Average deletion time for spruce: 0.00914668s

Average update time for forward star: 0.00677059s
Average update time for spruce: 0.0110091s

Average get neighbours time for forward star: 0.0541823s
Average get neighbours time for spruce: 0.0179685s

Average BFS time for forward star: 0.0477131s
Average BFS time for spruce: 0.088127s
n = 100000, m = 2560000
Average insertion time for forward star: 2.02057s
Average insertion time for spruce: 5.9477s

Average deletion time for forward star: 0.00857992s
Average deletion time for spruce: 0.0129077s

Average update time for forward star: 0.00945918s
Average update time for spruce: 0.00692304s

Average get neighbours time for forward star: 0.177055s
Average get neighbours time for spruce: 0.098648s

Average BFS time for forward star: 0.095907s
Average BFS time for spruce: 0.256389s
```

# Experimental results (10-threaded)
```
n = 1000, m = 2560000
Average insertion time for forward star: 0.074762s
Average insertion time for spruce: 0.222592s

Average deletion time for forward star: 0.000359859s
Average deletion time for spruce: 0.0012919s

Average update time for forward star: 0.000653866s
Average update time for spruce: 0.00149055s

Average get neighbours time for forward star: 0.0226525s
Average get neighbours time for spruce: 0.00173203s

Average BFS time for forward star: 0.0539062s
Average BFS time for spruce: 0.0366049s
n = 10000, m = 2560000
Average insertion time for forward star: 0.14303s
Average insertion time for spruce: 0.494296s

Average deletion time for forward star: 0.00117986s
Average deletion time for spruce: 0.00257326s

Average update time for forward star: 0.00142393s
Average update time for spruce: 0.00234146s

Average get neighbours time for forward star: 0.0125984s
Average get neighbours time for spruce: 0.00351733s

Average BFS time for forward star: 0.0677944s
Average BFS time for spruce: 0.0853134s
n = 100000, m = 2560000
Average insertion time for forward star: 0.664538s
Average insertion time for spruce: 6.57243s

Average deletion time for forward star: 0.00199863s
Average deletion time for spruce: 0.0262857s

Average update time for forward star: 0.00257133s
Average update time for spruce: 0.00485354s

Average get neighbours time for forward star: 0.0382472s
Average get neighbours time for spruce: 0.0172087s

Average BFS time for forward star: 0.131239s
Average BFS time for spruce: 0.231261s
```