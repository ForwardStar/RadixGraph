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
    std::cout << "Input a_before: ";
    std::vector<int> a_before;
    for (int i = 0; i < d; i++) {
        int ai;
        std::cin >> ai;
        a_before.push_back(ai);
    }
    std::cout << "Input a_after: ";
    std::vector<int> a_after;
    for (int i = 0; i < d; i++) {
        int ai;
        std::cin >> ai;
        a_after.push_back(ai);
    }
    SORT trie(d, a_before);
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
    double duration_insert = 0;
    double duration_transform = 0;
    for (int i = 0; i < n; i++) {
        uint64_t id = vids[i];

        auto start = std::chrono::high_resolution_clock::now();
        auto x = trie.RetrieveVertex(id, true);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        duration_insert += duration.count();
    }
    std::cout << "Size before: " << trie.size() << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    trie.Transform(d, a_after, vids);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    duration_transform += duration.count();
    std::cout << "Size after: " << trie.size() << std::endl;
    for (int i = 0; i < n; i++) {
        uint64_t id = vids[i];
        auto tmp = trie.RetrieveVertex(id);
        assert(tmp->node == id);
    }
    std::cout << "Insertion time: " << duration_insert << std::endl;
    std::cout << "Transformation time: " << duration_transform << std::endl;
    return 0;
}