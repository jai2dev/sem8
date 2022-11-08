
#ifndef NAADSG_IO_H_
#define NAADSG_IO_H_

#include "graph.h"
#include <string>
#include <tuple>
#include <vector>

namespace std {

tuple<uint32_t,vector<uint32_t>> read_train_gids(string filename);
    tuple<uint32_t,vector<edge>> read_edges(string filename,vector<uint32_t> &train_gids);

void branches_to_file(vector<Branch> &branches, string branches_file);
}

#endif
