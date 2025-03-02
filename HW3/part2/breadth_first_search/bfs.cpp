#include "bfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstddef>
#include <omp.h>

#include "../common/CycleTimer.h"
#include "../common/graph.h"

#define ROOT_NODE_ID 0
#define NOT_VISITED_MARKER -1

void vertex_set_clear(vertex_set *list)
{
    list->count = 0;
}

void vertex_set_init(vertex_set *list, int count)
{
    list->max_vertices = count;
    list->vertices = (int *)malloc(sizeof(int) * list->max_vertices);
    vertex_set_clear(list);
}

// Take one step of "top-down" BFS.  For each vertex on the frontier,
// follow all outgoing edges, and add all neighboring vertices to the
// new_frontier.

void top_down_step(
    Graph g,
    vertex_set *frontier,
    vertex_set *new_frontier,
    int *distances,
    bool *botton_up_new_frontier)
{   
    //int current;
    //int *local_frontier = (int*)malloc(sizeof(int) * g->num_nodes);
    #pragma omp parallel
    {
        //Vertex* local_frontier = new Vertex[g->num_nodes];
        int *local_frontier = (int*) malloc(sizeof(int) * g->num_nodes);
        int local_count = 0;
        #pragma omp for
        for (int i = 0; i < frontier->count; i++)
        {
            int node = frontier->vertices[i];
            //int distance = distances[node];
            
            int start_edge = g->outgoing_starts[node];
            int end_edge = (node == g->num_nodes - 1) ? g->num_edges : g->outgoing_starts[node + 1];

            // attempt to add all neighbors to the new frontier
            for (int neighbor = start_edge; neighbor < end_edge; neighbor++)
            {
                int outgoing = g->outgoing_edges[neighbor];

                if (distances[outgoing] == NOT_VISITED_MARKER)
                {
                    // handle race condition
                    __sync_bool_compare_and_swap(&distances[outgoing], NOT_VISITED_MARKER, distances[node] + 1);
                    // record new node
                    local_frontier[local_count] = outgoing;
                    // record count of node
                    local_count++;

                    if(botton_up_new_frontier){
                        botton_up_new_frontier[outgoing] = 1;
                    }
                }
            }
        }
        #pragma omp critical
        {
            memcpy(new_frontier->vertices + new_frontier->count, local_frontier, sizeof(int) * local_count);
            new_frontier->count += local_count;
        }
        delete [] local_frontier;
    }
}

// Implements top-down BFS.
//
// Result of execution is that, for each node in the graph, the
// distance to the root is stored in sol.distances.
void bfs_top_down(Graph graph, solution *sol)
{

    vertex_set list1;
    vertex_set list2;
    vertex_set_init(&list1, graph->num_nodes);
    vertex_set_init(&list2, graph->num_nodes);

    vertex_set *frontier = &list1;
    vertex_set *new_frontier = &list2;

    // initialize all nodes to NOT_VISITED
    #pragma omp parallel for schedule(dynamic, 1024)
    for (int i = 0; i < graph->num_nodes; i++)
        sol->distances[i] = NOT_VISITED_MARKER;

    // setup frontier with the root node
    frontier->vertices[frontier->count++] = ROOT_NODE_ID;
    sol->distances[ROOT_NODE_ID] = 0;

    while (frontier->count != 0)
    {

#ifdef VERBOSE
        double start_time = CycleTimer::currentSeconds();
#endif

        vertex_set_clear(new_frontier);

        top_down_step(graph, frontier, new_frontier, sol->distances, nullptr);

#ifdef VERBOSE
        double end_time = CycleTimer::currentSeconds();
        printf("frontier=%-10d %.4f sec\n", frontier->count, end_time - start_time);
#endif

        // swap pointers
        vertex_set *tmp = frontier;
        frontier = new_frontier;
        new_frontier = tmp;
    }
}


bool botton_up_step(Graph g, bool *frontier, bool *new_frontier, int *distances)
{
    bool end = true;
    
    //--------------------------------pesudocode--------------------------------------
    // for(each vertex v in graph)
    // if(v has not been visited && v shares an incoming edge with a vertex u on the frontier)
    //    add vertex v to frontier;

    #pragma omp parallel for schedule(dynamic, 1024)
    for (int node = 0; node < g->num_nodes; node++)
    {   
        //v has not been visited
        if (distances[node] == -1)
        {
            int start_edge = g->incoming_starts[node];
            int end_edge = (node == g->num_nodes - 1)
                           ? g->num_edges
                           : g->incoming_starts[node + 1];
            
            for(int neighbor = start_edge; neighbor < end_edge; neighbor++)
            {
                int incoming = g->incoming_edges[neighbor];
                
                //&& v shares an incoming edge with a vertex u on the frontier  
                if(frontier[incoming])
                {   
                    end = 0;
                    //last step if visited (----add vertex to frontier----)
                    new_frontier[node] = 1;
                    distances[node] = distances[incoming] + 1;
                    break;
                }

            }
        }
    }
    return end;
}

