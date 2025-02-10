# RadixGraph
A fast and space-efficient graph data structure.

# Compile and run
```sh
git clone https://github.com/ForwardStar/forward_star.git --recurse-submodules
cmake .
make
./forward_star
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
Average insertion time for forward star: 0.422699s
Average insertion time for spruce: 1.51126s

Average deletion time for forward star: 0.00203847s
Average deletion time for spruce: 0.0340424s

Average update time for forward star: 0.00570543s
Average update time for spruce: 0.116295s

Average get neighbours time for forward star: 0.0604059s
Average get neighbours time for spruce: 0.0153609s

Average BFS time for forward star: 0.574411s
Average BFS time for spruce: 1.53121s
n = 100000, m = 10000000
Average insertion time for forward star: 0.936805s
Average insertion time for spruce: 1.84409s

Average deletion time for forward star: 0.00497156s
Average deletion time for spruce: 0.0682484s

Average update time for forward star: 0.0197878s
Average update time for spruce: 0.139953s

Average get neighbours time for forward star: 0.0351155s
Average get neighbours time for spruce: 0.0339793s

Average BFS time for forward star: 0.542994s
Average BFS time for spruce: 1.88408s
n = 1000000, m = 10000000
Average insertion time for forward star: 2.02932s
Average insertion time for spruce: 2.75474s

Average deletion time for forward star: 0.00801279s
Average deletion time for spruce: 0.0532879s

Average update time for forward star: 0.0423716s
Average update time for spruce: 0.00622348s

Average get neighbours time for forward star: 0.124021s
Average get neighbours time for spruce: 0.0801724s

Average BFS time for forward star: 1.1841s
Average BFS time for spruce: 3.05221s
```