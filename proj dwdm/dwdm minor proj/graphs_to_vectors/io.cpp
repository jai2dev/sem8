

#include <fcntl.h>
#include <fstream>
#include "graph.h"
#include "io.h"
#include <iostream>
#include "param.h"
#include <string>
#include <sstream>
#include <tuple>
#include <unistd.h>
#include "util.h"
#include <vector>
#include <algorithm>

namespace std {
 

tuple<uint32_t,vector<uint32_t>> read_train_gids(string filename) {
      // read train gids into memory
  cerr << "Reading Train gids from: " << filename << endl;

  vector<uint32_t> gids;
  uint32_t num_train_graphs = 0;
  ifstream f(filename);
  string line;


  // read train gids from the file
  while ( getline(f, line)){
    uint32_t graph_id;
    stringstream ss;
    ss.str(line);
    ss >> graph_id;
    gids.push_back(graph_id);
    num_train_graphs++;
  }

#ifdef DEBUG
  for (uint32_t i = 0; i < gids.size(); i++) {
    cout << "graph " << gids.at(i) << endl;
  }
  cout << "Number of train graphs: " << num_train_graphs << endl;
#endif
return make_tuple(num_train_graphs, gids);

}
tuple<uint32_t,vector<edge>> read_edges (string filename,vector<uint32_t> &train_gids){

  vector<edge>  train_edges;
  uint32_t num_train_edges = 0;
  ifstream f(filename);
  string line;

    cerr << "Reading edges from: " << filename << endl;
  // read edges from the file
  uint32_t i = 0;
  uint32_t max_gid = 0;


  while ( getline(f, line)){
    string src_type, dst_type, e_type;
    uint32_t src_id,dst_id,graph_id;
    stringstream ss;
    ss.str(line);
    ss >> src_id;
    ss >> src_type;
    ss >> dst_id;
    ss >> dst_type;
    ss >> e_type;
    ss >> graph_id;
    if (graph_id > max_gid) {
      max_gid = graph_id;
    }

    i++; // skip newline
    if (find(train_gids.begin(),train_gids.end(),graph_id)!=train_gids.end()){ // if the graph is a train graph
      train_edges.push_back(make_tuple(src_id, src_type,
                                       dst_id, dst_type,
                                       e_type, graph_id));
      num_train_edges++;
    }
  }

  return make_tuple(max_gid + 1, train_edges);

}
void branches_to_file(vector<Branch> &branches, string branches_file){
        ofstream out;
        out.open(branches_file);
        // save prototype_branches
        for(auto &b: branches){
            out << b.r <<'\t' << b.d_out <<'\t' << b.d_in << endl;
            if (b.d_out >0){
                for (auto &e: b.es_out){
                    out << e.first <<'\t'<< e.second<<'\t';
                }
                out << endl;
            } else {
                out << endl;
            }
            if (b.d_in > 0 ){
                for (auto &e: b.es_in){
                    out << e.first <<'\t'<< e.second<<'\t';
                }
                out << endl;
            } else{
                out << endl;
            }
        }
        out.close();
    }


}
