#include "headers.h"
#include "optimized_trie.h"

int u, d, n;

int main() {
    std::cout << "Input n: ";
    std::cin >> n;
    std::cout << "Input log(u): ";
    std::cin >> u;
    std::cout << "Input d: ";
    std::cin >> d;
    std::vector<int> a_base;
    a_base.assign(d, ceil(double(u) / d));
    std::cout << "Input a: ";
    std::vector<int> a;
    for (int i = 0; i < d; i++) {
        int ai;
        std::cin >> ai;
        a.push_back(ai);
    }
    Trie trie_base = Trie(d, a_base);
    Trie trie_opt = Trie(d, a);
    std::default_random_engine generator;
    unsigned long long maximum = u < 64 ? (1ull << u) - 1 : -1;
    std::uniform_int_distribution distribution(0ull, maximum);
    std::unordered_set<uint64_t> vertex_ids;
    for (int i = 0; i < n; i++) {
        uint64_t id = distribution(generator);
        while (vertex_ids.find(id) != vertex_ids.end()) {
            id = distribution(generator);
        }
        vertex_ids.insert(id);
    }
    std::vector<uint64_t> vids;
    for (auto u : vertex_ids) {
        vids.emplace_back(u);
    }
    #pragma omp parallel for num_threads(10)
    for (int i = 0; i < n; i++) {
        uint64_t id = vids[i];
        auto x = trie_base.RetrieveVertex(id, true);
        x = trie_opt.RetrieveVertex(id, true);
        auto tmp = trie_base.RetrieveVertex(id);
        assert(tmp->node == id);
        tmp = trie_opt.RetrieveVertex(id);
        assert(tmp->node == id);
    }
    std::cout << "Allocated space of a baseline Trie: " << trie_base.size() << std::endl;
    std::cout << "Allocated space of your Trie: " << trie_opt.size() << std::endl;
    return 0;
}