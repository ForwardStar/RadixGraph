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
        // Generate skewed IDs using Zipf distribution
        // Generate in parallel
        std::cout << "Generating skewed IDs using Zipf distribution...\n";
        std::cout << "This may take a while for large n... Generating in parallel...\n";
        double s = 1.0; // skewness parameter
        std::vector<double> harmonic_series(n + 1, 0.0);
        for (int i = 1; i <= n; i++) {
            harmonic_series[i] = harmonic_series[i - 1] + 1.0 / std::pow(i, s);
        }
        tbb::concurrent_unordered_set<uint64_t> vertex_ids;
        #pragma omp parallel
        {
            if (omp_get_thread_num() == 0) {
                int tm = 0;
                while (vertex_ids.size() < n) {
                    std::cout << "Generated: " << vertex_ids.size() << " / " << n << ", time count: " << tm++ << "s" << std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    #pragma omp flush
                }
                std::cout << "Generation complete!" << std::endl;
            }
            while (vertex_ids.size() < n) {
                double z = ((double) rand() / RAND_MAX) * harmonic_series[n];
                int rank = 1;
                while (rank <= n && harmonic_series[rank] < z) {
                    rank++;
                }
                uint64_t id = rank - 1;
                if (vertex_ids.find(id) == vertex_ids.end()) {
                    vertex_ids.insert(id);
                }
            }
        }
        IDs.assign(vertex_ids.begin(), vertex_ids.end());
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