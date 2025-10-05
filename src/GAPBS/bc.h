#ifndef GRAPHINDEX_BC_H
#define GRAPHINDEX_BC_H

#include <functional>
#include <iostream>
#include <vector>

#include "benchmark.h"
#include "bitmap.h"
#include "platform_atomics.h"
#include "pvector.h"
#include "sliding_queue.h"
#include "../radixgraph.h"

typedef float ScoreT;
typedef double CountT;

void PBFS(RadixGraph *g, NodeID source, pvector<CountT> &path_counts,
    Bitmap &succ, std::vector<SlidingQueue<NodeID>::iterator> &depth_index,
    SlidingQueue<NodeID> &queue, uint32_t num_nodes);

pvector<ScoreT> Brandes(RadixGraph *g, NodeID num_iters, uint32_t num_nodes);

#endif