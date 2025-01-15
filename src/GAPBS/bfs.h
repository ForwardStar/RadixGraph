#ifndef GRAPHINDEX_BFS_H
#define GRAPHINDEX_BFS_H

#include <iostream>
#include <vector>

#include "benchmark.h"
#include "bitmap.h"
#include "command_line.h"
#include "platform_atomics.h"
#include "pvector.h"
#include "sliding_queue.h"
#include "../forward_star.h"

extern int64_t BUStep(ForwardStar* g, pvector<NodeID> &parent, Bitmap &front,
               Bitmap &next, int vertex_num);
extern int64_t TDStep(ForwardStar* g, pvector<NodeID> &parent,
               SlidingQueue<DummyNode*> &queue);
extern void QueueToBitmap(const SlidingQueue<DummyNode*> &queue, Bitmap &bm);
extern void BitmapToQueue(ForwardStar* g, int vertex_num, const Bitmap &bm,
                   SlidingQueue<DummyNode*> &queue);
extern pvector<NodeID> InitParent(ForwardStar* g, int vertex_num);
extern pvector<NodeID> DOBFS(ForwardStar* g, NodeID source, int vertex_num, int edge_num, int src_out_degree, int alpha = 15,
                      int beta = 18);

#endif //GRAPHINDEX_BFS_H
