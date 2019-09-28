/**
 * \brief Solving the Discrete Lotsizing Problem
 *
 * CSPlib Problem 58: http://www.csplib.org/Problems/prob058/
 *  MIT License
 *
 * Copyright (c) 2019 Andrea Rendl-Pitrey, Satalia
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Author: Andrea Rendl, September 2019
 */

#include <iostream>
#include <gecode/driver.hh>
#include <lotsizing.h>

using namespace Gecode;

/// Print LNS statistics
void
print(const Search::Statistics &stat) {
  using namespace std;
  cout << "\tpropagations:      " << stat.propagate << endl
       << "\tnodes:      " << stat.node << endl
       << "\tfailures:   " << stat.fail << endl
       << "\trestarts:   " << stat.restart << endl
       << "\tno-goods:   " << stat.nogood << endl
       << "\tpeak depth: " << stat.depth << endl;
}

/// Runs Large Neighbourhood search using the restart meta-engine
void runLNS(const InstanceOptions &opt) {
  Search::Options so;
  so.stop = Search::Stop::time(opt.time()); // timeout in milliseconds
  so.cutoff = Search::Cutoff::luby(50);

  LotSizing *lotSizing = new LotSizing(opt);
  RBS<LotSizing, BAB> rbs(lotSizing, so);
  while (LotSizing *s = rbs.next()) {
    s->print(std::cout); // print the solution
    delete s;
  }
  print(rbs.statistics());
  delete lotSizing;
}

int main(int argc, char **argv) {
  try {
    InstanceOptions opt("LotSizingProblem");
    opt.solutions(0); // all solutions
    opt.seed(11); //default seed
    opt.relax(0.7); // default destruction rate
    opt.branching(LotSizing::BRANCH_SDF_RANDOM); // default
    opt.branching(LotSizing::BRANCH_SDF_RANDOM, "sdf-random");
    opt.branching(LotSizing::BRANCH_STATIC_GREEDY, "static-greedy");
    opt.branching(LotSizing::BRANCH_STATIC_GREEDY_FIXED_PERIOD, "static-greedy-period");
    opt.branching(LotSizing::BRANCH_HYBRID_GREEDY, "hybrid-greedy");
    opt.branching(LotSizing::BRANCH_SDF_GREEDY, "sdf-greedy");
    opt.search(LotSizing::SOLVE_EXACT); // default
    opt.search(LotSizing::SOLVE_EXACT, "exact");
    opt.search(LotSizing::SOLVE_LNS, "lns");
    opt.parse(argc, argv);

    if (opt.search() == LotSizing::SOLVE_EXACT) {
      IntMinimizeScript::run<LotSizing, BAB, InstanceOptions>(opt);
    } else { // else: SOLVE_LNS
      runLNS(opt);
    }
  }
  catch (const std::runtime_error &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

