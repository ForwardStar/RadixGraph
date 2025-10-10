# RadixGraph
A fast and space-efficient graph data structure. To store a graph in an in-memory system, two key components are designed: (1) a vertex index that allows fast retrieval of vertices; (2) an edge index that allows efficient updates and retrival of neighbor edges after retrieving the vertex. This is also referred as an adjacency-list-based structure.

The RadixGraph consists of:
- **SORT as its vertex index:** SORT is a **S**pace-**O**ptimized **R**adix **T**ree that guarantees expected-space optimality among all radix-tree structures that use a pointer array at each tree node for indexing children nodes.

- **Snapshot-log edge structure:** it uses a compact array for each vertex to store its neighbor edges to ensure sequential access; the array is split into two same-length segments: snapshot segment and log segment. Updates are firstly appended to the log segment and then materialized into the snapshot segment. By careful designs, the insert/update/delete complexity is amortized O(1) and the get-neighbor complexity is O(d) where d is degree of the vertex.

**Which graphs to support:** this version of RadixGraph supports weighted, directed, simple graphs, but the ``weight`` field can be generalized to any properties as long as a NULL value is reserved (for weights, this NULL value is 0). This means RadixGraph can also support labeled graph. RadixGraph can also be extended to support multi-graphs, but adaptation is needed. Refer to our paper for details.

# APIs
**Initialize SORT:** ``SORT* s = new SORT(d, a)``, where ``d`` is an int representing the number of layers, ``a`` is an array or a vector with length ``d`` representing the node in i-th layer has a fan-out of 2^a[i]. The expected-space-optimized setting can be computed via the optimizer and is included in the latter section of this README.

**Initialize RadixGraph:** ``RadixGraph* r = new RadixGraph(d, a)``, where ``d``, ``a`` follow the SORT settings. Upon initializing the RadixGraph, a corresponding SORT is initialized with its corresponding setting.

See APIs with comments from ``src/radixgraph.h`` and ``src/sort.h``. The ``Vertex`` and ``WeightedEdge`` classes are defined in ``src/utils.h``.

SORT API list:
- ``bool CheckExistence(NodeID id);``
- ``Vertex* RetrieveVertex(NodeID id, bool insert_mode=false);``
- ``bool DeleteVertex(NodeID id);``
- ``void Transform(int d, std::vector<int> _num_bits, std::vector<uint64_t>& vertex_set);``

RadixGraph API list:
- ``bool InsertEdge(NodeID src, NodeID des, float weight);``
- ``bool UpdateEdge(NodeID src, NodeID des, float weight);``
- ``bool DeleteEdge(NodeID src, NodeID des);``
- ``bool GetNeighbours(NodeID src, std::vector<WeightedEdge> &neighbours, bool is_snapshot=false, int timestamp=2147483647);``
- ``bool GetNeighbours(Vertex* src, std::vector<WeightedEdge> &neighbours, bool is_snapshot=false, int timestamp=2147483647);``
- ``bool GetNeighboursByOffset(int src, std::vector<WeightedEdge> &neighbours, bool is_snapshot=false, int timestamp=2147483647);``
- ``void CreateSnapshots(bool sort_neighbours=false, bool make_dense=false);``
- ``int GetGlobalTimestamp();``
- ``void SetNumThreads(int nth=64);``

To fully exploit the performance of RadixGraph and ensure correctness, do take care of following things that may affect the efficiency and space:
- **Concurrent reads and writes:** by default it is disabled. To support concurrent workloads of RadixGraph where reads and writes are executed concurrently, you can: (1) set ``global_info.is_mixed_workloads = true`` before initiating RadixGraph; (2) pause all current workloads, set ``global_info.is_mixed_workloads = true`` and execute ``CreateSnapshots()`` in RadixGraph, after which you can resume workloads. This will enable MVCC components, like creating timestamps and multi-versioned arrays. The cost is that the memory consumption will be slightly higher and the read operations are slightly slower to ensure consistency;
- **Transactional graph analytics:** RadixGraph currently cannot initiate a transaction by itself; to ensure consistent visible graph snapshot during graph analytics, before executing graph analytics, increase ``threads_analytical`` by 1 in the ``Vertex`` class for all vertices and record the current timestamp with ``GetGlobalTimestamp()``. Then run ``GetNeighbours()`` with this ``timestamp`` parameter during the graph analytics (by default, it will return the latest version of neighbor edges if not given a timestamp). When the graph analytics ends, decrease ``threads_analytical`` by 1 for all vertices.
- **Read-heavy workloads:** if the next workloads are read-heavy, it is suggested to run ``CreateSnapshot()`` of RadixGraph before executing the workload. This will merge all log segments into their snapshot segments and improve read efficiency. This process is exclusive, i.e., reads and writes should be paused during the process.
- **Maximum number of threads and accomadated vertices:** the default maximum number of threads is 64; to change this, use ``SetNumThreads()`` function or initialize the RadixGraph with: ``RadixGraph(int d, std::vector<int> _num_children, int _num_threads)`` with an appropriate ``_num_threads`` parameter. The ``SetNumThreads()`` function is also exclusive.

