#ifndef _CFG_H_
#define _CFG_H_
#include "program.h"
#include <set>
#include <unordered_set>
#include <queue>
#include <string>
#include <map>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/json.h>

Block *createBlock(Json::Value &F, int labelIdx, std::string blockName) {
  Block *B = new Block();
  B->name = blockName;
  auto idx = labelIdx + 1;
  while (true) {
    auto &I = F["instrs"][idx];
    if (!I || I.isMember("label")) break;
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
  Block *head = createBlock(F, -1, "");
  std::map<std::string, Block *> label2Block;
  std::map<int, Block *> idx2Block;
  std::map<Block *, std::string> block2Label;
  std::map<Block *, int> block2Idx;
  label2Block[""] = head;
  block2Label[head] = "";
  idx2Block[-1] = head;
  block2Idx[head] = -1;
  for (int i = 0; i < F["instrs"].size() ; i++) {
    auto &I = F["instrs"][i];
    if (I.isMember("label")){
      auto *b = createBlock(F, i, I["label"].asString());
      label2Block[I["label"].asString()] = b;
      idx2Block[i] = b;
      block2Idx[b] = i;
      block2Label[b] = I["label"].asString();
    }
  }
  std::queue<Block *> q;
  std::unordered_set<Block *> visited;
  visited.insert(head);
  q.push(head);
  while(!q.empty()) {
    Block *b = q.front();
    q.pop();
    if (b->instrs.size() == 0) { // dummy block
      auto nextIdx = block2Idx[b] + 1;
      if (idx2Block.find(nextIdx) != idx2Block.end()) {
        auto *c = idx2Block[nextIdx];
        b->child.push_back(c);
        if (!visited.count(c)) {
          visited.insert(c);
          q.push(c);
        }
      }
    } else {
      auto &I = b->instrs[b->instrs.size() - 1];
      if (I["op"].asString() == "jmp" || I["op"].asString() == "br") {
        auto *c = label2Block[I["labels"][0].asString()];
        b->child.push_back(c);  
        if (!visited.count(c)) {
          visited.insert(c);
          q.push(c);
        }
        if (I["op"].asString() == "br") {
          auto *c = label2Block[I["labels"][1].asString()];
          b->child.push_back(c);  
          if (!visited.count(c)) {
            visited.insert(c);
            q.push(c);
          }
        }
      } else if (I["op"].asString() != "ret") { 
        auto nextIdx = block2Idx[b] + b->instrs.size() + 1;
        if (idx2Block.find(nextIdx) != idx2Block.end()){
          auto *c = idx2Block[nextIdx];
          b->child.push_back(c);
          if (!visited.count(c)) {
            visited.insert(c);
            q.push(c);
          }
        }
      }
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
