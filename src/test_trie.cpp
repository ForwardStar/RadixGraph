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
    SORT trie_base(d, a_base);
    SORT trie_opt(d, a);
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
    double duration_trie_base_insert = 0;
    double duration_trie_base_query = 0;
    double duration_trie_opt_insert = 0;
    double duration_trie_opt_query = 0;
    // #pragma omp parallel for num_threads(10)
    for (int i = 0; i < n; i++) {
        uint64_t id = vids[i];
        auto start = std::chrono::high_resolution_clock::now();
        auto x = trie_base.RetrieveVertex(id, true);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        duration_trie_base_insert += duration.count();

        start = std::chrono::high_resolution_clock::now();
        x = trie_opt.RetrieveVertex(id, true);
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        duration_trie_opt_insert += duration.count();

        start = std::chrono::high_resolution_clock::now();
        auto tmp = trie_base.RetrieveVertex(id);
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        duration_trie_base_query += duration.count();
        assert(tmp->node == id);
        
        start = std::chrono::high_resolution_clock::now();
        tmp = trie_opt.RetrieveVertex(id);
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        duration_trie_opt_query += duration.count();
        assert(tmp->node == id);
    }
    std::cout << "Allocated space of a baseline Trie: " << trie_base.size() << std::endl;
    std::cout << "Allocated space of your Trie: " << trie_opt.size() << std::endl;
    std::cout << "Total insertion time of a baseline Trie: " << duration_trie_base_insert << std::endl;
    std::cout << "Total insertion time of your Trie: " << duration_trie_opt_insert << std::endl;
    std::cout << "Total query time of a baseline Trie: " << duration_trie_base_query << std::endl;
    std::cout << "Total query time of your Trie: " << duration_trie_opt_query << std::endl;
    return 0;
}