void bfs_bottom_up(Graph graph, solution *sol)
{   

    

    //use bool to record visit or not
    bool *frontier = (bool*)calloc(graph->num_nodes, sizeof(bool));
	bool *new_frontier = (bool*)calloc(graph->num_nodes, sizeof(bool));

    for(int i = 0 ; i < graph->num_nodes ; i++){
        //frontier[i] = NOT_VISITED_MARKER;
        sol->distances[i] = -1;
        frontier[i] = 0;
    }

    sol->distances[ROOT_NODE_ID] = 0;
    frontier[ROOT_NODE_ID] = 1;

    bool end = 0;
    while(!end)
    {
        #pragma omp parallel for
        for (int i = 0 ; i < graph->num_nodes ; i++)
            new_frontier[i] = false;

        end = botton_up_step(graph,frontier, new_frontier, sol->distances);

        bool *tmp = frontier;
		frontier = new_frontier;
		new_frontier = tmp;
    }
    // For PP students:
    //
    // You will need to implement the "bottom up" BFS here as
    // described in the handout.
    //
    // As a result of your code's execution, sol.distances should be
    // correctly populated for all nodes in the graph.
    //
    // As was done in the top-down case, you may wish to organize your
    // code by creating subroutine bottom_up_step() that is called in
    // each step of the BFS process.
}

void bfs_hybrid(Graph graph, solution *sol)
{
    vertex_set list1;
    vertex_set list2;
    vertex_set_init(&list1, graph->num_nodes);
    vertex_set_init(&list2, graph->num_nodes);
    vertex_set *frontier = &list1;
    vertex_set *new_frontier = &list2;

    bool *botton_up_frontier = (bool*)calloc(graph->num_nodes, sizeof(bool));
	bool *botton_up_new_frontier = (bool*)calloc(graph->num_nodes, sizeof(bool));

    float threshold = 0.1;
    float THRESHOLD = graph->num_nodes * threshold;
    // initialize all nodes to NOT_VISITED
    #pragma omp parallel for schedule(dynamic, 1024)
    for(int i = 0 ; i < graph->num_nodes ; i++){
        //frontier[i] = NOT_VISITED_MARKER;
        sol->distances[i] = NOT_VISITED_MARKER;
        botton_up_frontier[i] = 0;
    }

    sol->distances[ROOT_NODE_ID] = 0;
    botton_up_frontier[ROOT_NODE_ID] = 1;
    bool end = 0, botton_up = false;

    // setup frontier with the root node
    frontier->vertices[frontier->count++] = ROOT_NODE_ID;

    while (frontier->count != 0 && !end)
    {   
        #pragma omp parallel for
        for (int i = 0 ; i < graph->num_nodes ; i++)
            botton_up_new_frontier[i] = false;

        if( (float)(frontier->count) <= THRESHOLD){
            
            vertex_set_clear(new_frontier);
            top_down_step(graph, frontier, new_frontier, sol->distances, botton_up_new_frontier);
            // swap pointers
            vertex_set *tmp = frontier;
            frontier = new_frontier;
            new_frontier = tmp;
        }

        else{

            

            end = botton_up_step(graph,botton_up_frontier, botton_up_new_frontier, sol->distances);
            
            botton_up = true;
        }

        bool *tmp = botton_up_frontier;
        botton_up_frontier = botton_up_new_frontier;
        botton_up_new_frontier = tmp;
    }

    // while(!end)
    // {
    //     #pragma omp parallel for
    //     for (int i = 0 ; i < graph->num_nodes ; i++)
    //         botton_up_new_frontier[i] = false;

    //     end = botton_up_step(graph,botton_up_frontier, botton_up_new_frontier, sol->distances);

    //     bool *tmp = botton_up_frontier;
	// 	frontier = new_frontier;
	// 	botton_up_new_frontier = tmp;
    // }

    // For PP students:
    //
    // You will need to implement the "hybrid" BFS here as
    // described in the handout.
}
