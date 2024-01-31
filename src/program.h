#ifndef _PROGRAM_H_
#define _PROGRAM_H_
#include<utility>
#include<set>
#include<queue>
#include<string>
#include<vector>
class Block {
  public:
    std::string name;
    std::vector<Json::Value> instrs;
    std::vector<Block *> child;
};

std::ostream &operator<<(std::ostream &os, const Block &block) {
  os<<"Block: "<<block.name<<std::endl;
  for (auto &i : block.instrs) {
    os<<i<<std::endl;
  }
  return os;
}

#endif // _PROGRAM_H_
