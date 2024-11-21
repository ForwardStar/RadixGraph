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
Average insertion time for forward star: 0.213548s
Average insertion time for spruce: 0.948861s

Average deletion time for forward star: 0.000229453s
Average deletion time for spruce: 0.000793248s

Average update time for forward star: 0.000364139s
Average update time for spruce: 0.00102304s

Average get neighbours time for forward star: 0.0262086s
Average get neighbours time for spruce: 0.00802368s

Average BFS time for forward star: 0.0261453s
Average BFS time for spruce: 0.0380498s
n = 10000, m = 2560000
Average insertion time for forward star: 0.52087s
Average insertion time for spruce: 1.52912s

Average deletion time for forward star: 0.00133935s
Average deletion time for spruce: 0.00123957s

Average update time for forward star: 0.00164078s
Average update time for spruce: 0.00141499s

Average get neighbours time for forward star: 0.039939s
Average get neighbours time for spruce: 0.0213993s

Average BFS time for forward star: 0.0298565s
Average BFS time for spruce: 0.0956646s
n = 100000, m = 2560000
Average insertion time for forward star: 2.1011s
Average insertion time for spruce: 8.80128s

Average deletion time for forward star: 0.000970202s
Average deletion time for spruce: 0.00173658s

Average update time for forward star: 0.00103964s
Average update time for spruce: 0.000873795s

Average get neighbours time for forward star: 0.142407s
Average get neighbours time for spruce: 0.123521s

Average BFS time for forward star: 0.0623936s
Average BFS time for spruce: 0.296055s
```

# Experimental results (10-threaded)
```
n = 1000, m = 2560000
Average insertion time for forward star: 0.0987028s
Average insertion time for spruce: 0.230811s

Average deletion time for forward star: 0.000110953s
Average deletion time for spruce: 0.000167953s

Average update time for forward star: 0.00025142s
Average update time for spruce: 0.0002134s

Average get neighbours time for forward star: 0.0185455s
Average get neighbours time for spruce: 0.00144376s

Average BFS time for forward star: 0.0391558s
Average BFS time for spruce: 0.0396483s
n = 10000, m = 2560000
Average insertion time for forward star: 0.215447s
Average insertion time for spruce: 1.10134s

Average deletion time for forward star: 0.000233898s
Average deletion time for spruce: 0.000232986s

Average update time for forward star: 0.000397018s
Average update time for spruce: 0.000272616s

Average get neighbours time for forward star: 0.017672s
Average get neighbours time for spruce: 0.00379529s

Average BFS time for forward star: 0.0493785s
Average BFS time for spruce: 0.0935286s
n = 100000, m = 2560000
Average insertion time for forward star: 0.778672s
Average insertion time for spruce: 9.79436s

Average deletion time for forward star: 0.000432629s
Average deletion time for spruce: 0.00443091s

Average update time for forward star: 0.00050977s
Average update time for spruce: 0.000776657s

Average get neighbours time for forward star: 0.0389308s
Average get neighbours time for spruce: 0.0201453s

Average BFS time for forward star: 0.126319s
Average BFS time for spruce: 0.293667s
```