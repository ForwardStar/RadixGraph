#ifndef TRIE
#define TRIE

#include "headers.h"

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
    double weight; // edge property
    _dummy_node* forward; // forward pointer to dummy node of target vertex
    int timestamp;
    uint8_t type;
} WeightedEdge;

typedef struct _dummy_node {
    uint64_t node = -1; // this dummy node corresponds to which vertex
    int idx = -1; // the index of this dummy node
    AtomicBitmap* flag = nullptr;
    WeightedEdge* next = nullptr;
    int cap = 0;
    std::atomic<int> cnt = 0, deg = 0;
    std::atomic<uint8_t> mtx = 0;

    ~_dummy_node() {
        if (next) delete [] next;
        if (flag) delete flag;
    }
} DummyNode;

class Trie {
    public:
        typedef struct _trie_node {
            DummyNode** head = nullptr;
            _trie_node** children = nullptr;
            AtomicBitmap* mtx = nullptr;
            int sz = 0;

            ~_trie_node() {
                if (children) {
                    for (int i = 0; i < sz; i++) {
                        if (children[i]) {
                            delete children[i];
                        }
                    }
                    delete [] children;
                }
                if (head) {
                    for (int i = 0; i < sz; i++) {
                      if (head[i]) {
                        delete head[i];
                      }
                    }
                    delete [] head;
                }
                if (mtx) {
                    delete mtx;
                }
            }
        } TrieNode;

        TrieNode* root = nullptr;
        DummyNode** dummy_nodes;
        std::vector<int> num_bits, sum_bits;
        int depth = 0, space = 0, cap = 0;
        std::atomic<int> cnt = 0;
        std::atomic<uint8_t> mtx = 0;

        inline DummyNode* InsertVertex(TrieNode* current, uint64_t id, int d);

        DummyNode* RetrieveVertex(uint64_t id, bool insert_mode=false);

        bool DeleteVertex(uint64_t id);

        long long size();

        Trie() {}
        Trie(int d, int _num_bits[]);
        Trie(int d, std::vector<int> _num_bits);
        ~Trie();
};

#endif