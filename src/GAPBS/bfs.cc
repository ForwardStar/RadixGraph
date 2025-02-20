// Copyright (c) 2015, The Regents of the University of California (Regents)
// See LICENSE.txt for license details

#include "bfs.h"
#include "time.h"

/*
GAP Benchmark Suite
Kernel: Breadth-First Search (BFS)
Author: Scott Beamer

Will return parent array for a BFS traversal from a source vertex

This BFS implementation makes use of the Direction-Optimizing approach [1].
It uses the alpha and beta parameters to determine whether to switch search
directions. For representing the frontier, it uses a SlidingQueue for the
top-down approach and a Bitmap for the bottom-up approach. To reduce
false-sharing for the top-down approach, thread-local QueueBuffer's are used.

To save time computing the number of edges exiting the frontier, this
implementation precomputes the degrees in bulk at the beginning by storing
them in parent array as negative numbers. Thus the encoding of parent is:
  parent[x] < 0 implies x is unvisited and parent[x] = -out_degree(x)
  parent[x] >= 0 implies x been visited

[1] Scott Beamer, Krste Asanović, and David Patterson. "Direction-Optimizing
    Breadth-First Search." International Conference on High Performance
    Computing, Networking, Storage and Analysis (SC), Salt Lake City, Utah,
    November 2012.
*/
int64_t BUStep(ForwardStar* g, pvector<NodeID> &parent, Bitmap &front,
               Bitmap &next, int vertex_num) {
    int64_t awake_count = 0;
    next.reset();
    #pragma omp parallel for reduction(+ : awake_count) schedule(dynamic, 1024)
    for (int i = 0; i < vertex_num; i++) {
        if (parent[i] == -1) {
            auto u = g->vertex_index->dummy_nodes[i];
            // if (!u || u->node == -1) continue;
            std::vector<WeightedEdge> neighbours;
            g->GetNeighbours(u, neighbours);
            for (auto e : neighbours) {
                int vidx = e.idx;
                if (front.get_bit(vidx)) {
                    parent[i] = vidx;
                    awake_count++;
                    next.set_bit(i);
                    break;
                }
            }
        }
    }
    return awake_count;
}

int64_t TDStep(ForwardStar* g, pvector<NodeID> &parent,
               SlidingQueue<int> &queue) {
    int64_t scout_count = 0;
    long int thread_times[64] = {0};
    #pragma omp parallel
    {
        QueueBuffer<int> lqueue(queue);
        #pragma omp for reduction(+ : scout_count) nowait schedule(dynamic)
        for (auto q_iter = queue.begin(); q_iter < queue.end(); q_iter++) {
            NodeID from_node_id = *q_iter;
            DummyNode* u = g->vertex_index->dummy_nodes[from_node_id];
            std::vector<WeightedEdge> neighbours;
            g->GetNeighbours(u, neighbours);
            for (auto e : neighbours) {
                int vidx = e.idx;
                NodeID curr_val = parent[vidx];
                if (curr_val == -1) {
                    if (compare_and_swap(parent[vidx], curr_val, from_node_id)) {
                        lqueue.push_back(vidx);
                        scout_count += g->degree[vidx];
                    }
                }
            }
        }

        lqueue.flush();
    }
    return scout_count;
}

void QueueToBitmap(const SlidingQueue<int> &queue, Bitmap &bm) {
  #pragma omp parallel for
  for (auto q_iter = queue.begin(); q_iter < queue.end(); q_iter++) {
    bm.set_bit_atomic(*q_iter);
  }
}

void BitmapToQueue(ForwardStar* g, int vertex_num, const Bitmap &bm,
                   SlidingQueue<int> &queue) {
  #pragma omp parallel
  {
    QueueBuffer<int> lqueue(queue);
    #pragma omp for nowait
    for (NodeID n = 0; n < vertex_num; n++)
      if (bm.get_bit(n)) {
        lqueue.push_back(n);
      }
    lqueue.flush();
  }
  queue.slide_window();
}

pvector<NodeID> InitParent(ForwardStar* g, int vertex_num) {
    pvector<NodeID> parent(vertex_num);
    #pragma omp parallel for
    for (NodeID n = 0; n < vertex_num; n++) {
        parent[n] = -1;
    }
    return parent;
}

pvector<NodeID> DOBFS(ForwardStar* g, NodeID source, int vertex_num, int edge_num, int src_out_degree, int alpha,
                      int beta) {
    auto u = g->vertex_index->RetrieveVertex(source);
    if (src_out_degree == -1) src_out_degree = u->deg;
    pvector<NodeID> parent = InitParent(g, vertex_num);
    parent[u->idx] = u->idx;

    SlidingQueue<int> queue(vertex_num);
    queue.push_back(u->idx);
    queue.slide_window();
    Bitmap curr(vertex_num);
    curr.reset();
    Bitmap front(vertex_num);
    front.reset();
    int64_t edges_to_check = edge_num;
    int64_t scout_count = src_out_degree;
    while (!queue.empty()) {
        // edit for only top-down search
        if (scout_count > edges_to_check / alpha) {
            int64_t awake_count, old_awake_count;
            QueueToBitmap(queue, front);
            awake_count = queue.size();
            queue.slide_window();
            do {
                old_awake_count = awake_count;
                awake_count = BUStep(g, parent, front, curr, vertex_num);
                front.swap(curr);
            } while ((awake_count >= old_awake_count) ||
                    (awake_count > vertex_num / beta));
            BitmapToQueue(g, vertex_num, front, queue);
            scout_count = 1;
        } else {
            edges_to_check -= scout_count;
            scout_count = TDStep(g, parent, queue);
            queue.slide_window();
        }
    }
    return parent;
}