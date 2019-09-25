#include <iostream>
#include <gecode/driver.hh>
#include "lotsizing.cpp"

using namespace Gecode;

int main(int argc, char **argv) {
  try {
    InstanceOptions opt("LotSizingProblem");
    opt.solutions(0);
    opt.branching(LotSizing::BRANCH_BASE);
    opt.branching(LotSizing::BRANCH_BASE, "base");
    opt.branching(LotSizing::BRANCH_GREEDY, "greedy");
    //opt.branching(LotSizing::BRANCH_GREEDY_DYNAMIC, "greedy-dynamic");
    opt.parse(argc, argv);

    IntMinimizeScript::run<LotSizing, BAB, InstanceOptions>(opt);
    return 0;
  }
  catch (const std::runtime_error &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

