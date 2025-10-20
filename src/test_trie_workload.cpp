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
#include <tbb/concurrent_unordered_set.h>

int main(int argc, char* argv[]) {
    int n, bit_length;
    std::cout << "The number of IDs to be inserted: ";
    std::cin >> n;
    std::cout << "The bit length of IDs: ";
    std::cin >> bit_length;
    std::cout << "Which workload to use? (u: uniform; s: slightly skewed; h: highly skewed) ";
    char workload_type;
    std::cin >> workload_type;
    std::default_random_engine generator(42);
    unsigned long long maximum = bit_length < 64 ? (1ull << bit_length) - 1 : -1;
    std::uniform_int_distribution distribution(0ull, maximum);
    std::vector<uint64_t> IDs;
    if (workload_type == 'u') {
        std::unordered_set<uint64_t> vertex_ids;
        while (IDs.size() < n) {
            uint64_t id = distribution(generator);
            if (vertex_ids.find(id) == vertex_ids.end()) {
                vertex_ids.insert(id);
                IDs.push_back(id);
            }
        }
    } else if (workload_type == 's') {
        maximum /= 1.5; // reduce range to introduce slight skewness
        std::unordered_set<uint64_t> vertex_ids;
        while (IDs.size() < n) {
            uint64_t id = distribution(generator);
            if (vertex_ids.find(id) == vertex_ids.end()) {
                vertex_ids.insert(id);
                IDs.push_back(id);
            }
        }
    } else if (workload_type == 'h') {
        // Generate skewed IDs using Reciprocal Distribution
        std::unordered_set<uint64_t> vertex_ids;
        std::uniform_real_distribution<double> real_distribution(0.000001, 1.0);
        while (IDs.size() < n) {
            double u = real_distribution(generator);
            double logx = u * log(static_cast<double>(maximum) + 1);
            uint64_t id = static_cast<uint64_t>(exp(logx)) - 1;
            if (vertex_ids.find(id) == vertex_ids.end()) {
                vertex_ids.insert(id);
                IDs.push_back(id);
            }
        }
    }
    SORT sort(n, bit_length, ceil(log2(bit_length)));
    // Generate vEB-tree setting
    std::vector<int> num_bits(ceil(log2(bit_length)));
    int now = bit_length, i = 0;
    while (now > 1) {
        int b = ceil(now / 2.0);
        num_bits[i++] = b;
        now -= b;
        if (i == num_bits.size()) num_bits[i - 1] += now;
    }
    SORT vEB(num_bits);
    // Insert IDs to SORT in original order
    // Print memory usage after 10% insertions
    std::string filename = "workload_";
    filename += workload_type;;
    filename += "_log.txt";
    std::ofstream f(filename);
    size_t total = n;
    int cnt = 0;
    for (size_t i = 0; i < total; ++i) {
        sort.RetrieveVertex(IDs[i], true);
        vEB.RetrieveVertex(IDs[i], true);
        if ((i + 1) % (total / 10) == 0) {
            std::cout << "SORT: memory after " << (i + 1) << " insertions is " << sort.Size() * 8 << " bytes" << std::endl;
            std::cout << "vEB: memory after " << (i + 1) << " insertions is " << vEB.Size() * 8 << " bytes" << std::endl;
            f << ++cnt << " " << sort.Size() * 8 << " " << vEB.Size() * 8 << std::endl;
        }
    }
    return 0;
}