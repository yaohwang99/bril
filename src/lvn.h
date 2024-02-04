#ifndef _LVN_H_
#define _LVN_H_
#include <set>
#include <queue>
#include <map>
#include <vector>
#include <unordered_map>
#include <utility>
#include "program.h"

void lvn(Block *head) {
  std::set<Block *> visited;
  std::queue<Block *> q;
  if (head) {
    visited.insert(head);
    q.push(head);
  }
  while (!q.empty()) {
    Block *b = q.front();
    q.pop();
    std::vector<std::string> num2Var; // table
    std::map<std::pair<std::string, std::vector<int>>, int> value2Num;
    std::unordered_map<std::string, int> var2Num;
    for (auto &I : b->instrs) {
      auto value = std::pair<std::string, std::vector<int>>({I["op"].asString(), std::vector<int>()});
      if (I.isMember("args")) {
        for (auto &V : I["args"]) {
          value.second.push_back(var2Num[V.asString()]);
        }
      }
      if (I.isMember("value")) { // I["op"] is const
        value.second.push_back(I["value"].asInt());
      }
      if (value2Num.find(value) != value2Num.end()) { // value in table
        Json::Value dest = I["dest"];
        var2Num[I["dest"].asString()] = value2Num[value];
        // reconstruct I
        I = Json::Value(Json::objectValue);
        I["op"] = Json::Value("id");
        I["dest"] = dest;
        I["args"] = Json::Value(Json::arrayValue);
        I["args"].append(Json::Value(num2Var[value2Num[value]]));
      } else { // value not in table
        if (I.isMember("dest")) {
          int newNum = num2Var.size();
          var2Num[I["dest"].asString()] = newNum;
          num2Var.push_back(I["dest"].asString());
          value2Num[value] = newNum;
        }
        if (I["op"].asString() == "const")
          continue;
        // reconstruct I
        for(int i = 0; i < value.second.size(); i++){
          I["args"][i] = num2Var[value.second[i]];
        }
      }
    }
    for (auto c : b->child) {
      if (!visited.count(c)) {
        visited.insert(c);
        q.push(c);
      }
    }
  }
}
#endif
