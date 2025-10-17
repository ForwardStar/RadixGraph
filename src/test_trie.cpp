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
#include "utils.h"
#include "sort.h"

int u, d, n;

int main() {
    std::cout << "Input n: ";
    std::cin >> n;
    std::cout << "Input bit length of IDs: ";
    std::cin >> u;
    std::cout << "Input number of layers of your Trie: ";
    std::cin >> d;
    std::cout << "Input fanout array a: ";
    std::vector<int> a;
    for (int i = 0; i < d; i++) {
        int ai;
        std::cin >> ai;
        a.push_back(ai);
    }
    SORT trie(a);
    SORT sort(n, u, d);
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
    double duration_trie_insert = 0;
    double duration_trie_query = 0;
    double duration_sort_insert = 0;
    double duration_sort_query = 0;

    // Insert of Trie
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        uint64_t id = vids[i];
        std::chrono::high_resolution_clock::time_point start, end;
        std::chrono::duration<double> duration;
        start = std::chrono::high_resolution_clock::now();
        trie.InsertSimpleVertex(id);
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        duration_trie_insert += duration.count();
    }

    // Insert of SORT
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        uint64_t id = vids[i];
        std::chrono::high_resolution_clock::time_point start, end;
        std::chrono::duration<double> duration;
        start = std::chrono::high_resolution_clock::now();
        sort.InsertSimpleVertex(id);
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        duration_sort_insert += duration.count();
    }

    // Query of Trie
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        uint64_t id = vids[i];
        std::chrono::high_resolution_clock::time_point start, end;
        std::chrono::duration<double> duration;
        start = std::chrono::high_resolution_clock::now();
        auto tmp = trie.RetrieveVertex(id);
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        duration_trie_query += duration.count();
    }

    // Query of SORT
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        uint64_t id = vids[i];
        std::chrono::high_resolution_clock::time_point start, end;
        std::chrono::duration<double> duration;
        start = std::chrono::high_resolution_clock::now();
        auto tmp = sort.RetrieveVertex(id);
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        duration_sort_query += duration.count();
    }

    std::cout << "Allocated space of your Trie: " << trie.Size() * 8 << " bytes" << std::endl;
    std::cout << "Insertion throughput of your Trie: " << n / duration_trie_insert << " ops" << std::endl;
    std::cout << "Query throughput of your Trie: " << n / duration_trie_query << " ops" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Allocated space of SORT: " << sort.Size() * 8 << " bytes" << std::endl;
    std::cout << "Insertion throughput of SORT: " << n / duration_sort_insert << " ops" << std::endl;
    std::cout << "Query throughput of SORT: " << n / duration_sort_query << " ops" << std::endl;

    return 0;
}