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
#ifndef TRIE
#define TRIE

#include "headers.h"
#define CAP_DUMMY_NODES 50000000

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

typedef struct _weighted_edge;
typedef struct _dummy_node;

typedef struct _weighted_edge {
    float weight = 0; // edge property
    int idx = -1; // the index of destination's dummy node
} WeightedEdge;

typedef struct _dummy_node {
    NodeID node = -1; // this dummy node corresponds to which vertex
    int idx = -1; // the index of this dummy node
    WeightedEdge* next = nullptr;
    int cap = 0;
    std::atomic<int> cnt, deg;
    std::atomic_flag mtx = ATOMIC_FLAG_INIT;

    ~_dummy_node() {
        if (next) free(next);
    }
} DummyNode;

class Trie {
    public:
        typedef struct _trie_node {
            uint64_t* children = nullptr;
            AtomicBitmap* mtx = nullptr;

            ~_trie_node() {
              if (mtx) delete mtx;
              if (children) delete [] children;
            }
        } TrieNode;

        TrieNode* root = nullptr;
        DummyNode** dummy_nodes = nullptr;
        std::vector<int> num_bits, sum_bits;
        int depth = 0, space = 0;
        std::atomic<int> cnt;
        bool enable_query = true;

        inline DummyNode* InsertVertex(TrieNode* current, NodeID id, int d);

        DummyNode* RetrieveVertex(NodeID id, bool insert_mode=false);

        bool DeleteVertex(NodeID id);

        long long size();

        Trie() {}
        Trie(int d, int _num_bits[], bool _enable_query=true);
        Trie(int d, std::vector<int> _num_bits, bool _enable_query=true);
        ~Trie();
};

#endif