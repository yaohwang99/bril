#ifndef _TDCE_H_
#define _TDCE_H_
#include "program.h"
#include <iostream>
#include <queue>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <utility>
void tdce(Block *head) {
  std::queue<Block *> q;
  std::set<Block *> visited;
  if (!head)
    return;
  // remove redefinition in single block
  auto removeRedef = [] (Block *c) {  
    std::unordered_map<std::string, int> unusedDef;
    std::unordered_set<int> shouldDelete;
    for (int idx = 0 ; idx < c->instrs.size(); idx++) {
      auto &I = c->instrs[idx];
      if(I.isMember("args")){
        for (auto &V : I["args"]) {
          unusedDef.erase(V.asString());
        }
      }
      if (I.isMember("dest")){
        auto &V = I["dest"];
        if (unusedDef.count(V.asString())) {
          shouldDelete.insert(unusedDef[V.asString()]);
        }
        unusedDef[V.asString()] = idx;
      }
    }
    for (int idx = c->instrs.size() - 1; idx >= 0; idx--) {
      if (shouldDelete.count(idx)) {
        c->instrs.erase(c->instrs.begin() + idx);
      }
    }
  };
  visited.insert(head);
  q.push(head);
  while (!q.empty()) {
    Block *B = q.front();
    q.pop();
    removeRedef(B);
    for (auto *b : B->child) {
      if (!visited.count(b)) {
        visited.insert(b);
        q.push(b);
      }
    }
  }
  // remove unused definition not used in the whole function
  std::unordered_map<std::string, std::pair<std::string, int>> unusedDef; // var name to <block name, instr id>
  std::set<std::pair<std::string, int>> shouldDelete;
  bool hasConverged = true;
  do {
    hasConverged = true;
    unusedDef.clear();
    shouldDelete.clear();
    visited.clear();
    visited.insert(head);
    q.push(head);
    while (!q.empty()) {
      Block *B = q.front();
      q.pop();
      for (int idx = 0 ; idx < B->instrs.size(); idx++) {
        auto &I = B->instrs[idx];
        if (I.isMember("args")) {
          for (auto &V : I["args"]) {
            if (unusedDef.find(V.asString()) != unusedDef.end()) {
              shouldDelete.erase(unusedDef[V.asString()]);
              unusedDef.erase(V.asString());
            }
          }    
        }
        if(I.isMember("dest")){
          auto &V = I["dest"];
          unusedDef[V.asString()] = {B->name, idx};
          shouldDelete.insert({B->name,idx});
        }
      }
      for (auto *b : B->child) {
        if (!visited.count(b)) {
          visited.insert(b);
          q.push(b);
        }
      }
    }
    visited.clear();
    visited.insert(head);
    q.push(head);
    while (!q.empty()) {
      Block *B = q.front();
      q.pop();
      for (int idx = B->instrs.size(); idx >= 0; idx--) {
        if (shouldDelete.count({B->name, idx})) {
          B->instrs.erase(B->instrs.begin() + idx);
          hasConverged = false;
        }
      }
      for (auto *b : B->child) {
        if (!visited.count(b)) {
          visited.insert(b);
          q.push(b);
        }
      }
    }
  } while (!hasConverged);

}
#endif
