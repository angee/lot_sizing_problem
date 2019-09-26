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

int main(int argc, char **argv) {
  try {
    InstanceOptions opt("LotSizingProblem");
    opt.solutions(0);
    opt.seed(11);
    opt.iterations(10);
    opt.relax(0.7); // destruction rate
    //opt.time(10 * 1000); // timeout in milliseconds
    opt.branching(LotSizing::BRANCH_BASE);
    opt.branching(LotSizing::BRANCH_BASE, "base");
    opt.branching(LotSizing::BRANCH_GREEDY, "greedy");
    opt.branching(LotSizing::BRANCH_GREEDY_DYNAMIC, "greedy-dynamic");
    opt.parse(argc, argv);

    IntMinimizeScript::run<LotSizing, BAB, InstanceOptions>(opt);
  }
  catch (const std::runtime_error &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

