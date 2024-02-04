#include <fstream>
#include <iostream>
#include <stdio.h>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/json.h>
#include "program.h"
#include "cfg.h"
#include "tdce.h"
#include "lvn.h"

int main(int argc, char **argv) {
  // std::ifstream fin(argv[1], std::ifstream::binary);
  Json::Reader reader;
  Json::Value root;
  reader.parse(std::cin, root);
  // std::cout<<root<<std::endl;
  std::vector<Block *> blockHeads;
  for (auto &func : root["functions"]) {
    Block *B = makeCFG(func);
    blockHeads.push_back(B);
    printCFG(B);
  }
  for (auto *head : blockHeads) {
    lvn(head);
    tdce(head);
    printCFG(head);
  }
  return 0;
}
