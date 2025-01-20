#ifndef HEADERS
#define HEADERS

#include <iostream>
#include <cmath>
#include <cassert>
#include <cstring>
#include <random>
#include <chrono>
#include <fstream>
#include <stdint.h>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stack>
#include <thread>
#include <omp.h>

const int max_number_of_threads = omp_get_num_threads();
const size_t bitmap_size = max_number_of_threads * 2;

#endif