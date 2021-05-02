#include "swarm_intrinsics.h"
#include "scc/queues.h"
#include "scc/autoparallel.h"
int __argc;
char **__argv;
swarm_runtime::GraphT<int> edges;
swarm_runtime::GraphT<int> transposed_edges;
double *num_paths;
double *dependences;
bool *visited;
bool* in_frontier_3;
void visited_generated_vector_op_apply_func_2(int v) {
	visited[v] = (bool)0;
}
void dependences_generated_vector_op_apply_func_1(int v) {
	dependences[v] = 0;
}
void num_paths_generated_vector_op_apply_func_0(int v) {
	num_paths[v] = 0;
}
void forward_update(int src, int dst, swarm_runtime::VertexFrontier __output_frontier) {
	bool result_var4 = (bool)0;
	result_var4 = swarm_runtime::sum_reduce(num_paths[dst], num_paths[src]);
	if (result_var4) {
		if (!in_frontier_3[dst]) {
			in_frontier_3[dst] = true;
			swarm_runtime::builtin_addVertex(__output_frontier, dst);
		}
	}
}
bool visited_vertex_filter(int v) {
	bool output;
	output = (visited[v]) == ((bool)0);
	return output;
}
void mark_visited(int v) {
	visited[v] = (bool)1;
}
void mark_unvisited(int v) {
	visited[v] = (bool)0;
}
void backward_vertex_f(int v) {
	visited[v] = (bool)1;
	swarm_runtime::sum_reduce(dependences[v], (((float) 1)  / num_paths[v]));
}
void backward_update(int src, int dst) {
	swarm_runtime::sum_reduce(dependences[dst], dependences[src]);
}
void final_vertex_f(int v) {
	if ((num_paths[v]) != (0)) {
		dependences[v] = ((dependences[v] - (((float) 1)  / num_paths[v])) * num_paths[v]);
	} else {
		dependences[v] = 0;
	}
}
void reset(int v) {
	dependences[v] = 0;
	num_paths[v] = 0;
	visited[v] = 0;
}
SWARM_FUNC_ATTRIBUTES
void swarm_main() {
	for (int _iter = 0, m = swarm_runtime::builtin_getVertices(edges); _iter < m; _iter++) {
		reset(_iter);
	};
	swarm::UnorderedQueue<int>* frontier = new swarm::UnorderedQueue<int>();
	frontier->init(swarm_runtime::builtin_getVertices(edges));
	int start_vertex = atoi(__argv[2]);
	swarm_runtime::builtin_addVertex(frontier, start_vertex);
	num_paths[start_vertex] = 1;
	visited[start_vertex] = (bool)1;
	int round = 0;
	swarm_runtime::VertexFrontierList frontier_list = swarm_runtime::create_new_vertex_frontier_list(swarm_runtime::builtin_getVertices(edges));
	swarm_runtime::builtin_insert(frontier_list, frontier);
	for (int i = 0, m = 0; i < m; i++) {
		frontier->push(i);
	}
	in_frontier_3 = new bool [swarm_runtime::builtin_getVertices(edges)]();
	while ((swarm_runtime::builtin_getVertexSetSize(frontier)) != (0)) {
		round = (round + 1);
		swarm::UnorderedQueue<int>* output = new swarm::UnorderedQueue<int>();
		output->init(swarm_runtime::builtin_getVertices(edges));
		frontier->for_each([output](int src) {
			int32_t edgeZero = edges.h_src_offsets[src];
			int32_t edgeLast = edges.h_src_offsets[src+1];
			for (int j = edgeZero; j < edgeLast; j++) {
				int dst = edges.h_edge_dst[j];
				if (visited_vertex_filter(dst)) {
					{
						swarm::UnorderedQueue<int>* __output_frontier = output;
						bool result_var4 = (bool)0;
						result_var4 = swarm_runtime::sum_reduce(num_paths[dst], num_paths[src]);
						if (result_var4) {
							if (!in_frontier_3[dst]) {
								in_frontier_3[dst] = true;
								swarm_runtime::builtin_addVertex(__output_frontier, dst);
							}
						}
					}
				}
			}
		});
		frontier->for_each([](int v) { 
			{
				in_frontier_3[v] = (bool)0;
			}
		});
		swarm_runtime::deleteObject(frontier);
		output->for_each([](int v) { 
			mark_visited(v);
		});
		swarm_runtime::builtin_insert(frontier_list, output);
		frontier = output;
	}
	delete[] in_frontier_3;
	for (int _iter = 0, m = swarm_runtime::builtin_getVertices(edges); _iter < m; _iter++) {
		mark_unvisited(_iter);
	};
	swarm_runtime::builtin_retrieve(frontier_list, frontier);
	swarm_runtime::builtin_retrieve(frontier_list, frontier);
	frontier->for_each([](int v) { 
		backward_vertex_f(v);
	});
	round = (round - 1);
	in_frontier_3 = new bool [swarm_runtime::builtin_getVertices(edges)]();
	while ((round) > (0)) {
		frontier->for_each([](int src) {
			int32_t edgeZero = transposed_edges.h_src_offsets[src];
			int32_t edgeLast = transposed_edges.h_src_offsets[src+1];
			for (int j = edgeZero; j < edgeLast; j++) {
				int dst = transposed_edges.h_edge_dst[j];
				if (visited_vertex_filter(dst)) {
					{
						swarm_runtime::sum_reduce(dependences[dst], dependences[src]);
					}
				}
			}
		});
;
		swarm_runtime::builtin_retrieve(frontier_list, frontier);
		frontier->for_each([](int v) { 
			backward_vertex_f(v);
		});
		round = (round - 1);
	}
	delete[] in_frontier_3;
	swarm_runtime::deleteObject(frontier);
	for (int _iter = 0, m = swarm_runtime::builtin_getVertices(edges); _iter < m; _iter++) {
		final_vertex_f(_iter);
	};
}

#include <iostream>
#include <iomanip>
#include <fstream>
int main(int argc, char* argv[]) {
	__argc = argc;
	__argv = argv;
	swarm_runtime::load_graph(edges, __argv[1]);
	
	num_paths = new double[swarm_runtime::builtin_getVertices(edges)];
	dependences = new double[swarm_runtime::builtin_getVertices(edges)];
	visited = new bool[swarm_runtime::builtin_getVertices(edges)];
	for (int _iter = 0, m = swarm_runtime::builtin_getVertices(edges); _iter < m; _iter++) {
		num_paths_generated_vector_op_apply_func_0(_iter);
	};
	for (int _iter = 0, m = swarm_runtime::builtin_getVertices(edges); _iter < m; _iter++) {
		dependences_generated_vector_op_apply_func_1(_iter);
	};
	for (int _iter = 0, m = swarm_runtime::builtin_getVertices(edges); _iter < m; _iter++) {
		visited_generated_vector_op_apply_func_2(_iter);
	};
	transposed_edges = swarm_runtime::builtin_transpose(edges);
	SCC_PARALLEL( swarm_main(); );

	std::ofstream f("bc_answers.txt");
        if (!f.is_open()) {
                printf("file open failed.\n");
                return -1;
        }
        for (int i = 0; i < swarm_runtime::builtin_getVertices(edges); i++) {
                f << std::setprecision(32) << dependences[i] << std::endl;
        }
        f.close();
}