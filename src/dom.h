#ifndef _DOM_H_
#define _DOM_H_
#include <iostream>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <functional>
#include <cassert>
#include <string>
#include "program.h"
void dom(Block *head) {
  std::vector<Block *> worklist;
  std::unordered_set<Block *> visited;
  std::vector<std::set<int>> pred;
  std::unordered_map<Block *, int> block2Idx;
  std::function<void(Block *)> dfs;
  // post order dfs
  dfs = [&](Block *b) {
    for (auto &c : b->child) {
      if (!visited.count(c)){
        visited.insert(c);
        dfs(c);
      }
    }
    worklist.push_back(b);
    pred.push_back(std::set<int>());
    block2Idx[b] = worklist.size() - 1;
    assert(pred.size() == worklist.size());
    for (auto &c : b->child) {
      pred[block2Idx[c]].insert(worklist.size() - 1);
    }
  };
  dfs(head);
  std::vector<int> idom(worklist.size(), -1);
  assert(block2Idx[head] == worklist.size() - 1);
  idom[block2Idx[head]] = block2Idx[head];
  auto intersect = [&](int finger1, int finger2) -> int {
    while (finger1 != finger2) {
      while (finger1 < finger2)
        finger1 = idom[finger1];
      while (finger1 > finger2)
        finger2 = idom[finger2];
    }
    return finger1;
  };
  bool hasChanged = true;
  while(hasChanged) {
    hasChanged = false;
    // reversed postorder
    for (int i = worklist.size() - 2; i >= 0; i--) {
      int newIDom = -1;
      for (auto p : pred[i]){
        if (newIDom == -1 && idom[p] != -1) { // first processed predecessor of i 
          newIDom = p;
        } else if (idom[p] != -1 && newIDom != -1) {
          newIDom = intersect(p, newIDom); 
        }
      }
      if (newIDom != idom[i]){
        idom[i] = newIDom;
        hasChanged = true;
      }
    }
  }
  idom[idom.size() - 1] = -1;
  for (int i = worklist.size() - 1; i >= 0; i--) {
    std::cout<<worklist[i]->name <<": "<<std::endl; 
    for (int j = i; idom[j] != -1; j = idom[j]) {
      std::cout<<worklist[j]->name<<",";
    }
    std::cout<<std::endl;
  }


}
#endif

