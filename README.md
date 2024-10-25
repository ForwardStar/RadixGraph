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
Average insertion time for adjacency array list: 2.84195s
Average insertion time for forward star: 0.703207s
Average insertion time for spruce: 1.69768s

Average deletion time for adjacency array list: 0.0394939s
Average deletion time for forward star: 0.000971971s
Average deletion time for spruce: 0.00169923s

Average update time for adjacency array list: 0.0236764s
Average update time for forward star: 0.00099087s
Average update time for spruce: 0.00191415s

Average get neighbours time for adjacency array list: 0.065714s
Average get neighbours time for forward star: 0.115408s
Average get neighbours time for spruce: 0.0262438s

Average BFS time for adjacency array list: 0.621542s
Average BFS time for forward star: 0.120326s
Average BFS time for spruce: 0.355132s
n = 10000, m = 2560000
Average insertion time for adjacency array list: 3.67139s
Average insertion time for forward star: 1.30777s
Average insertion time for spruce: 1.39898s

Average deletion time for adjacency array list: 0.0103011s
Average deletion time for forward star: 0.001487s
Average deletion time for spruce: 0.00195205s

Average update time for adjacency array list: 0.00634457s
Average update time for forward star: 0.00146444s
Average update time for spruce: 0.0019218s

Average get neighbours time for adjacency array list: 0.0930062s
Average get neighbours time for forward star: 0.142278s
Average get neighbours time for spruce: 0.0597064s

Average BFS time for adjacency array list: 0.699886s
Average BFS time for forward star: 0.158753s
Average BFS time for spruce: 0.433621s
n = 100000, m = 2560000
Average insertion time for adjacency array list: 6.21797s
Average insertion time for forward star: 2.96867s
Average insertion time for spruce: 1.88376s

Average deletion time for adjacency array list: 0.00359288s
Average deletion time for forward star: 0.00158721s
Average deletion time for spruce: 0.00146533s

Average update time for adjacency array list: 0.00294137s
Average update time for forward star: 0.00159786s
Average update time for spruce: 0.000742626s

Average get neighbours time for adjacency array list: 0.323728s
Average get neighbours time for forward star: 0.300988s
Average get neighbours time for spruce: 0.27955s

Average BFS time for adjacency array list: 1.14734s
Average BFS time for forward star: 0.301351s
Average BFS time for spruce: 0.796216s
n = 1000000, m = 2560000
Average insertion time for adjacency array list: 10.3882s
Average insertion time for forward star: 4.03073s
Average insertion time for spruce: 2.20813s

Average deletion time for adjacency array list: 0.00340926s
Average deletion time for forward star: 0.00195247s
Average deletion time for spruce: 0.00143072s

Average update time for adjacency array list: 0.00336005s
Average update time for forward star: 0.00204922s
Average update time for spruce: 0.000505456s

Average get neighbours time for adjacency array list: 2.60527s
Average get neighbours time for forward star: 1.41841s
Average get neighbours time for spruce: 0.930707s

Average BFS time for adjacency array list: 4.71818s
Average BFS time for forward star: 1.15693s
Average BFS time for spruce: 2.68607s
```