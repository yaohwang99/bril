#ifndef _DOM_H_
#define _DOM_H_
#include <iostream>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <functional>
#include "program.h"
void dom(Block *head) {
  std::vector<Block *> worklist;
  std::map<Block *, std::set<Block *>> dom;
  std::unordered_map<Block *, std::set<Block *>> pred;
  std::unordered_set<Block *> visited;
  std::function<void(Block *)> dfs;
  // post order dfs
  dfs = [&](Block *b) {
    for (auto &c : b->child) {
      pred[c].insert(b);
      if (!visited.count(c)){
        visited.insert(c);
        dfs(c);
      }
    }
    dom[b].insert(b);
    worklist.push_back(b);
  };
  dfs(head);
  for (int i = worklist.size() - 1; i >= 0; i--) {
    for (auto &it : dom) {
      if (it.first == head)
        continue;
      it.second.insert(worklist[i]);
    }
  }
  bool hasChanged = true;
  while(hasChanged) {
    hasChanged = false;
    for (int i = worklist.size() - 1; i >= 0; i--) {
      Block *B = worklist[i];
      int sz = dom[B].size();
      dom[B].clear();
      dom[B].insert(B);
      std::set<Block *> intersect;
      if (pred[B].size()) {
        intersect = dom[*pred[B].begin()];
        auto j = pred[B].begin();
        ++j;
        for (; j != pred[B].end(); j++){
          for(auto it = intersect.begin(); it != intersect.end();){
            if (!dom[*j].count(*it))
              it = intersect.erase(it);
            else
              it++;
          }
        }
      }
      for (Block *b : intersect) {
        dom[B].insert(b);
      }

      hasChanged |= (sz != dom[B].size());
    }
  }

  for(auto it : dom){
    std::cout<<it.first->name<< ": ";
    for(Block *b : it.second)
      std::cout<<b->name<<",";
    std::cout<<std::endl;
  }

}
#endif

