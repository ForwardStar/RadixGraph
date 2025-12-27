# RadixGraph
RadixGraph is used for **storing and updating weighted graphs in the main memory**.

Key features: RadixGraph is built upon a novel **Space-Optimized Radix Tree (SORT)** as the vertex index to insert or query vertices. For edge storage, RadixGraph supports **amortized O(1) time for dynamic graph updates** (insertion, deletion and update of edges).

You can incorporate **RadixGraph** to any C++ project requiring dynamic graph storage and updates. Either single-threaded, multi-threaded, or concurrent read-write workloads, RadixGraph can efficiently handle them. We also encourage users to incorporate **SORT** into their projects for general applications (e.g., key-value stores, document indexing).

# Compile
We recommend using compiler ``GCC 11.4.0+``. You need to compile RadixGraph on Linux platform with OpenMP and Intel Thread Building Block (TBB).

If prerequisites are completed, compile radixgraph by:
```sh
git clone https://github.com/ForwardStar/RadixGraph.git
mkdir build && cd build
cmake ..
make -j
```

This will generate ``libRG.a`` and some test modules in the ``build`` folder.

**Integrate RadixGraph into your project:** (1) include the header file ``src/radixgraph.h`` in your source files; (2) link with the static library ``libRG.a`` when compiling your project. Example:
```sh
g++ your_source.cpp -o your_program -I/path_to_radixgraph/src -L/path_to_radixgraph/build -lRG -fopenmp -ltbb -std=c++17 -O3
```

# APIs
See example usages in the ``examples`` directory. See full APIs with comments from ``src/radixgraph.h``. The ``Vertex`` and ``WeightedEdge`` classes are defined in ``src/utils.h``.

RadixGraph API list:
- ``bool InsertEdge(NodeID src, NodeID des, float weight);``
- ``bool UpdateEdge(NodeID src, NodeID des, float weight);``
- ``bool DeleteEdge(NodeID src, NodeID des);``
- ``bool GetNeighbours(NodeID src, std::vector<WeightedEdge> &neighbours, int timestamp=2147483647);``
- ``bool GetNeighbours(Vertex* src, std::vector<WeightedEdge> &neighbours, int timestamp=2147483647);``
- ``bool GetNeighboursByOffset(int src, std::vector<WeightedEdge> &neighbours, int timestamp=2147483647);``
- ``void CreateSnapshots(bool sort_neighbours=false, bool make_dense=false);``
- ``int GetGlobalTimestamp();``
- ``void SetNumThreads(int nth=64);``
- ``int GetInternalID(NodeID id);``
- ``NodeID GetExternalID(int offset);``
- ``NodeID GetExternalID(WeightedEdge& e);``
- ``int GetNumVertices();``
- ``void SetExpandRate(double ratio);``

To fully exploit the performance of RadixGraph and ensure correctness, do take care of following things that may affect the efficiency and space:
- **Concurrent reads and writes:** by default it is disabled. To support concurrent workloads of RadixGraph where reads and writes are executed concurrently, you can: (1) set ``global_info.is_mixed_workloads = true`` before initiating RadixGraph; (2) pause all current workloads, set ``global_info.is_mixed_workloads = true`` and execute ``CreateSnapshots()`` in RadixGraph, after which you can resume workloads. This will enable MVCC components, like creating timestamps and multi-versioned arrays. The cost is that the memory consumption will be slightly higher and the read operations are slightly slower to ensure consistency.
- **Read-heavy workloads:** if the next workloads are read-heavy, it is suggested to run ``CreateSnapshot()`` of RadixGraph before executing the workload. This will merge all log segments into their snapshot segments and improve read efficiency. This process is exclusive, i.e., reads and writes should be paused during the process.
- **Maximum number of threads:** the default maximum number of threads is 64; to change this, use ``SetNumThreads()`` function or initialize the RadixGraph with an appropriate ``_num_threads`` parameter. The ``SetNumThreads()`` function is also exclusive.

