/*
 * Copyright (C) 2025 Haoxuan Xie
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef HEADERS
#define HEADERS

#include <atomic>
#include <iostream>
#include <cmath>
#include <cassert>
#include <cstring>
#include <random>
#include <chrono>
#include <fstream>
#include <sstream>
#include <stdint.h>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stack>
#include <thread>
#include <omp.h>
#include <tbb/concurrent_vector.h>

typedef uint32_t NodeID;

struct GlobalInfo {
    std::atomic<int> global_timestamp = 0;
    bool is_mixed_workloads = false;
    int cap_dummy_nodes = 50000000;
};

extern GlobalInfo global_info;

// Revised from GAPBS: https://github.com/sbeamer/gapbs
class AtomicBitmap {
 public:
  explicit AtomicBitmap(size_t size) {
    size_t num_words = (size + kBitsPerWord - 1) / kBitsPerWord;
    start_ = new std::atomic<uint8_t>[num_words];
    end_ = start_ + num_words;
  }

  ~AtomicBitmap() {
    delete [] start_;
  }

  void reset() {
    std::fill(start_, end_, 0);
  }

  void clear_bit(size_t pos) {
    start_[word_offset(pos)].fetch_and(~((uint8_t) 1l << bit_offset(pos)));
  }

  void set_bit(size_t pos) {
    start_[word_offset(pos)].fetch_or((uint8_t) 1l << bit_offset(pos));
  }

  void set_bit_atomic(size_t pos) {
    size_t offset_w = word_offset(pos), offset_b = bit_offset(pos);
    while (start_[offset_w].fetch_or((uint8_t) 1l << offset_b) & ((uint8_t) 1l << offset_b)) {}
  }

  bool get_bit(size_t pos) const {
    return (start_[word_offset(pos)] >> bit_offset(pos)) & 1l;
  }

 private:
  std::atomic<uint8_t> *start_ = nullptr;
  std::atomic<uint8_t> *end_ = nullptr;

  static const size_t kBitsPerWord = 8;
  static size_t word_offset(size_t n) { return n / kBitsPerWord; }
  static size_t bit_offset(size_t n) { return n & (kBitsPerWord - 1); }
};

/* WeightedEdge:
   - Represents a directed weighted edge;
   - e.weight: when it is 0, this edge is a delete log; otherwise it represents the weight of the edge;
   - e.idx: the offset (logical ID) of destination's vertex; vertex_table[e.idx] returns the DummyNode of the vertex.
*/
struct WeightedEdge {
    float weight = 0;
    int idx = -1; 
};

/* WeightedEdgeArray: multi-versioned edge array
    - size: the number of edges in this array;
    - cap: the capacity of this array;
    - physical_size: the number of written edges in this array (<= size);
    - edge: the edge array;
    - timestamp: the timestamp array, storing the timestamps of each edge;
    - prev_arr: the previous version of this edge array;
    - next_arr: the next version of this edge array (if any);
    - snapshot_timestamp: if this array is a snapshot, it stores the timestamp when the snapshot is created; otherwise it is 0;
    - snapshot_deg: if this array is a snapshot, it stores the degree when the snapshot is created; otherwise it is 0;
    - deg: the latest degree of this vertex;
    - threads_get_neighbor: how many threads are currently reading this edge array;
    - threads_analytical: how many analytical threads are currently reading this edge array (for analytical queries).
*/
class WeightedEdgeArray {
    public:
      std::atomic<int> size = 0, cap = 0, physical_size = 0; // Size and capacity of the array; physical size (<= size) is the number of written edges
      WeightedEdge* edge = nullptr;
      int* timestamp = nullptr; // Store edges and timestamp separately (but a better practice is to store them in a compact array)
      WeightedEdgeArray* prev_arr = nullptr;
      WeightedEdgeArray* next_arr = nullptr; // Snapshots are chained together
      int snapshot_timestamp = 0;
      std::atomic<int> snapshot_deg = 0, deg = 0; // The snapshot degree and latest degree
      std::atomic<int> threads_get_neighbor = 0, threads_analytical = 0; // How many threads reading this snapshot
      WeightedEdgeArray(int m) {
        edge = new WeightedEdge[m];
        cap = m;
      }
      ~WeightedEdgeArray() {
        if (edge) delete [] edge;
        if (timestamp) delete [] timestamp;
        if (next_arr) {
          next_arr->prev_arr = prev_arr;
        }
        if (prev_arr) {
          prev_arr->next_arr = next_arr;
        }
      }
};

/* Vertex:
   - Stores the information of a vertex;
   - N.node: the vertex ID of this DummyNode;
   - N.idx: the offset (logical ID) of this vertex;
   - N.del_time: the deletion time of this vertex;
   - N.next: the edge array pointer;
   - t_total, t_compact: timers for measuring the time (unit: s) of total operations and log compaction operations (only used for benchmarking and debugging);
   Note that we do not store ``Size`` since it can be retrieved by next->size; N.idx is stored for practical implementation but can be removed.
*/
struct Vertex {
    NodeID node = -1;
    int idx = -1, del_time = -1;
    std::atomic_flag mtx;
    std::atomic<WeightedEdgeArray*> next;
    #if DEBUG_MODE
      std::atomic<double> t_total, t_compact;
    #endif
    ~Vertex() {
      if (next.load()) delete next.load();
    }
};

struct SimpleVertex {
    NodeID node = -1;
    std::atomic<WeightedEdgeArray*> next;
};

#endif