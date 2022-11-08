//
// Created by Abderrahmane on 6/16/2018.
//

#ifndef NAADSG_GRAPH_H
#define NAADSG_GRAPH_H

#include <vector>
#include <tuple>
#include <unordered_map>

namespace std {

// edge field indices
    #define F_S               0           // source node id
    #define F_STYPE           1           // source node type
    #define F_D               2           // destination node id
    #define F_DTYPE           3           // destination node type
    #define F_ETYPE           4           // edge type
    #define F_GID             5           // graph id (tag)

// data structures
    typedef struct Branch{
        string r; // the root of the branch
        unordered_map<string, int> es_out; // the edge structure of the outgoing edges from r
        unordered_map<string, int> es_in;  // the edge structure of the incoming edges to r
        uint32_t d_out; // the number of outgoing edges
        uint32_t d_in;  // the number of incoming edges
    } Branch;

    typedef tuple<uint32_t, string, uint32_t, string, string, uint32_t> edge;
    typedef unordered_map<pair<uint32_t,string>, vector<tuple<uint32_t,string,string>>> graph;
    typedef vector<double> graph_vector; // vector representation of a graph

    void update_graphs(edge& e, vector<graph>& graphs);

    double compute_branch_edit_distance(Branch &Br1,Branch &Br2);

    tuple<vector<Branch>,vector<Branch>> construct_prototype_branches(unordered_map<uint32_t, unordered_map<uint32_t, Branch>>
                                                &map_graph_branches, vector<uint32_t> train_gids,string dataset);

    tuple<vector<Branch>,vector<Branch>> SPS_C(unordered_map<uint32_t ,vector<Branch>>  &all_branches); /* the Spanning wise class prototypes
                                                                                   * selector */
    unordered_map<uint32_t, unordered_map<uint32_t, Branch>> graph_to_branches(vector<graph> &graphs); /* graph
                                                                                                        decomposition to
                                                                                                branches*/
}

#endif //NAADSG_GRAPH_H
