# Forward_star
A graph data structure for optimizing graph traversals and graph OLAP tasks.

# Compile and run
```sh
git clone https://github.com/ForwardStar/forward_star.git --recurse-submodules
cmake .
make
./ForwardStar
```

# Experimental results (single-thread)
```
n = 10, m = 256000
Average insertion time for adjacency linked list: 0.114999s
Average insertion time for chained forward star: 0.147755s
Average insertion time for adjacency array list: 0.119622s
Average insertion time for array forward star: 0.149946s
Average insertion time for spruce: 0.596813s

Average deletion time for adjacency linked list: 0.000402068s
Average deletion time for chained forward star: 0.000563624s
Average deletion time for adjacency array list: 0.00614657s
Average deletion time for array forward star: 0.000588839s
Average deletion time for spruce: 0.000350364s

Average update time for adjacency linked list: 0.000425071s
Average update time for chained forward star: 0.000552997s
Average update time for adjacency array list: 0.000510506s
Average update time for array forward star: 0.000577858s
Average update time for spruce: 0.000409034s

Average get neighbours time for adjacency linked list: 0.00838923s
Average get neighbours time for chained forward star: 0.0125175s
Average get neighbours time for adjacency array list: 0.00563809s
Average get neighbours time for array forward star: 0.00505537s
Average get neighbours time for spruce: 0.00211525s

Average BFS time for adjacency linked list: 0.0333132s
Average BFS time for chained forward star: 0.0122734s
Average BFS time for adjacency array list: 0.0333432s
Average BFS time for array forward star: 0.00537802s
Average BFS time for spruce: 0.0247245s
n = 100, m = 256000
Average insertion time for adjacency linked list: 0.283897s
Average insertion time for chained forward star: 0.363017s
Average insertion time for adjacency array list: 0.289328s
Average insertion time for array forward star: 0.375463s
Average insertion time for spruce: 0.732038s

Average deletion time for adjacency linked list: 0.00269788s
Average deletion time for chained forward star: 0.001411s
Average deletion time for adjacency array list: 0.0102338s
Average deletion time for array forward star: 0.00150835s
Average deletion time for spruce: 0.000876789s

Average update time for adjacency linked list: 0.002565s
Average update time for chained forward star: 0.00140751s
Average update time for adjacency array list: 0.00194619s
Average update time for array forward star: 0.00147625s
Average update time for spruce: 0.000915881s

Average get neighbours time for adjacency linked list: 0.024211s
Average get neighbours time for chained forward star: 0.028079s
Average get neighbours time for adjacency array list: 0.0110484s
Average get neighbours time for array forward star: 0.0105663s
Average get neighbours time for spruce: 0.00431838s

Average BFS time for adjacency linked list: 0.0725356s
Average BFS time for chained forward star: 0.0271133s
Average BFS time for adjacency array list: 0.07396s
Average BFS time for array forward star: 0.0109942s
Average BFS time for spruce: 0.0505071s
n = 1000, m = 256000
Average insertion time for adjacency linked list: 0.528162s
Average insertion time for chained forward star: 0.685051s
Average insertion time for adjacency array list: 0.537681s
Average insertion time for array forward star: 0.712616s
Average insertion time for spruce: 0.818061s

Average deletion time for adjacency linked list: 0.0212695s
Average deletion time for chained forward star: 0.00274128s
Average deletion time for adjacency array list: 0.0256439s
Average deletion time for array forward star: 0.00308785s
Average deletion time for spruce: 0.00172217s

Average update time for adjacency linked list: 0.0173372s
Average update time for chained forward star: 0.00275709s
Average update time for adjacency array list: 0.00992657s
Average update time for array forward star: 0.00332913s
Average update time for spruce: 0.00206154s

Average get neighbours time for adjacency linked list: 0.0448325s
Average get neighbours time for chained forward star: 0.0586201s
Average get neighbours time for adjacency array list: 0.0192141s
Average get neighbours time for array forward star: 0.026093s
Average get neighbours time for spruce: 0.00945065s

Average BFS time for adjacency linked list: 0.127178s
Average BFS time for chained forward star: 0.0588772s
Average BFS time for adjacency array list: 0.130192s
Average BFS time for array forward star: 0.0253291s
Average BFS time for spruce: 0.0789375s
n = 10000, m = 256000
Average insertion time for adjacency linked list: 0.827211s
Average insertion time for chained forward star: 1.06825s
Average insertion time for adjacency array list: 0.858128s
Average insertion time for array forward star: 1.13041s
Average insertion time for spruce: 0.860616s

Average deletion time for adjacency linked list: 0.0253894s
Average deletion time for chained forward star: 0.00494613s
Average deletion time for adjacency array list: 0.0287366s
Average deletion time for array forward star: 0.00520691s
Average deletion time for spruce: 0.00207389s

Average update time for adjacency linked list: 0.0205216s
Average update time for chained forward star: 0.00471742s
Average update time for adjacency array list: 0.0119217s
Average update time for array forward star: 0.00536799s
Average update time for spruce: 0.00248333s

Average get neighbours time for adjacency linked list: 0.0773957s
Average get neighbours time for chained forward star: 0.095906s
Average get neighbours time for adjacency array list: 0.040721s
Average get neighbours time for array forward star: 0.064535s
Average get neighbours time for spruce: 0.0307969s

Average BFS time for adjacency linked list: 0.194083s
Average BFS time for chained forward star: 0.0935955s
Average BFS time for adjacency array list: 0.198383s
Average BFS time for array forward star: 0.0479246s
Average BFS time for spruce: 0.127273s
n = 100000, m = 256000
Average insertion time for adjacency linked list: 1.33911s
Average insertion time for chained forward star: 1.73262s
Average insertion time for adjacency array list: 1.47358s
Average insertion time for array forward star: 1.87648s
Average insertion time for spruce: 0.922452s

Average deletion time for adjacency linked list: 0.0273099s
Average deletion time for chained forward star: 0.00803699s
Average deletion time for adjacency array list: 0.0305814s
Average deletion time for array forward star: 0.00853804s
Average deletion time for spruce: 0.0023057s

Average update time for adjacency linked list: 0.0223523s
Average update time for chained forward star: 0.00770756s
Average update time for adjacency array list: 0.0136731s
Average update time for array forward star: 0.00855675s
Average update time for spruce: 0.00282116s

Average get neighbours time for adjacency linked list: 0.217554s
Average get neighbours time for chained forward star: 0.250591s
Average get neighbours time for adjacency array list: 0.173346s
Average get neighbours time for array forward star: 0.225947s
Average get neighbours time for spruce: 0.0975422s

Average BFS time for adjacency linked list: 0.375096s
Average BFS time for chained forward star: 0.149857s
Average BFS time for adjacency array list: 0.383148s
Average BFS time for array forward star: 0.106319s
Average BFS time for spruce: 0.229358s
```