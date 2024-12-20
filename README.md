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

# Experimental results (10-threaded)
```
n = 1000, m = 10000000
Average insertion time for forward star: 1.17346s
Average insertion time for spruce: 2.31754s

Average deletion time for forward star: 0.00226916s
Average deletion time for spruce: 0.00165744s

Average update time for forward star: 0.00273128s
Average update time for spruce: 0.0036881s

Average get neighbours time for forward star: 0.0623258s
Average get neighbours time for spruce: 0.004028s

Average BFS time for forward star: 0.0865063s
Average BFS time for spruce: 0.0914657s
n = 10000, m = 10000000
Average insertion time for forward star: 1.2624s
Average insertion time for spruce: 0.886746s

Average deletion time for forward star: 0.00263937s
Average deletion time for spruce: 0.00439712s

Average update time for forward star: 0.00305124s
Average update time for spruce: 0.0149488s

Average get neighbours time for forward star: 0.0389517s
Average get neighbours time for spruce: 0.0046724s

Average BFS time for forward star: 0.156787s
Average BFS time for spruce: 0.179689s
n = 100000, m = 10000000
Average insertion time for forward star: 1.56904s
Average insertion time for spruce: 1.27232s

Average deletion time for forward star: 0.00275134s
Average deletion time for spruce: 0.0135495s

Average update time for forward star: 0.00305005s
Average update time for spruce: 0.0211157s

Average get neighbours time for forward star: 0.0441868s
Average get neighbours time for spruce: 0.0181994s

Average BFS time for forward star: 0.294408s
Average BFS time for spruce: 0.41564s
n = 1000000, m = 10000000
Average insertion time for forward star: 3.3533s
Average insertion time for spruce: 4.08117s

Average deletion time for forward star: 0.00374812s
Average deletion time for spruce: 0.0500982s

Average update time for forward star: 0.00379229s
Average update time for spruce: 0.000939211s

Average get neighbours time for forward star: 0.295925s
Average get neighbours time for spruce: 0.08056s

Average BFS time for forward star: 1.12978s
Average BFS time for spruce: 1.46072s
```