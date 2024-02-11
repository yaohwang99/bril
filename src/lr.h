#ifndef _LR_H_
#define _LR_H_
#include "program.h"
#include <string>
#include <set>
#include <map>
#include <queue>
#include <vector>
#include <utility>
std::pair<std::map<Block *, std::set<std::string>>,std::map<Block *, std::set<std::string>>>  lr(Block *head) {
  if (!head)
    return {};
  std::queue<Block *> q;
  std::set<Block *> visited;
  std::map<Block *, std::set<std::string>> in, out;
  std::map<Block *, std::vector<Block *>> pred;
  std::vector<Block *> initWorklist;
  visited.insert(head);
  initWorklist.push_back(head);
  q.push(head);
  while (!q.empty()) {
    Block *B = q.front();
    initWorklist.push_back(B);
    q.pop();
    for (auto *b : B->child) {
      pred[b].push_back(B);
      if (!visited.count(b)) {
        visited.insert(b);
        q.push(b);
      }
    }
  }
  std::queue<Block *> workList;
  for (auto it = initWorklist.rbegin(); it != initWorklist.rend(); it++) {
    workList.push(*it);
  }

  while (!workList.empty()) {
    Block *b = workList.front();
    workList.pop();
    auto sz1 = in[b].size();
    for (Block *succ : b->child) {
      for (auto &V : in[succ])
        out[b].insert(V);
    }
    auto currOut = out[b];
    auto currIn = std::set<std::string>();
    for (int i = b->instrs.size() - 1; i >= 0; i--) {
      auto &I = b->instrs[i];
      auto diff = currOut;
      currIn.clear();
      if (I.isMember("dest")) { // diff = out - def
        diff.erase(I["dest"].asString());
      }
      if (I.isMember("args")) {
        for (auto &V : I["args"]) { // in = use
          currIn.insert(V.asString());
        }
      }
      for (auto &V : diff) { // in = in U diff
        currIn.insert(V);
      }
      currOut = currIn;
    }
    in[b] = currIn;
    if (sz1 != in[b].size()) {
      for (Block *p : pred[b]) {
        workList.push(p); 
      }
    }
      std::cout<<b->name<<": "<<std::endl;
      std::cout<<"in: ";
      for (auto &V : in[b]){
        std::cout<<V<<",";
      }
      std::cout<<std::endl;
      std::cout<<"out: ";
      for (auto &V : out[b]) {
        std::cout<<V<<",";
      }
      std::cout<<std::endl;
  }
  return {in, out};
}
#endif
