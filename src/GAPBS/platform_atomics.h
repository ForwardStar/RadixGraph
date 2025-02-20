// Copyright (c) 2015, The Regents of the University of California (Regents)
// See LICENSE.txt for license details

#ifndef PLATFORM_ATOMICS_H_
#define PLATFORM_ATOMICS_H_


/*
GAP Benchmark Suite
File:   Platform Atomics
Author: Scott Beamer

Wrappers for compiler intrinsics for atomic memory operations (AMOs)
 - If not using OpenMP (serial), provides serial fallbacks
*/
template<typename T, typename U>
T fetch_and_add(T &x, U inc) {
  T orig_val = x;
  x += inc;
  return orig_val;
}

template<typename T>
bool compare_and_swap(T &x, const T &old_val, const T &new_val) {
  if (x == old_val) {
    x = new_val;
    return true;
  }
  return false;
}

#endif  // PLATFORM_ATOMICS_H_