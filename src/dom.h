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
#include "cfg.h"
#include "lr.h"
void dom(Block *head) {
  std::vector<Block *> worklist;
  std::unordered_set<Block *> visited;
  std::vector<std::set<int>> pred;
  std::unordered_map<Block *, int> block2Idx;
  std::function<void(Block *)> dfs;
  auto lrResult = lr(head);
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
  };
  dfs(head);
  visited.clear();
  std::queue<Block *>q;
  if (head){
    visited.insert(head);
    q.push(head);
  }
  while(!q.empty()){
    Block *b = q.front();
    q.pop();
    for (auto c : b->child){
      pred[block2Idx[c]].insert(block2Idx[b]);
      if (!visited.count(c)) {
        visited.insert(c);
        q.push(c);
      }
    }
  }

  std::vector<int> idom(worklist.size(), -1);
  std::vector<std::set<int>> df(worklist.size());
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
    std::cout<<"dom["<<worklist[i]->name<<"]:";
    for (int j = i; idom[j] != -1; j = idom[j]) {
      std::cout<<worklist[j]->name<<",";
    }
    std::cout<<std::endl;
  }

  for (int i = 0; i < df.size(); i++) {
    if (pred[i].size() >= 2) {
      for (int p : pred[i]){
        int runner = p;
        while (runner != idom[i]) {
          std::cout<<"insert " <<worklist[i]->name<<"into "<<worklist[runner]->name<<std::endl;
          df[runner].insert(i);
          runner = idom[runner];
        }
      }
    }
  }
  for (int i = 0; i < pred.size(); i++){
    std::cout<<"pred["<<worklist[i]->name<<"]:";
    for (int j : pred[i]){
      std::cout<< worklist[j]->name<<",";
    }
    std::cout<<std::endl;
  }

  for (int i = 0; i < df.size(); i++){
    std::cout<<"df["<<worklist[i]->name<<"]:";
    for (int j : df[i]){
      std::cout<< worklist[j]->name<<",";
    }
    std::cout<<std::endl;
  }
  std::vector<std::vector<int>> ichild(idom.size());
  for (int i = 0; i < idom.size(); i++) {
    if (idom[i] != -1)
      ichild[idom[i]].push_back(i);
  }
  for (int i = 0; i < df.size(); i++){
    std::cout<<"child["<<worklist[i]->name<<"]:";
    for (int j : ichild[i]){
      std::cout<< worklist[j]->name<<",";
    }
    std::cout<<std::endl;
  }

  std::map<std::string, std::queue<Block *>> defs; // var to blocks that defines var
  std::map<std::string, std::unordered_set<Block *>> visitedSet;
  std::map<Block *, std::unordered_set<std::string>> added;
  for(auto *B : worklist){
    for(auto &I : B->instrs) {
      if(I.isMember("dest")){
        if (visitedSet[I["dest"].asString()].count(B))
          continue;
        visitedSet[I["dest"].asString()].insert(B);
        defs[I["dest"].asString()].push(B);
      }
    }
  }
  // convert to ssa
  for(auto &it : defs) {
    std::string v = it.first;
    auto &q = it.second;
    while (!q.empty()){
      auto *d = q.front();
      q.pop();
      for (int i : df[block2Idx[d]]) {
        auto *b = worklist[i];
        // make sure the var is alive and not added
        if (!added[b].count(v) && lrResult.first[b].count(v)){
          added[b].insert(v);
          std::cout<<"add phi("<<v<<") to "<<b->name<<std::endl;
          Json::Value newI(Json::ValueType::objectValue);
          newI["op"] = Json::Value("phi");
          newI["dest"] = Json::Value(v);
          newI["args"] = Json::Value(Json::arrayValue);
          newI["labels"] = Json::Value(Json::arrayValue);
          b->instrs.insert(b->instrs.begin(), newI);
          if (!visitedSet[v].count(b)) {
            visitedSet[v].insert(b);
            q.push(b);
          }
        }
      }
    }
  }
  std::cout<<"insert empty phi node:"<<std::endl;
  printCFG(head);
  std::function<void(Block *)> rename;
  std::unordered_map<std::string, std::stack<std::string>> stk;
  std::unordered_map<std::string, int> counter;

  rename = [&](Block *b) {
    std::vector<std::string> toBePoped;
    for (auto &I : b->instrs) {
      if (I.isMember("args") && I["op"].asString() != "phi"){
        for (int i = 0; i < I["args"].size(); i++){
          I["args"][i] = Json::Value(stk[I["args"][i].asString()].top());
        }
      }
      if (I.isMember("dest")){
        std::string oldName  = I["dest"].asString();
        if (counter.find(oldName) == counter.end())
          counter[oldName] = 1;
        std::string newName = oldName + "." + std::to_string(counter[oldName]++);
        I["dest"] = Json::Value(newName);
        stk[oldName].push(newName);
        toBePoped.push_back(oldName);
      }
    }
    for (auto *s : b->child) { 
      for (auto &ph : s->instrs) {
        if (ph.isMember("op") && ph["op"].asString() == "phi"){ 
          auto oldName = ph["dest"].asString();
          int dot = oldName.find(".");
          if (dot != -1) {
            oldName = oldName.substr(0, dot);
          }
          std::cout<<"finding "<<oldName<<std::endl;
          assert(stk.find(oldName) != stk.end());
          ph["args"].append(Json::Value(stk[oldName].top()));
          ph["labels"].append(Json::Value(b->name));
        }
      }
    }
    for (int c : ichild[block2Idx[b]]){
      rename(worklist[c]);
    }
    for (auto v : toBePoped) {
      stk[v].pop();
    }
  };
  rename(head);
}


#endif

