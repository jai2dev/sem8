
#ifndef NAADSG_UTIL_H_
#define NAADSG_UTIL_H_

#include <string>
#include <iostream>

namespace std {

inline void panic(string message) {
  cout << message << endl;
  exit(-1);
}


}

#endif
