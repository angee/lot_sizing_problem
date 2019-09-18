#include <iostream>
#include <gecode/driver.hh>
#include "lotsizing.cpp"

using namespace Gecode;

char *parseInput(int argc, char **argv) {
  if (argc != 2) {
    throw std::runtime_error("Expecting exactly one input, the problem file path.");
  }
  return argv[1];
}

int main(int argc, char **argv) {
  try {
    InstanceOptions opt("LotSizingProblem");
    opt.instance(parseInput(argc, argv));
    opt.solutions(0);

    IntMinimizeScript::run<LotSizing, BAB, InstanceOptions>(opt);
    return 0;
  }
  catch (const std::runtime_error &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

