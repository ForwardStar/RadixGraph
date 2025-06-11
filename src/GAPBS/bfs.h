#ifndef GRAPHINDEX_BFS_H
#define GRAPHINDEX_BFS_H

#include "benchmark.h"
#include "bitmap.h"
#include "platform_atomics.h"
#include "pvector.h"
#include "sliding_queue.h"
#include "../radixgraph.h"

extern int64_t BUStep(RadixGraph* g, pvector<NodeID> &parent, Bitmap &front,
               Bitmap &next, int vertex_num);
extern int64_t TDStep(RadixGraph* g, pvector<NodeID> &parent,
               SlidingQueue<int> &queue);
extern void QueueToBitmap(const SlidingQueue<int> &queue, Bitmap &bm);
extern void BitmapToQueue(RadixGraph* g, int vertex_num, const Bitmap &bm,
                   SlidingQueue<int> &queue);
extern pvector<NodeID> InitParent(RadixGraph* g, int vertex_num);
extern pvector<NodeID> DOBFS(RadixGraph* g, NodeID source, int vertex_num, int edge_num, int src_out_degree, int alpha = 15,
                      int beta = 18);

#endif //GRAPHINDEX_BFS_H
