# RadixGraph
A fast and space-efficient graph data structure. For full experiments, please refer to [gfe_driver_RadixGraph](https://github.com/ForwardStar/gfe_driver).

See APIs with comments from ``src/radixgraph.h``.

# Compile and run
You need to run RadixGraph on Linux platform with openMP. We recommend using compiler ``GCC 10+``.
```sh
git clone https://github.com/ForwardStar/RadixGraph.git --recurse-submodules
cmake .
make
./radixgraph
```

# Trie and test data setting
This demo randomly generates a graph of n vertices, m edges and the vertex ids are within [0, u-1].

Test settings:
```
n = [10000, 100000, 1000000]
m = [10000000, 10000000, 10000000]
log(u) = 30
```
Trie settings:
```cpp
std::vector<int> d = {3, 3, 3};
std::vector<std::vector<int>> a = {
    {19, 6, 5},
    {21, 5, 4},
    {23, 4, 3},
};
```

# Experimental results (16-threaded)
```
n = 10000, m = 10000000
Average insertion time for RadixGraph: 0.634027s
Average insertion time for Spruce: 1.07461s

Average deletion time for RadixGraph: 0.00200624s
Average deletion time for Spruce: 0.00313379s

Average update time for RadixGraph: 0.00238965s
Average update time for Spruce: 0.0118663s

Average get neighbours time for RadixGraph: 0.00259046s
Average get neighbours time for Spruce: 0.00441188s

Average BFS time for RadixGraph: 0.0183108s
Average BFS time for Spruce: 0.787641s
n = 100000, m = 10000000
Average insertion time for RadixGraph: 0.7155s
Average insertion time for Spruce: 1.23096s

Average deletion time for RadixGraph: 0.00136374s
Average deletion time for Spruce: 0.0124691s

Average update time for RadixGraph: 0.0014669s
Average update time for Spruce: 0.0202966s

Average get neighbours time for RadixGraph: 0.0142622s
Average get neighbours time for Spruce: 0.0122289s

Average BFS time for RadixGraph: 0.0630945s
Average BFS time for Spruce: 0.997874s
n = 1000000, m = 10000000
Average insertion time for RadixGraph: 0.979794s
Average insertion time for Spruce: 1.66883s

Average deletion time for RadixGraph: 0.00144994s
Average deletion time for Spruce: 0.0157172s

Average update time for RadixGraph: 0.00152506s
Average update time for Spruce: 0.000704472s

Average get neighbours time for RadixGraph: 0.106581s
Average get neighbours time for Spruce: 0.0436538s

Average BFS time for RadixGraph: 0.301307s
Average BFS time for Spruce: 1.4709s
```

# License
This project is licensed under the **Apache License 2.0** - see the [LICENSE](LICENSE) file for details.