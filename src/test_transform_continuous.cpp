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
#include "optimizer.h"

int main(int argc, char* argv[]) {
    std::cout << "Set n from 10^5 to 10^7 and log(u) = 32. Computing optimal parameters for l = 5 for all n..." << std::endl;
    std::vector<std::vector<int>> results_a;
    std::vector<int> results_n;
    // Compute using binary search
    for (int n = 100000; n <= 10000000; n++) {
        auto a = OptimizeParameters(n, 32, 5);
        int l = n, r = 10000000;
        while (l < r) {
            int mid = (l + r + 1) / 2;
            auto b = OptimizeParameters(mid, 32, 5);
            bool equal = true;
            for (int i = 0; i < 5; i++) {
                if (a[i] != b[i]) {
                    equal = false;
                    break;
                }
            }
            if (equal) {
                l = mid;
            } else {
                r = mid - 1;
            }
        }
        results_a.push_back(a);
        results_n.push_back(n);
        n = l;
    }
    std::cout << "Optimal parameters for different n:" << std::endl;
    for (int i = 0; i < results_a.size(); i++) {
        std::cout << "n = " << results_n[i] << ", a = ";
        for (auto x : results_a[i]) {
            std::cout << x << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "Generating 10^7 random vertex IDs..." << std::endl;
    std::default_random_engine generator(42);
    std::uniform_int_distribution distribution(0ull, (1ull << 32) - 1);
    std::unordered_set<uint64_t> vertex_ids;
    for (int i = 0; i < 10000000; i++) {
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
    std::cout << "Testing transformation..." << std::endl;
    SORT trie(5, results_a[0]);
    int now = 1;
    std::vector<uint64_t> current_vids;
    for (int i = 0; i < vids.size(); i++) {
        trie.RetrieveVertex(vids[i], true);
        current_vids.push_back(vids[i]);
        if (i == results_n[now]) {
            std::cout << "Transforming to n = " << results_n[now] << ", a = ";
            for (auto x : results_a[now]) {
                std::cout << x << " ";
            }
            std::cout << std::endl;
            auto start = std::chrono::high_resolution_clock::now();
            trie.Transform(5, results_a[now], current_vids);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff = end - start;
            std::cout << "Transformation takes " << diff.count() << " s" << std::endl;
            now++;
        }
    }
    return 0;
}