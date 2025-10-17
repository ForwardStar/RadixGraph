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
#include "radixgraph.h"
#include "sort.h"
#if USE_ART
    #include "art.h"
#endif
#define NUM_THREADS 64

int main() {
    int n, bit_length;
    std::cout << "Enter number of vertices: ";
    std::cin >> n;
    std::cout << "Enter bit length of vertex IDs: ";
    std::cin >> bit_length;
    // Generate data
    std::default_random_engine generator(42);
    unsigned long long maximum = (1ull << bit_length) - 1;
    std::uniform_int_distribution distribution(0ull, maximum);
    std::unordered_set<uint64_t> vertex_set;
    std::vector<uint64_t> vertex_ids;
    for (int i = 0; i < n; i++) {
        uint64_t id = distribution(generator);
        while (vertex_set.find(id) != vertex_set.end()) {
            id = distribution(generator);
        }
        vertex_ids.push_back(id);
        vertex_set.insert(id);
    }

    unsigned int start_memory, end_memory;
    std::chrono::high_resolution_clock::time_point start, end;
    std::chrono::duration<double> duration;

    // Test ART
    #if USE_ART
        start_memory = get_proc_mem();
        start = std::chrono::high_resolution_clock::now();
        ART* art = new ART();
        unodb::this_thread().qsbr_pause();
        std::vector<unodb::qsbr_thread> threads(NUM_THREADS);
        for (int i = 0; i < NUM_THREADS; i++) {
            threads[i] = unodb::qsbr_thread([&, i]() {
                for (int j = i; j < vertex_ids.size(); j += NUM_THREADS) {
                    art->RetrieveVertex(vertex_ids[j], true);
                }
            });
        }
        for (int i = 0; i < NUM_THREADS; i++) {
            threads[i].join();
        }
        unodb::this_thread().qsbr_resume();
        unodb::this_thread().quiescent();
        unodb::this_thread().quiescent();
        end = std::chrono::high_resolution_clock::now();
        end_memory = get_proc_mem();
        std::cout << "Memory used by ART: " << (end_memory - start_memory) << " KB" << std::endl;
        duration = end - start;
        std::cout << "Time for ART insertion: " << duration.count() << "s" << std::endl;
        std::cout << "Throughput for ART insertion: " << n / duration.count() << " ops" << std::endl;

        // Test query
        start = std::chrono::high_resolution_clock::now();
        unodb::this_thread().qsbr_pause();
        threads.clear();
        threads.resize(NUM_THREADS);
        for (int i = 0; i < NUM_THREADS; i++) {
            threads[i] = unodb::qsbr_thread([&, i]() {
                for (int j = i; j < vertex_ids.size(); j += NUM_THREADS) {
                    art->RetrieveVertex(vertex_ids[j], false);
                }
            });
        }
        for (int i = 0; i < NUM_THREADS; i++) {
            threads[i].join();
        }
        unodb::this_thread().qsbr_resume();
        unodb::this_thread().quiescent();
        unodb::this_thread().quiescent();
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        std::cout << "Time for ART query: " << duration.count() << "s" << std::endl;
        std::cout << "Throughput for ART query: " << n / duration.count() << " ops" << std::endl;
        delete art;
    #endif

    // Test SORT
    SORT* sort = new SORT(n, bit_length, ceil(log2(bit_length)));
    start_memory = get_proc_mem();
    start = std::chrono::high_resolution_clock::now();
    #pragma omp parallel for num_threads(NUM_THREADS)
    for (auto id : vertex_ids) {
        sort->RetrieveVertex(id, true);
    }
    end = std::chrono::high_resolution_clock::now();
    end_memory = get_proc_mem();
    std::cout << "Memory used by SORT: " << (end_memory - start_memory) << " KB" << std::endl;
    duration = end - start;
    std::cout << "Time for SORT insertion: " << duration.count() << "s" << std::endl;
    std::cout << "Throughput for SORT insertion: " << n / duration.count() << " ops" << std::endl;

    // Test query
    start = std::chrono::high_resolution_clock::now();
    #pragma omp parallel for num_threads(NUM_THREADS)
    for (auto id : vertex_ids) {
        sort->RetrieveVertex(id, false);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "Time for SORT query: " << duration.count() << "s" << std::endl;
    std::cout << "Throughput for SORT query: " << n / duration.count() << " ops" << std::endl;
    delete sort;

    return 0;
}