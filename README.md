# Forward_star
A graph data structure for optimizing graph traversals and graph OLAP tasks.

# Experimental results
```
n = 10, m = 256000
Average insertion time for adjacency linked list: 0.149987s
Average insertion time for chained forward star: 0.159111s
Average insertion time for adjacency array list: 0.121414s
Average insertion time for array forward star: 0.159733s
Average BFS time for adjacency linked list: 0.0743683s
Average BFS time for chained forward star: 0.0472459s
Average BFS time for adjacency array list: 0.0255649s
Average BFS time for array forward star: 0.00263147s
n = 100, m = 256000
Average insertion time for adjacency linked list: 0.379878s
Average insertion time for chained forward star: 0.403734s
Average insertion time for adjacency array list: 0.305074s
Average insertion time for array forward star: 0.402522s
Average BFS time for adjacency linked list: 0.122834s
Average BFS time for chained forward star: 0.0721248s
Average BFS time for adjacency array list: 0.0491853s
Average BFS time for array forward star: 0.00454665s
n = 1000, m = 256000
Average insertion time for adjacency linked list: 0.716622s
Average insertion time for chained forward star: 0.740041s
Average insertion time for adjacency array list: 0.564591s
Average insertion time for array forward star: 0.74707s
Average BFS time for adjacency linked list: 0.173269s
Average BFS time for chained forward star: 0.0999842s
Average BFS time for adjacency array list: 0.0740743s
Average BFS time for array forward star: 0.00708884s
n = 10000, m = 256000
Average insertion time for adjacency linked list: 1.1343s
Average insertion time for chained forward star: 1.16575s
Average insertion time for adjacency array list: 0.932031s
Average insertion time for array forward star: 1.23453s
Average BFS time for adjacency linked list: 0.241039s
Average BFS time for chained forward star: 0.126377s
Average BFS time for adjacency array list: 0.11245s
Average BFS time for array forward star: 0.0138833s
n = 100000, m = 256000
Average insertion time for adjacency linked list: 1.90688s
Average insertion time for chained forward star: 1.96455s
Average insertion time for adjacency array list: 1.67191s
Average insertion time for array forward star: 2.1206s
Average BFS time for adjacency linked list: 0.465351s
Average BFS time for chained forward star: 0.166148s
Average BFS time for adjacency array list: 0.323809s
Average BFS time for array forward star: 0.0550014s
```