#ifndef LOT_SIZING_PROBLEM_LOTSIZING_H
#define LOT_SIZING_PROBLEM_LOTSIZING_H
/**
 * \brief Discrete Lotsizing Problem
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
 *
 */

#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <sstream>
#include <string>
#include <chrono>

#include <lotsizing_instance.h>
#include <static_greedy_brancher.h>
#include <hybrid_greedy_brancher.h>
#include <sdf_greedy_brancher.h>
#include <static_greedy_from_period_brancher.h>

using namespace Gecode;
using namespace std::chrono;

class LotSizing : public IntMinimizeScript {
 protected:
  /// random number generator
  const Rnd rnd;
  /// the solving options
  const InstanceOptions &options;
  /// problem instance/input
  const LotSizingInstance instance;
  /// Relaxation probability for LNS
  double percentage;

  /** The sequence of orders that are produced:
    array[Periods] of var Orders0: production_by_order; */
  IntVarArray production_by_order;
  /** the objective: the sum of all costs */
  IntVar objective;

  /// the start time of when the instance was created through the main constructor
  high_resolution_clock::time_point start;

 public:
  enum {
    MODEL_ONE_COUNT, // use one main count constraint
    MODEL_N_COUNT    // use several count constraints
  };
  /// Branching to use for model
  enum {
    BRANCH_SDF_RANDOM, ///< Base branching: smallest domain and random value
    BRANCH_STATIC_GREEDY, ///< Greedy branching: static var order and pick value with smallest change cost
    BRANCH_HYBRID_GREEDY, ///< Dynamic greedy branching: smallest domain, and if predecessor value set, pick smallest change cost value
    BRANCH_SDF_GREEDY, /// Smallest domain first with greedy value selection
    BRANCH_STATIC_GREEDY_FIXED_PERIOD
  };
  /// Solving approach for model
  enum {
    SOLVE_EXACT,  /// complete search
    SOLVE_LNS     /// large neighbourhood search
  };

  LotSizing(const InstanceOptions &opt);

  // constructor for cloning
  LotSizing(LotSizing &l)
      : IntMinimizeScript(l),
        options(l.options),
        rnd(l.rnd.seed()),
        percentage(l.percentage),
        instance(l.instance),
        start(l.start) {
    production_by_order.update(*this, l.production_by_order);
    objective.update(*this, l.objective);
  }

  virtual Space *copy(void) {
    return new LotSizing(*this);
  }

  virtual IntVar cost(void) const {
    return objective;
  }

  /// Print solution
  virtual void print(std::ostream &os) const;

  /// Slave function for restarts in LNS
  bool slave(const MetaInfo &mi);

};

#endif //LOT_SIZING_PROBLEM_LOTSIZING_H