# Test modules
Test modules contain ``test_radixgraph``, ``test_gapbs``, ``test_analytics``, ``test_batch_updates``, ``test_trie``, ``test_trie_workload``, ``test_transform_continuous``, and ``test_vertex_index``. They are mainly used for testing correctness and case studies for RadixGraph and SORT. For comparison with other systems and experiments on real-world graphs, please refer to [GFE Driver for RadixGraph](https://github.com/ForwardStar/gfe_driver).

``test_radixgraph`` randomly generates a graph of n vertices, m edges and the vertex ids are within [0, u-1]. It will output the efficiency of RadixGraph on this randomly generated graph. You can also test RadixGraph with your dataset by ``./test_radixgraph <edge_file>``, where ``edge_file`` is a text file containing edges in the format of ``src des weight`` in each line.

``test_gapbs`` randomly generates a graph and tests RadixGraph with graph algorithms in [GAP Benchmark Suite](https://github.com/sbeamer/gapbs). Similarly, you can also test RadixGraph with your dataset by ``./test_gapbs <edge_file>``.

``test_analytics`` is a more comprehensive test module for graph analytics, including both GAPBS algorithms and multi-hop queries. You can test RadixGraph with your dataset by ``./test_analytics <edge_file>``.

``test_batch_updates`` reads in a data file by ``./test_batch_updates <edge_file>`` and inserts edges into RadixGraph to construct the graph. Then it performs updates with different batch sizes to evaluate the efficiencies for different batches.

``test_trie`` randomly generates n vertex ids and inserts them into your customized Trie. It will output the efficiency and space consumption of your Trie on this vertex set and compare it with SORT.

``test_trie_workload`` evaluates memory footprints of SORT and vEB tree under different workloads. It will also output the log file ``workload_<workload_type>_log.txt`` that records the memory footprints during the insertion process. You can plot the footprints by running ``python3 plot_workload_log.py``. Figures will be saved in ``figures/workload_log.pdf``.

``test_transform_continuous`` randomly generates 10^7 vertex ids within [0, 2^32-1] and tests time costs of transforming SORT when inserting these 10^7 vertex ids. It will also record the memory footprint of SORT during the insertion and transformation process into ``memory_log.txt``. You can plot the footprint by running ``python3 plot_memory_log.py``. Figures will be saved in ``figures/memory_log.pdf``.

``test_vertex_index`` randomly generates n vertex ids and inserts them into SORT and ART. It will output the efficiency and space consumption of different vertex indexes.

# Alternative vertex indexes
If you do not want to use SORT as the vertex index, you can use RadixGraph with the **adaptive radix tree (ART)** or **vertex array**.

ART is implemented by [unodb](https://github.com/laurynas-biveinis/unodb?tab=readme-ov-file). Clone the project into the folder of RadixGraph:
```sh
git clone https://github.com/laurynas-biveinis/unodb.git
cd unodb
git submodule update --init --recursive
cd ..
```

Then reconfigure RadixGraph with CMake:
```sh
rm -rf build && mkdir build && cd build
cmake .. -DSTATS=OFF
```

and set in ``radixgraph.h``:
```cpp
#define USE_SORT 0
#define USE_ART 1
```

Recompile and RadixGraph automatically uses ART as its vertex index. Note: ART has its own thread management system through ``unodb::qsbr_thread``. It is user's responsibility to create and manage threads when using ART as the vertex index in RadixGraph.

To use vertex array, set both ``USE_SORT`` and ``USE_ART`` as 0:
```cpp
#define USE_SORT 0
#define USE_ART 0
```

RadixGraph will use vertex array as the default vertex index.

Note that the APIs for initializing RadixGraph is different between SORT, ART and vertex array. For SORT, you need to provide the number of vertices to be stored and bit-length of these vertex IDs. For ART, no additional information is required. For vertex array, there is a ``_max_vertex_id=MAX_VERTEX_ID)`` parameter upon initialization and RadixGraph pre-allocates a vertex array of size ``_max_vertex_id+1``. The default ``MAX_VERTEX_ID`` is 50000000, you can change it in ``src/radixgraph.h``. Alternatively, reset the vertex array size using the RadixGraph API ``void SetMaximumID(int max_id=MAX_VERTEX_ID);``. Note that vertex array is only suitable for dense graphs with small ID ranges.

# SORT related
SORT (Space-Optimized Radix Tree) is a space-optimized radix tree which serves as the vertex index in RadixGraph. The optimal radix tree structure is found via dynamic programming. If you are interested in SORT, refer to ``src/sort.h`` for full APIs. You are welcome to integrate the SORT data structure into your own project, and we see great potentials of SORT in other applications (e.g., key-value stores, document indexing, etc.).

A standalone dynamic programming optimizer is also included in ``optimizer.cpp``. You can compile it by:
```sh
g++ optimizer.cpp -o optimizer -std=c++11 -O3
```

Then run ``./optimizer -h`` for help information.

# Reproducing full experiments in our paper
We provide the complete reproducing procedures in [``reproduce_radixgraph.ipynb``](https://entuedu-my.sharepoint.com/:u:/g/personal/haoxuan001_e_ntu_edu_sg/IQB0_JARj8sEQYqKdgL0Os_TAUTWogAsIKcFFLFpWynFSv0?e=edndcX). Use jupyter notebook to open the file and run.

# License
This project is licensed under the **Apache License 2.0** - see the [LICENSE](LICENSE) file for details.
