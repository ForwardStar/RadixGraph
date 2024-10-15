#include "headers.h"
#include "optimized_trie.h"

int u, d, n;

int main() {
    srand(time(0));
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
    for (int i = 0; i < n; i++) {
        uint64_t id = rand() % (1ull << u);
        trie_base.InsertVertex(id, new DummyNode());
        trie_opt.InsertVertex(id, new DummyNode());
    }
    std::cout << "Size of a baseline Trie: " << trie_base.size() << std::endl;
    std::cout << "Size of your Trie: " << trie_opt.size() << std::endl;
    return 0;
}