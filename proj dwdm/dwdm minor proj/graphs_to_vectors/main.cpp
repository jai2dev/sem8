

#include <algorithm>
#include <bitset>
#include <cassert>
#include <deque>
#include <iostream>

#include <string>
#include <unordered_map>
#include <set>
#include <vector>
#include <algorithm>
#include <sstream>
#include <random>
#include <chrono>
#include <fstream>


#include "docopt.h"
#include "graph.h"
#include "hash.h"
#include "io.h"
#include "param.h"



using namespace std;

static const char USAGE[] =
        R"(LEADS (Training phase).

    Usage:
      LEADS --edges=<edge file>
		     --train=<train graphs file>
             --dataset=<dataset>
             --M=<number of branches>
             --prototypes=<prototype branchs output>
             --graph_vectors=<train graph vectors output>

      LEADS (-h | --help)

    Options:
      -h, --help                              Show this screen.
      --edges=<edge file>                     Incoming stream of edges.
      --train=<train graphs file>	          Train graphs id's
      --dataset=<dataset>                     'ALL', 'YDC', 'GFC', 'YDG','AUTH,.
      --M=<number of branches>                Number of prototype branches
      --prototypes=<prototype branchs output> Output file of prototype branches
      --graph_vectors=<train graph vectors output> Output file of train graph vectors
)";

long M;

int main(int argc, char *argv[]) {

    // arguments
    map<string, docopt::value> args = docopt::docopt(USAGE, {argv + 1, argv + argc});
    string edge_file(args["--edges"].asString());
    string train_gids_file(args["--train"].asString());
    string prototypes_output(args["--prototypes"].asString());
    string graph_vectors_output(args["--graph_vectors"].asString());
    M = args["--M"].asLong();
    string dataset(args["--dataset"].asString());
    if (!(dataset.compare("ALL") == 0 ||
          dataset.compare("AUTH")== 0 ||
          dataset.compare("YDC") == 0 ||
          dataset.compare("YDG") == 0 ||
          dataset.compare("GFC") == 0)) {
        cout << "Invalid dataset: " << dataset << ". ";
        exit(-1);
    }



    uint32_t num_graphs;
    vector<uint32_t> train_gids;
    vector<edge> train_edges;


    // reading training Gids
    cerr << "Reading training gids..." << endl;
    tie(num_graphs, train_gids) = read_train_gids(train_gids_file);

    cerr << "Reading ALL edges..." << endl;
    tie(num_graphs, train_edges) = read_edges(edge_file, train_gids);




    // per-graph data structures
    unordered_map<uint32_t, graph_vector> graphs_vectors; // key = gid , value = the graph vector
    // initialization of graphs vectors
    for (int i = 0; i < num_graphs; i++) {
        for (int j = 0; j < M; j++) graphs_vectors[i].push_back(0);

    }
    unordered_map<uint32_t, unordered_map<uint32_t, Branch>> train_graphs_to_branches; /* key 1 = gid
                                                                                     key 2 = branch id
                                                                                     value = branch*/
    // construct training graphs
    vector<graph> graphs(num_graphs);
    cerr << "Constructing " << num_graphs << "train graphs..." << endl;
    for (auto &e : train_edges) {
        update_graphs(e, graphs);
    }

    cerr << "End of train graphs instantiation" << endl;

    cerr << "Decomposition of " << num_graphs << "train graph  to branches..." << endl;
    // decompose the training graphs to vectors of branches
    train_graphs_to_branches = graph_to_branches(graphs);// key = gid , value = map of the branches
    cerr << "End of the decomposition " << endl;
    cerr << "The selection of prototype branches" << endl;
    vector<Branch> prototype_branches;
    vector<Branch> median_branches;
    tie (prototype_branches,median_branches) = construct_prototype_branches(train_graphs_to_branches, train_gids,dataset);

    cout << prototype_branches.size() << endl;
    // saving prototype and median branches to files
    branches_to_file(prototype_branches,prototypes_output);


    cerr << "Transform training  graphs to vectors" << endl;

    vector<double> train_graphs_sizes; // index = gid , value = the size of the graph
    unordered_map<uint32_t, double> graphs_sizes; // key = gid ; value = the size of the graph

    for (auto g : graphs) {
        uint32_t size = 0;
        for (auto &r : g) {
            size += r.second.size();
        }
        train_graphs_sizes.push_back(size);
    }
    vector<graph>().swap(graphs);
    cerr << "Converting test graphs to vectors" << endl;
    for (auto &g : train_graphs_to_branches) {
        if (find(train_gids.begin(), train_gids.end(), g.first) != train_gids.end()) { // is a train graph
            for (int i = 0; i < prototype_branches.size(); i++) {
                graphs_vectors[g.first].push_back(0);
            }
            cout << g.first << endl;
            for (auto &br : g.second) {
                double w = (br.second.d_in + br.second.d_out) /
                           (2 * train_graphs_sizes.at(g.first)); // the weight of the branch br
                int k = 0;
                for (auto &brp:prototype_branches) {
                    double bed = compute_branch_edit_distance(br.second, brp);
                    graphs_vectors[g.first].at(k) += w * (1 - bed);
                    k++;
                }
            }
        }
    }
    /// saving the train graph vectors to file
    ofstream out_file;
    out_file.open(graph_vectors_output);
    for (auto &gid:train_gids) {
        cout << gid << "\t";
        out_file << gid << "\t";
        for (int i = 0; i < (M-1); i++) {
            out_file << graphs_vectors[gid].at(i) <<"\t";
            cout << graphs_vectors[gid].at(i) << "\t";
        }
        out_file << graphs_vectors[gid].at(M-1) << endl;
        cout << graphs_vectors[gid].at(M-1) << endl;
    }

    return 0;
}