# Forward_star
A graph data structure for optimizing graph traversals and graph OLAP tasks.

# Compile and run
```sh
cmake .
make
./ForwardStar
```

# Experimental results
```
n = 10, m = 256000
Average insertion time for adjacency linked list: 0.150728s
Average insertion time for chained forward star: 0.202961s
Average insertion time for adjacency array list: 0.167815s
Average insertion time for array forward star: 0.209558s
Average BFS time for adjacency linked list: 0.065152s
Average BFS time for chained forward star: 0.0252291s
Average BFS time for adjacency array list: 0.0335006s
Average BFS time for array forward star: 0.00538497s
n = 100, m = 256000
Average insertion time for adjacency linked list: 0.371476s
Average insertion time for chained forward star: 0.495885s
Average insertion time for adjacency array list: 0.411538s
Average insertion time for array forward star: 0.527726s
Average BFS time for adjacency linked list: 0.156298s
Average BFS time for chained forward star: 0.0700884s
Average BFS time for adjacency array list: 0.0650835s
Average BFS time for array forward star: 0.00990759s
n = 1000, m = 256000
Average insertion time for adjacency linked list: 0.694241s
Average insertion time for chained forward star: 0.922737s
Average insertion time for adjacency array list: 0.787879s
Average insertion time for array forward star: 1.00848s
Average BFS time for adjacency linked list: 0.260785s
Average BFS time for chained forward star: 0.127034s
Average BFS time for adjacency array list: 0.0996583s
Average BFS time for array forward star: 0.0159844s
n = 10000, m = 256000
Average insertion time for adjacency linked list: 1.21322s
Average insertion time for chained forward star: 1.62503s
Average insertion time for adjacency array list: 1.44781s
Average insertion time for array forward star: 1.84906s
Average BFS time for adjacency linked list: 0.401929s
Average BFS time for chained forward star: 0.179642s
Average BFS time for adjacency array list: 0.176705s
Average BFS time for array forward star: 0.0346863s
n = 100000, m = 256000
Average insertion time for adjacency linked list: 2.43924s
Average insertion time for chained forward star: 3.1045s
Average insertion time for adjacency array list: 2.84662s
Average insertion time for array forward star: 3.56147s
Average BFS time for adjacency linked list: 0.795834s
Average BFS time for chained forward star: 0.259266s
Average BFS time for adjacency array list: 0.510769s
Average BFS time for array forward star: 0.11135s
```