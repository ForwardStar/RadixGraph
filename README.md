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
Average insertion time for forward star: 0.220131s
Average insertion time for spruce: 0.840989s

Average deletion time for forward star: 0.000404183s
Average deletion time for spruce: 0.000804307s

Average update time for forward star: 0.0005952s
Average update time for spruce: 0.00101043s

Average get neighbours time for forward star: 0.0627918s
Average get neighbours time for spruce: 0.00937988s

Average BFS time for forward star: 0.0682509s
Average BFS time for spruce: 0.0341235s
n = 10000, m = 2560000
Average insertion time for forward star: 0.693987s
Average insertion time for spruce: 0.976988s

Average deletion time for forward star: 0.000966067s
Average deletion time for spruce: 0.00128916s

Average update time for forward star: 0.00458114s
Average update time for spruce: 0.0014956s

Average get neighbours time for forward star: 0.0890751s
Average get neighbours time for spruce: 0.0212527s

Average BFS time for forward star: 0.0895781s
Average BFS time for spruce: 0.081896s
n = 100000, m = 2560000
Average insertion time for forward star: 2.08913s
Average insertion time for spruce: 1.66511s

Average deletion time for forward star: 0.00106358s
Average deletion time for spruce: 0.00129984s

Average update time for forward star: 0.00108019s
Average update time for spruce: 0.000555301s

Average get neighbours time for forward star: 0.204141s
Average get neighbours time for spruce: 0.0839405s

Average BFS time for forward star: 0.134669s
Average BFS time for spruce: 0.206742s
n = 1000000, m = 2560000
Average insertion time for forward star: 2.88715s
Average insertion time for spruce: 4.58948s

Average deletion time for forward star: 0.00127693s
Average deletion time for spruce: 0.00132708s

Average update time for forward star: 0.00127251s
Average update time for spruce: 0.000322965s

Average get neighbours time for forward star: 1.15154s
Average get neighbours time for spruce: 0.49362s

Average BFS time for forward star: 0.694269s
Average BFS time for spruce: 1.30109s
```