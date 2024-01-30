#ifndef _PROGRAM_H_
#define _PROGRAM_H_
#include<utility>
#include<set>
#include<queue>
#include<string>
#include<vector>
//class Type {
//  public:
//    std::string name;
//    Type *next;
//  Type(){
//    name="";
//    next=nullptr;
//  }
//  Type(std::string name) {
//    name = name;
//    next=nullptr;
//  }
//};
//class Label {
//  public:
//    std::string name;
//};
//class Instruction {
//  public:
//    std::string name;
//};
class Block {
  public:
    std::string name;
    std::vector<Json::Value> instrs;
    std::vector<Block *> child;
};
class Function {
  public:
    //std::string name;
    //std::vector<std::pair<std::string, Type>> args;
    //Type type;
    Json::Value value;
    Block *firstBlock;
};
class Program {
  public:
    Json::Value value;
    std::vector<Function *> functions;
};

//std::ostream &operator<<(std::ostream &os, const Label &label) {
//  os<<"Label: "<<label.name<<std::endl;
//  return os;
//}
//std::ostream &operator<<(std::ostream &os, const Instruction &instruction) {
//  os<<"Instruction: "<<instruction.name<<std::endl;
//  return os;
//}
std::ostream &operator<<(std::ostream &os, const Block &block) {
  os<<"Block: "<<block.name<<std::endl;
  for (auto &i : block.instrs) {
    os<<i<<std::endl;
  }
  return os;
}
//std::ostream &operator<<(std::ostream &os, const Type &type) {
//  os<<"Type: "<<type.name;
//  if (type.next) {
//    os<<*type.next;
//  } else {
//    os<<std::endl;
//  }
//  return os;
//}
std::ostream &operator<<(std::ostream &os, const Function &f) {
  os<<"Function: "<<f.value["name"]<<std::endl;
  for (auto &arg : f.value["args"]){
    os<<"args: "<< "name: "<<arg["name"]<<" type: "<<arg["type"]<<std::endl;
  }
  std::set<Block *> visited;
  std::queue<Block *>q;
  if (f.firstBlock){
    visited.insert(f.firstBlock);
    q.push(f.firstBlock);
  }
  while(!q.empty()){
    Block *b = q.front();
    q.pop();
    os<<*b<<std::endl;
    for (auto c : b->child){
      if (!visited.count(c)) {
        visited.insert(c);
        q.push(c);
      }
    }
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, const Program &program) {
  os<<"program: "<<std::endl;
  for (auto *f : program.functions) {
    os<<*f<<std::endl;
  }
  return os;
}

#endif // _PROGRAM_H_
