/**
 * \brief Greedy branching heuristic for the Discrete Lotsizing Problem
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

/// Print statistics
void
print(const Search::Statistics &stat, bool restart) {
  using namespace std;
  cout << "\t\t\tnodes:      " << stat.node << endl
       << "\t\t\tfailures:   " << stat.fail << endl;
  if (restart)
    cout << "\t\t\trestarts:   " << stat.restart << endl
         << "\t\t\tno-goods:   " << stat.nogood << endl;
  cout << "\t\t\tpeak depth: " << stat.depth << endl;
}


int main(int argc, char **argv) {
  try {
    InstanceOptions opt("LotSizingProblem");
    opt.solutions(0);
    opt.seed(11);
    opt.iterations(10);
    opt.relax(0.7); // destruction rate
    //opt.time(10 * 1000); // timeout in milliseconds
    opt.branching(LotSizing::BRANCH_BASE); // default
    opt.branching(LotSizing::BRANCH_BASE, "base");
    opt.branching(LotSizing::BRANCH_GREEDY, "greedy");
    opt.branching(LotSizing::BRANCH_GREEDY_DYNAMIC, "greedy-dynamic");
    opt.search(LotSizing::SOLVE_EXACT, "exact");
    opt.search(LotSizing::SOLVE_LNS, "lns");
    opt.search(LotSizing::SOLVE_EXACT); // default
    opt.parse(argc, argv);

    if (opt.search() == LotSizing::SOLVE_EXACT) {
      IntMinimizeScript::run<LotSizing, BAB, InstanceOptions>(opt);
    } else { // LNS
      Search::Options so;
      so.stop = Search::Stop::time(opt.time());
      so.cutoff = Search::Cutoff::luby(50);

      int objective = 0;
      LotSizing *lotSizing = new LotSizing(opt);
      RBS<LotSizing, BAB> rbs(lotSizing, so);
      while (LotSizing *s = rbs.next()) {
        s->print(std::cout);
        objective = s->cost().val();
        delete s;
      }
      std::cout << "objective = " << objective << ";\n";
      print(rbs.statistics(), true);
      if (rbs.stopped()) {
        std::cout << "Stopped due to time-out..." << std::endl;
      }
      delete lotSizing;
    }
  }
  catch (const std::runtime_error &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

