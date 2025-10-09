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
#include <unistd.h>
#include "radixgraph.h"
#include "optimized_trie.h"
#if USE_ART
    #include "art.h"
#endif
#include <tbb/concurrent_hash_map.h>
#define NUM_THREADS 64

// From Spruce (https://github.com/Stardust-SJF/Spruce/blob/main/src/memory_analysis.cpp)
#define VMRSS_LINE 22 // Different OSs have different values
unsigned int get_proc_mem() {
    int pid = getpid();
    char file_name[64] = { 0 };
    FILE* fd;
    char line_buff[512] = { 0 };
    sprintf(file_name, "/proc/%d/status", pid);
    fd = fopen(file_name, "r");
    if (fd == NULL) return 0;
    char name[64];
    int vmrss = 0;
    int i;
    for (i = 0; i < VMRSS_LINE - 1; i++) {
        fgets(line_buff, sizeof(line_buff), fd);
    }
    fgets(line_buff, sizeof(line_buff), fd);
    sscanf(line_buff, "%s %d", name, &vmrss);
    fclose(fd);
    return vmrss;
}

int main() {
    int n, logu;
    std::cout << "Enter number of vertices: ";
    std::cin >> n;
    std::cout << "Enter log2 of integer universe size: ";
    std::cin >> logu;
    // Read settings from file
    std::ifstream fin("settings");
    int d;
    fin >> d;
    std::vector<int> a(d);
    for (auto& i : a) fin >> i;
    // Generate data
    std::default_random_engine generator(42);
    unsigned long long maximum = (1ull << logu) - 1;
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

    // Test SORT
    auto start_memory = get_proc_mem();
    std::chrono::high_resolution_clock::time_point start, end;
    start = std::chrono::high_resolution_clock::now();
    SORT* sort = new SORT(d, a);
    #pragma omp parallel for num_threads(NUM_THREADS)
    for (auto id : vertex_ids) {
        sort->InsertSimpleVertex(id);
    }
    end = std::chrono::high_resolution_clock::now();
    auto end_memory = get_proc_mem();
    std::cout << "Memory used by SORT: " << (end_memory - start_memory) << " KB" << std::endl;
    std::chrono::duration<double> duration = end - start;
    std::cout << "Time for SORT insertion: " << duration.count() << "s" << std::endl;
    delete sort;

    // Test concurrent hash map
    start_memory = get_proc_mem();
    start = std::chrono::high_resolution_clock::now();
    tbb::concurrent_hash_map<uint64_t, int> cmap;
    #pragma omp parallel for num_threads(NUM_THREADS)
    for (auto id : vertex_ids) {
        tbb::concurrent_hash_map<uint64_t, int>::accessor a;
        cmap.insert(a, id);
        a->second = 1;
    }
    end = std::chrono::high_resolution_clock::now();
    end_memory = get_proc_mem();
    std::cout << "Memory used by concurrent hash map: " << (end_memory - start_memory) << " KB" << std::endl;
    duration = end - start;
    std::cout << "Time for concurrent hash map insertion: " << duration.count() << "s" << std::endl;

    // Test ART
    #if USE_ART
        start_memory = get_proc_mem();
        start = std::chrono::high_resolution_clock::now();
        ART* art = new ART();
        #pragma omp parallel for num_threads(NUM_THREADS)
        for (auto id : vertex_ids) {
            art->InsertSimpleVertex(id);
        }
        end = std::chrono::high_resolution_clock::now();
        end_memory = get_proc_mem();
        std::cout << "Memory used by ART: " << (end_memory - start_memory) << " KB" << std::endl;
        duration = end - start;
        std::cout << "Time for ART insertion: " << duration.count() << "s" << std::endl;
        delete art;
    #endif
    return 0;
}