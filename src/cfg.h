#ifndef _CFG_H_
#define _CFG_H_
#include "program.h"
#include <set>
#include <queue>
#include <string>
#include <map>

Block *createBlock(Json::Value &F, int idx, std::string blockName) {
  Block *B = new Block();
  B->name = blockName;
  while (true) {
    auto &I = F["instrs"][idx];
    if (!I) break;
    if (!I.isMember("op")){
      idx++;
      continue;
    }
    B->instrs.push_back(I);
    if (I["op"].asString() == "br") {
      break;
    } else if (I["op"].asString() == "jmp") {
      break;
    } else if (I["op"].asString() == "ret") {
      break;
    } else {
      idx++;
    }
  }
  return B;
}
Block *makeCFG(Json::Value &F) {
  Block *head = createBlock(F, 0, F["name"].asString());
  std::map<std::string, int> label2Idx;
  std::map<int, Block *> idx2Block;
  idx2Block[0]=head;
  for (int i = 0; i < F["instrs"].size() ; i++) {
    auto &I = F["instrs"][i];
    if (I.isMember("label")){
      label2Idx[I["label"].asString()]=i;
    }
  }
  std::queue<Block *> q;
  q.push(head);
  while(!q.empty()) {
    Block *b = q.front();
    q.pop();
    auto &I = b->instrs[b->instrs.size() - 1];
    if (I["op"].asString() == "br") {
      int idx1 = label2Idx[I["labels"][0].asString()] + 1;
      int idx2 = label2Idx[I["labels"][1].asString()] + 1;
      if (idx2Block.find(idx1) == idx2Block.end()) {
        Block *nb = createBlock(F,idx1, I["labels"][0].asString());
        idx2Block[idx1] = nb;
        b->child.push_back(nb);
        q.push(nb);
      } else {
        b->child.push_back(idx2Block[idx1]);  
      }
      if (idx2Block.find(idx2) == idx2Block.end()) {
        Block *nb = createBlock(F,idx2, I["labels"][1].asString());
        idx2Block[idx2] = nb;
        b->child.push_back(nb);
        q.push(nb);
      } else {
        b->child.push_back(idx2Block[idx2]);
      }
    } else if (I["op"].asString() == "jmp") {
      int idx1 = label2Idx[I["labels"][0].asString()] + 1;
      if (idx2Block.find(idx1) == idx2Block.end()) {
        Block *nb = createBlock(F,idx1, I["labels"][0].asString());
        idx2Block[idx1] = nb;
        b->child.push_back(nb);
        q.push(nb);
      } else {
        b->child.push_back(idx2Block[idx1]);  
      }
    } else {
      continue;
    }
  }
  return head;
}
void printCFG(Block *head) {

  std::set<Block *> visited;
  std::queue<Block *>q;
  if (head){
    visited.insert(head);
    q.push(head);
  }
  while(!q.empty()){
    Block *b = q.front();
    q.pop();
    std::cout<<*b<<std::endl;
    for (auto c : b->child){
      if (!visited.count(c)) {
        visited.insert(c);
        q.push(c);
      }
    }
  }
}

#endif // _CFG_H_
