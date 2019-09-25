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

#include <lotsizing_instance.h>
#include <lotsizing_greedy_brancher.h>

using namespace Gecode;

class LotSizing : public IntMinimizeScript {
 protected:
  const Rnd rnd;
  const InstanceOptions &options;
  const LotSizingInstance instance;

  /** The sequence of orders that are produced:
    array[Periods] of var Orders0: production_by_order; */
  IntVarArray production_by_order;
  /** For each order, the time period in which it is produced:<br>
    array[Orders] of var Periods: production_period; */
  IntVarArray production_period;
  /** the inventory periods that are required for the production plan
      (i.e. the number of periods the order is completed before the due date)
    array[Orders] of var 0..max(due_period): inventory_periods; */
  IntVarArray inventory_periods;
  /** the change cost for changing the machine setup from period p to p+1
    array[1..nb_periods-1] of var 0..max(change_cost): change_cost_for_period; */
  IntVarArray change_cost_for_period;
  /** the order in which orders are produced
    array[Periods] of var Orders0: production_order; */
  IntVarArray production_order; //
  /** the objective: the sum of all costs */
  IntVar objective;

 public:
  /// Branching to use for model
  enum {
    BRANCH_BASE, ///< Use base branching: smallest domain and random value
    BRANCH_GREEDY, ///< Use greedy branching: static var order and pick value with smallest change cost
    BRANCH_GREEDY_DYNAMIC ///< Use dynamic greedy branching: smallest domain, and if previous value set, pick smallest change cost value
  };

  LotSizing(const InstanceOptions &opt);

  // constructor for cloning
  LotSizing(LotSizing &l) : IntMinimizeScript(l), options(l.options), rnd(l.rnd.seed()), instance(l.instance) {
    production_by_order.update(*this, l.production_by_order);
    production_period.update(*this, l.production_period);
    inventory_periods.update(*this, l.inventory_periods);
    change_cost_for_period.update(*this, l.change_cost_for_period);
    production_order.update(*this, l.production_order);
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

};

#endif //LOT_SIZING_PROBLEM_LOTSIZING_H
