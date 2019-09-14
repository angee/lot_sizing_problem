#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <fstream>
#include <sstream>
#include <string>

#include "lotsizing_instance.h"

using namespace Gecode;

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
  IntVarArray production_order; // FIXME: can it be an IntVarArgs array?
  /** the objective: the sum of all costs */
  IntVar objective;

 public:
  LotSizing(const InstanceOptions &opt)
      : IntMinimizeScript(opt), options(opt), rnd(11),
      // read the instance
        instance((LotSizingInstanceReader(opt.instance())).generateInstance()),
      // initialise the variable arrays
        production_by_order(*this, instance.getPeriods(), 0, instance.getOrders()),
        production_period(*this, instance.getOrders(), 0, instance.getPeriods() - 1),
        inventory_periods(*this, instance.getOrders(), 0, instance.calculateMaxDuePeriod()),
        change_cost_for_period(*this, instance.getPeriods() - 1, 0, instance.calculateMaxChangeCost()),
        production_order(*this, instance.getPeriods(), 0, instance.getOrders()),
        objective(*this, 0, instance.calculateUpperBoundForObjective()) {
    // print the instance for debug reasons
    instance.print();

    /// CONSTRAINTS

    // (1) the number of times each order is produced
    for(unsigned order = 1; order <= instance.getOrders(); order++) {
      count(*this, production_by_order, order, IRT_EQ, 1); // each order is produced exactly once
    }
    count(*this, production_by_order, 0, IRT_EQ, instance.getPeriods() - instance.getOrders()); // idle times

    // (2) Don't produce the order AFTER its due date
    for(unsigned order = 0; order < instance.getOrders(); order++) {
      for(unsigned period = 0; period < instance.getPeriods(); period++) {
        if(instance.getDuePeriod(order) < period) {
          // production_by_order[period] != order
          rel(*this, production_by_order[period] != order);
        }
      }
    }

    // (3) Linking the production_period variables with the production_by_order variables
    for(unsigned order = 1; order <= instance.getOrders(); order++) {
      // production_by_order[production_period[order]] = order
      element(*this, production_by_order, production_period[order-1], order);
    }

    // (4) Redundant constraints: alldiff(production_period)
    distinct(*this, production_period);

    // (5) sets the number of periods that inventory is necessary for each order
    for(unsigned order = 0; order < instance.getOrders(); order++) {
      // inventory_periods[order] = due_period[order] - production_period[order]
      rel(*this, inventory_periods[order] == instance.getDuePeriod(order) - production_period[order]);
    }

    // TODO: problem model

    // branching instructions
    branch(*this, production_by_order, INT_VAR_SIZE_MIN(), INT_VAL_RND(rnd));
  }

  // constructor for cloning
  LotSizing(LotSizing &l) : IntMinimizeScript(l), options(l.options), rnd(l.rnd.seed()), instance(l.instance) {
    production_by_order.update(*this, l.production_by_order);
    production_period.update(*this, l.production_period);
    inventory_periods.update(*this, l.inventory_periods);
    change_cost_for_period.update(*this, l.change_cost_for_period);
    production_order.update(*this, production_order);
    objective.update(*this, l.objective);
  }

  virtual Space *copy(void) {
    return new LotSizing(*this);
  }

  virtual IntVar cost(void) const {
    return objective;
  }

  /// Print solution
  virtual void print(std::ostream& os) const {
    os << "production_order = [";
    for(unsigned period = 0; period < instance.getPeriods(); period++) {
      os << production_by_order[period].val() << ", ";
    }
    os << "];\n";
    os << "production_period = [";
    for(unsigned order = 0; order < instance.getOrders(); order++) {
      os << production_period[order].val() << ", ";
    }
    os << "];\n";
    os << "inventory_periods = [";
    for(unsigned order = 0; order < instance.getOrders(); order++) {
      os << inventory_periods[order].val() << ", ";
    }
    os << "];\n";
    //os << "objective = " << objective.val() << "\n";
  }

};

