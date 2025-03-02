#include "page_rank.h"

#include <stdlib.h>
#include <cmath>
#include <omp.h>
#include <utility>

#include "../common/CycleTimer.h"
#include "../common/graph.h"

// pageRank --
//
// g:           graph to process (see common/graph.h)
// solution:    array of per-vertex vertex scores (length of array is num_nodes(g))
// damping:     page-rank algorithm's damping parameter
// convergence: page-rank algorithm's convergence threshold
//

void pageRank(Graph g, double *solution, double damping, double convergence)
{

  // initialize vertex weights to uniform probability. Double
  // precision scores are used to avoid underflow for large graphs
  
  int numNodes = num_nodes(g);
  bool converged = 0;
  double equal_prob = 1.0 / numNodes;

  #pragma omp parallel for
  for (int i = 0; i < numNodes; ++i)
  {
    solution[i] = equal_prob;
  }

  double *score_new = (double*) calloc(numNodes, sizeof(double));
  double out_num_vj, sum_leave_vj, global_diff = 0.0, no_num_out, no_score, out_size;
  while(!converged){
    no_num_out = 0.0;

    #pragma omp parallel for reduction(+:no_num_out)
    for (int i = 0; i < numNodes; i++)
    {
      int out_go_size = outgoing_size(g, i);
      if (out_go_size == 0) 
        no_num_out += solution[i]; 
      // else
      //   score_new[i] = solution[i] / out_go_size;
    }

    no_score = no_num_out * damping / numNodes;
    //#pragma omp parallel for schedule(dynamic, 1024)
    #pragma omp parallel for private(sum_leave_vj)
    for(int i = 0 ; i < numNodes; i++){
      score_new[i] = 0.0;
      sum_leave_vj = 0.0;
      const Vertex *start = incoming_begin(g, i);
      const Vertex *end = incoming_end(g, i);
      for (const Vertex *vj = start ; vj != end ; vj++){
    
        sum_leave_vj += solution[*vj] / (double)outgoing_size(g, *vj);
      }
      score_new[i] = (damping * sum_leave_vj) + ((1.0 - damping) / numNodes) + no_score;
    }

    global_diff = 0.0;
    #pragma omp parallel for reduction(+:global_diff)
    for (int i = 0; i < numNodes; i++)
    {
      global_diff += fabs(score_new[i] - solution[i]);
      solution[i] = score_new[i];
    }
    converged = (global_diff < convergence);
    
  }
  delete score_new;
  /*
     For PP students: Implement the page rank algorithm here.  You
     are expected to parallelize the algorithm using openMP.  Your
     solution may need to allocate (and free) temporary arrays.

     Basic page rank pseudocode is provided below to get you started:

     ///initialization: see example code above
     score_old[vi] = 1/numNodes;

     while (!converged) {

       ///compute score_new[vi] for all nodes vi:
       score_new[vi] = sum over all nodes vj reachable from incoming edges
                          { score_old[vj] / number of edges leaving vj  }
       score_new[vi] = (damping * score_new[vi]) + (1.0-damping) / numNodes;

       score_new[vi] += sum over all nodes v in graph with no outgoing edges
                          { damping * score_old[v] / numNodes }

       /// compute how much per-node scores have changed
       /// quit once algorithm has converged

       global_diff = sum over all nodes vi { abs(score_new[vi] - score_old[vi]) };
       converged = (global_diff < convergence)
     }

   */
}


