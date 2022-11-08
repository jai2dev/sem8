
#include <algorithm>
#include <iostream>
#include <set>
#include <cmath>
#include <fstream>
#include "graph.h"
#include "hash.h"
#include "param.h"


namespace std {

    void update_graphs(edge &e, vector<graph> &graphs) {
        auto &src_id = get<F_S>(e);
        auto &src_type = get<F_STYPE>(e);
        auto &dst_id = get<F_D>(e);
        auto &dst_type = get<F_DTYPE>(e);
        auto &e_type = get<F_ETYPE>(e);
        auto &gid = get<F_GID>(e);

        // append edge to the edge list for the source
        graphs[gid][make_pair(src_id,
                              src_type)].push_back(make_tuple(dst_id,
                                                              dst_type,
                                                              e_type));
    }

    tuple<vector<Branch>,vector<Branch>> construct_prototype_branches(unordered_map<uint32_t, unordered_map<uint32_t, Branch>> &map_graph_branches, vector<uint32_t> train_gids,string dataset) {


        vector<Branch> prototype_branches; // the prototype branches selected from train graphs
        vector<Branch> median_branches; // the median branches
        unordered_map<uint32_t, vector<Branch>> all_branches; /* All the branches grouped by the classes of
                                                                      benign train graphs*/

        if (dataset.compare("ALL")==0){
            for (auto gid: train_gids) {
                for (auto &m: map_graph_branches[gid]) all_branches[0].push_back(m.second); // we have one class
            }
        }
        else {
            for (auto gid: train_gids) {
                for (auto &m: map_graph_branches[gid]) all_branches[(gid) / 100].push_back(m.second);
            }
        }
        tie(prototype_branches,median_branches) =    SPS_C(all_branches); // the used algorithm for selecting the prototype branches is SPS-C

        return tie(prototype_branches,median_branches);
    }

    double compute_branch_edit_distance(Branch &Br1, Branch &Br2) {
        double bed = 0;
        double max_bed = 1+max(Br1.d_out, Br2.d_out)+max(Br1.d_in, Br2.d_in);
        if (Br1.r != Br2.r) bed += 1;
        bed += max(Br1.d_out, Br2.d_out);
        bed += max(Br1.d_in, Br2.d_in);
        if (max(Br1.d_out, Br2.d_out) == Br2.d_out) {
            for (auto &a : Br1.es_out)bed -= min(a.second, Br2.es_out[a.first]);
        } else {
            for (auto &a : Br2.es_out)bed -= min(a.second, Br1.es_out[a.first]);
        }
        if (max(Br1.d_in, Br2.d_in) == Br2.d_in) {
            for (auto &a : Br1.es_in)bed -= min(a.second, Br2.es_in[a.first]);
        } else {
            for (auto &a : Br2.es_in)bed -= min(a.second, Br1.es_in[a.first]);
        }
        return bed/max_bed;
    }

    tuple<vector<Branch>,vector<Branch>> SPS_C(unordered_map<uint32_t, vector<Branch>> &all_branches) {

        int index, median_index, furthest_branch_index, c;
        double d = 0;
        vector<Branch> prototype_branches;
        vector<Branch> median_branches;
        int nc = all_branches.size(); // the number of classes of train graphs

        c = 1; // the class id
        for (auto &m : all_branches) { // compute the prototype branches for each class
            cout << c << endl;
            // Find the median graph of the class
            vector<pair<int, double>> min_distances;
            index = 0;
            cout << "number of branches :" << m.second.size() << endl;                        // delete this

            median_index = rand()% (m.second.size());
            median_branches.push_back(m.second.at(median_index));

            prototype_branches.push_back(m.second.at(median_index)); // the median branch
            m.second.erase(m.second.begin() + median_index); // delete the median branch from the set of all branch
            vector<pair<int, double >>().swap(min_distances); // free the allocated memory
            // the spanning selector
            int counter = 1;
            while (counter < (M / nc)) {   // Select the furthest Branch away from the already selected prototypes
                index = 0;
                for (auto &br : m.second) {
                        d = INF;
                        for (auto &bp : prototype_branches) {
                            double bed = compute_branch_edit_distance(br, bp);
                            if (bed < d) d = bed;
                        }
                        min_distances.push_back(pair<int, double>(index, d));
                        index++;
                }
                furthest_branch_index = (*max_element(min_distances.begin(), min_distances.end(),
                                                      [](pair<int, double> a, pair<int, double> b) {
                                                          return a.second < b.second;
                                                      })).first;
                prototype_branches.push_back(m.second.at(furthest_branch_index)); // add the furthest branch
                m.second.erase(m.second.begin() +
                               furthest_branch_index); // delete the already added branch from the set of all branch
                vector<pair<int, double >>().swap(min_distances); // free the allocated memory
                counter++;
            }
            c++;
        }
        return tie(prototype_branches,median_branches);
    }

    unordered_map<uint32_t, unordered_map<uint32_t, Branch>> graph_to_branches(vector<graph> &graphs){

        unordered_map<uint32_t, unordered_map<uint32_t, Branch>> map_graph_branches; /* key1= gid , key2= root id */
        unordered_map<uint32_t, vector<Branch>> all_branches; /* All the branches grouped by the classes of
                                                                      benign train graphs*/
        int index = 0;
        // construct graph branches
        for (auto &g : graphs) {
            int gid = index;
            for (auto &e:g) { //
                int src_id = e.first.first;// the source root id
                string src_label = e.first.second; // the source root label
                for (auto &v: e.second) {
                    int dest_id = get<0>(v);
                    string dest_label = get<1>(v);
                    string edge_label = get<2>(v);
                    map_graph_branches[gid][src_id].r = src_label;
                    map_graph_branches[gid][dest_id].r = dest_label;
                    map_graph_branches[gid][src_id].es_out[edge_label]++;
                    map_graph_branches[gid][dest_id].es_in[edge_label]++;
                    map_graph_branches[gid][src_id].d_out++;
                    map_graph_branches[gid][dest_id].d_in++;
                }
            }
            index++;
        }
        return  map_graph_branches;
    }


}