# Compile and run
We recommend using compiler ``GCC 11.4.0+``. You need to run RadixGraph on Linux platform with openMP and Intel Thread Building Block (TBB). For root users:
```sh
sudo apt-get install libtbb-dev
```

If prerequisites are completed, compile radixgraph by:
```sh
git clone https://github.com/ForwardStar/RadixGraph.git
cmake .
make
./radixgraph
```

For non-root users that can only install TBB locally, replace the cmake step with:
```sh
cmake . -DCMAKE_CXX_FLAGS="-I/path/to/tbb/include" -DCMAKE_EXE_LINKER_FLAGS="-L/path/to/tbb/lib"
```

Note that the executable ``test_radixgraph`` is a demo experiment. To integrate RadixGraph into your project, use the compiled library ``libRG.a``.

# Demo experiment
This demo (i.e., ``test_radixgraph`` executable file) randomly generates a graph of n vertices, m edges and the vertex ids are within [0, u-1]. It will output the efficiency of RadixGraph on this randomly generated graph. For full experiments, please refer to [gfe_driver_RadixGraph](https://github.com/ForwardStar/gfe_driver).

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

# Compute SORT configuration
The optimizer is compiled separately with the main RadixGraph components:
```sh
g++ optimizer.cpp -o optimizer -O3
```

Then run:
```sh
./optimizer
```

It requires you to input ``n`` (the number of vertices), ``log(u)`` (the range of IDs in [0, u-1]) and ``l`` (the number of layers of SORT). Then it will generate SORT configuration in a following format:
```
l'
a(0) a(1) ... a(l-1)
```

Note that ``l'`` may be less than ``l``, since ``ai=0`` layers are pruned. All experiments will absorb SORT configuration file as in the above format.

# Test Tries under different workloads
```sh
python3 workload_generator.py [n] [u]
```

This will randomly generate n IDs within [0, u] into ``workload.txt``.
```
./test_workload workload.txt <sort_setting_file> > results_<uniform/skewed>_<vEB/SORT>.txt
```

To plot memory footprints of different methods over different workloads:
```
python3 memory_footprint.py
```

# Debug mode of RadixGraph
To enable debug mode, set:
```cpp
#define DEBUG_MODE 1
```

in ``src/radixgraph.h`` and recompile. This mode is primary used for recording the overall operation time and log compaction time for each vertex. Each vertex will maintain a struct:
```cpp
struct DebugInfo {
    NodeID node = -1;
    int deg = 0;
    double t_total = 0, t_compact = 0;
};
```

You can retrieve the information for all vertices by:
```cpp
std::vector<DebugInfo> GetDebugInfo();
```

# Alternative vertex indexes
If you do not want to use SORT as the vertex index, you can use RadixGraph with the **adaptive radix tree (ART)** or **vertex array**.

ART is implemented by [unodb](https://github.com/laurynas-biveinis/unodb?tab=readme-ov-file). Clone the project into the folder of RadixGraph:
```sh
git clone https://github.com/laurynas-biveinis/unodb.git
cd unodb
git submodule update --init --recursive
cd ..
```

Then reconfigure RadixGraph with the CMake:
```sh
cmake . --fresh -DSTATS=OFF
```

and set in ``radixgraph.h``:
```cpp
#define USE_SORT 0
#define USE_ART 1
```

Recompile and RadixGraph automatically uses ART as its vertex index. Under this case, the RadixGraph initialization interface becomes ``RadixGraph(int _num_threads=64)`` and you do not need to pass SORT settings. Note: we do not suggest using ART, as it has its own thread management scheme (``Optimistic Lock Coupling (OLC)`` with ``Quiescent State Based Reclamation (QSBR)``), while RadixGraph is mainly designed for ``openMP`` and ``std::thread``. Although we currently did not find problems with the ART integration, we do not guarantee full concurrency correctness when using ART.

To use vertex array, set both ``USE_SORT`` and ``USE_ART`` as 0:
```cpp
#define USE_SORT 0
#define USE_ART 0
```

RadixGraph will use vertex array as the default vertex index. Under this case, the RadixGraph initialization interface becomes ``RadixGraph(int _num_threads=64, int _max_vertex_id=MAX_VERTEX_ID)``, which pre-allocates a vertex array of size ``_max_vertex_id+1``. The default ``MAX_VERTEX_ID`` is 50000000, you can change it in ``src/radixgraph.h``. Alternatively, reset the vertex array size using the RadixGraph API ``void SetMaximumID(int max_id=MAX_VERTEX_ID);``. Note that vertex array is only suitable for dense graphs with small ID ranges.

# License
This project is licensed under the **Apache License 2.0** - see the [LICENSE](LICENSE) file for details.