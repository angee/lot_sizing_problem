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

class GreedyBranching : public Brancher {
  /// Views for the the production order
  ViewArray<Int::IntView> production_by_order;
  /// the problem instance
  const LotSizingInstance instance;
  /// Next view to branch on (the next period in the production order)
  mutable int current_period;
  // the orders that can be assigned in the current period
  mutable std::vector<int> potential_orders;

  class Choice : public Gecode::Choice {
   public:
    /// period: the position in production order array
    int period;
    /// order: the value assigned to the element of the variable array
    int order;
    /** Initialize choice for brancher \a b, position \a pos (period),
     *  and value \a val (order).
     */
    Choice(const Brancher &b, int pos, int val)
        : Gecode::Choice(b, 2), period(pos), order(val) {}

    /// Archive into \a e
    virtual void archive(Archive &e) const {
      Gecode::Choice::archive(e);
      e << period << order;
    }
  };

 public:
  GreedyBranching(Home home, ViewArray<Int::IntView> &production, const LotSizingInstance instance0) :
      Brancher(home), production_by_order(production), instance(instance0), current_period(0) {
    // at first, all orders, including idle time, is possible (TODO: order by due period)
    for (int order = -1; order < instance.getOrders(); order++) {
      potential_orders.push_back(order);
    }
  }
  // copy constructor
  GreedyBranching(Space &home, GreedyBranching &gb) : Brancher(home, gb),
                                                      production_by_order(gb.production_by_order),
                                                      instance(gb.instance), current_period(gb.current_period) {
    production_by_order.update(home, gb.production_by_order);
  }

  static void post(Home home, ViewArray<Int::IntView> &p, const LotSizingInstance instance) {
    (void) new(home) GreedyBranching(home, p, instance);
  }

  /// Copy brancher
  virtual Actor *copy(Space &home) {
    return new(home) GreedyBranching(home, *this);
  }

  /// Check status of brancher, return true if alternatives left
  virtual bool status(const Space &) const {
    if (!potential_orders.empty()) {
      return true;
    }
    for (int period = current_period; period < production_by_order.size(); period++) {
      if (!production_by_order[period].assigned()) {
        current_period = period;
        if (period == 0) { // initialise the orders, TODO: order by due period
          for (int order = -1; order < instance.getOrders(); order++) {
            potential_orders.push_back(order);
          }
        } else {
          // since space is not yet failed, there must be some possible order values in the list
          potential_orders =
              instance.getOrdersDueAfterPeriodOrderedByChangeCost(current_period,
                                                                  production_by_order[period - 1].val());
        }
        // remove all order values that are not in the variable's domain
        auto it = potential_orders.begin();
        while (it != potential_orders.end()) {
          if (!production_by_order[current_period].in(*it)) {
            it = potential_orders.erase(it);
          } else it++;
        }
        return true;
      }
    }
    return false;
  }

  /// Return choice
  virtual Gecode::Choice *choice(Space &) {
    int order = potential_orders[0];
    // remove the order that was just chosen
    auto it = potential_orders.begin();
    potential_orders.erase(it);
    return new Choice(*this, current_period, order);
  }

  /// Return choice
  virtual const Gecode::Choice *choice(const Space &, Archive &e) {
    int pos, val;
    e >> pos >> val;
    return new Choice(*this, pos, val);
  }

  /// Perform commit for choice \a _c and alternative \a a
  virtual ExecStatus commit(Space &home, const Gecode::Choice &_c,
                            unsigned int a) {
    const Choice &c = static_cast<const Choice &>(_c);
    if (a == 0)
      return me_failed(production_by_order[c.period].eq(home, c.order)) ? ES_FAILED : ES_OK;
    else
      return me_failed(production_by_order[c.period].nq(home, c.order)) ? ES_FAILED : ES_OK;
  }

  /// Print explanation
  virtual void print(const Space &, const Gecode::Choice &_c,
                     unsigned int a,
                     std::ostream &o) const {
    const Choice &c = static_cast<const Choice &>(_c);
    o << "production_by_order[" << c.period << "] "
      << ((a == 0) ? "=" : "!=")
      << " " << c.order;
  }
};

/// Post branching (assumes that \a s is sorted)
void greedyBranching(Home home, const IntVarArgs &production_by_order, const LotSizingInstance &instance) {
  ViewArray<Int::IntView> production(home, production_by_order);
  return GreedyBranching::post(home, production, instance);
}

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
  LotSizing(const InstanceOptions &opt)
      : IntMinimizeScript(opt), options(opt), rnd(3),
      // read the instance
        instance((LotSizingInstanceReader(opt.instance())).generateInstance()),
      // initialise the variable arrays
        production_by_order(*this, instance.getPeriods(), -1, instance.getOrders() - 1),
        production_period(*this, instance.getOrders(), 0, instance.getPeriods() - 1),
        inventory_periods(*this, instance.getOrders(), 0, instance.calculateMaxDuePeriod()),
        change_cost_for_period(*this, instance.getPeriods() - 1, 0, instance.calculateMaxChangeCost()),
        production_order(*this, instance.getPeriods(), 0, instance.getOrders()),
        objective(*this, 0, instance.calculateUpperBoundForObjective()) {
    // print the instance for debug reasons
    instance.print();

    /// CONSTRAINTS

    // (1) the number of times each order is produced
//    // VERSION-1
//    IntSetArgs occurrence(instance.getOrders()+1);
//    int num_idle_periods = instance.getPeriods() - instance.getOrders();
//    occurrence[0] = IntSet({num_idle_periods});
//    for(unsigned order = 1; order <= instance.getOrders(); order++) {
//      occurrence[order] = IntSet({1});
//    }
//    count(*this, production_by_order, occurrence, IntArgs::create(instance.getOrders()+1, -1));

    // VERSION-2
    for (unsigned order = 0; order < instance.getOrders(); order++) {
      count(*this, production_by_order, order, IRT_EQ, 1); // each order is produced exactly once
    }
    count(*this, production_by_order, -1, IRT_EQ, instance.getPeriods() - instance.getOrders()); // idle times

    // (2) Don't produce the order AFTER its due date
    for (unsigned order = 0; order < instance.getOrders(); order++) {
      for (unsigned period = 0; period < instance.getPeriods(); period++) {
        if (instance.getDuePeriod(order) < period) {
          // production_by_order[period] != order
          rel(*this, production_by_order[period], IRT_NQ, order);
        }
      }
    }

    // (3) Linking the production_period variables with the production_by_order variables
    for (unsigned order = 0; order < instance.getOrders(); order++) {
      // production_by_order[production_period[order]] = order
      element(*this, production_by_order, production_period[order], order);
    }

    // (4) Redundant constraints: alldiff(production_period)
    //distinct(*this, production_period);

    // (5) sets the number of periods that inventory is necessary for each order
    for (unsigned order = 0; order < instance.getOrders(); order++) {
      // inventory_periods[order] = due_period[order] - production_period[order]
      rel(*this, inventory_periods[order] == (instance.getDuePeriod(order) - production_period[order]));
    }

    // (6) set "production_order" to the order in which items are produced. We will use these variables
    //     to impose the change_cost constraints

    // binary helper variable that is 1 when production is zero
    BoolVarArgs productionIsZero(*this, instance.getPeriods() - 1, 0, 1);
    for (unsigned period = 0; period < instance.getPeriods() - 1; period++) {
      // (production_is_zero) <-> production_by_order == -1
      Reify r(productionIsZero[period]);
      rel(*this, production_by_order[period + 1], IRT_EQ, -1, r);
    }
    rel(*this, production_order[0] == production_by_order[0] + 1); // +1: we need production_order for array access
    for (unsigned period = 1; period < instance.getPeriods(); period++) {
      // production_is_zero[p] == 1 --> production_order[p] == production_order[p-1]
      rel(*this, productionIsZero[period - 1] >> (production_order[period] == production_order[period - 1]));
      // production_is_zero[p] == 0 --> production_order[p] == production_by_order[p]
      rel(*this, (!productionIsZero[period - 1]) >> (production_order[period] == production_by_order[period] + 1));
    }

    // (7) change cost constraint
    // shared array for element constraint
    int nb_entries = instance.getOrders() + 1;
    IntSharedArray change_cost_as_array(nb_entries * nb_entries);
    for (unsigned i = 0; i < nb_entries; i++) {
      for (unsigned j = 0; j < nb_entries; j++) {
        if (i == 0 || j == 0) { // these are entries for "-1" -> no production
          change_cost_as_array[i * nb_entries + j] = 0;
        } else {
          change_cost_as_array[i * nb_entries + j] = instance.getChangeCosts(i - 1, j - 1);
        }
      }
    }

    for (unsigned period = 0; period < instance.getPeriods() - 1; period++) {
      // change_cost_for_period[p] = change_cost[production_order[p], production_order[p+1]]
      element(*this,
              change_cost_as_array,
              production_order[period + 1], // column index
              nb_entries,
              production_order[period], // row index
              nb_entries,
              change_cost_for_period[period]);
    }

    // (8) breaking symmetry: complete orders of same type in a fixed order (the ones first are produced first)
    for (unsigned item_type = 0; item_type < instance.getTypes(); item_type++) {
      int num_orders_per_type = instance.getNumberOfOrders(item_type);
      if (num_orders_per_type > 1) {
        for (unsigned k = 0; k < num_orders_per_type - 1; k++) {
          // production_period[order_number(item_type, k)] < production_period[order_number(item_type, k+1)]
          rel(*this,
              production_period[instance.getOrderNumber(item_type, k)],
              IRT_LE,
              production_period[instance.getOrderNumber(item_type, k + 1)]);
        }
      }
    }


    // (9) objective = sum(p in 1..nb_periods-1) (change_cost_for_period[p])
    //                       + sum(o in Orders) (inventory_periods[o]) * inventory_cost;
    // setting up the variables of the linear expression
    IntVarArgs x(instance.getPeriods() - 1 + instance.getOrders());
    IntArgs a(instance.getPeriods() - 1 + instance.getOrders());
    for (unsigned p = 0; p < instance.getPeriods() - 1; p++) {
      x[p] = change_cost_for_period[p];
      a[p] = 1;
    }
    for (unsigned o = 0; o < instance.getOrders(); o++) {
      x[o + instance.getPeriods() - 1] = inventory_periods[o];
      a[o + instance.getPeriods() - 1] = instance.getInventoryCost();
    }
    linear(*this, a, x, IRT_EQ, objective);

    // branching instructions
    //branch(*this, production_by_order, INT_VAR_SIZE_MIN(), INT_VAL_RND(rnd));
    greedyBranching(*this, production_by_order, instance);
  }

  // constructor for cloning
  LotSizing(LotSizing &l) : IntMinimizeScript(l), options(l.options), rnd(l.rnd.seed()), instance(l.instance) {
    //std::cout << "Cloning...\n";
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
  virtual void print(std::ostream &os) const {
    os << "production_by_order = [";
    for (unsigned period = 0; period < instance.getPeriods(); period++) {
      os << production_by_order[period].val() << ", ";
    }
    os << "];\n";
    os << "production_period = [";
    for (unsigned order = 0; order < instance.getOrders(); order++) {
      os << production_period[order].val() << ", ";
    }
    os << "];\n";
    os << "inventory_periods = [";
    for (unsigned order = 0; order < instance.getOrders(); order++) {
      os << inventory_periods[order].val() << ", ";
    }
    os << "];\n";
    os << "production_order = [";
    for (unsigned period = 0; period < instance.getPeriods(); period++) {
      os << production_order[period].val() << ", ";
    }
    os << "];\n";
    os << "change_cost = [";
    for (unsigned period = 0; period < instance.getPeriods() - 1; period++) {
      os << change_cost_for_period[period].val() << ", ";
    }
    os << "];\n";
    os << "production = [";
    for (unsigned period = 0; period < instance.getPeriods(); period++) {
      int order = production_by_order[period].val();
      if (order != -1) {
        os << instance.getTypeOfOrder(order) << ", ";
      } else {
        os << order << ", ";
      }
    }
    os << "]\n";
    os << "objective = " << objective.val() << "\n--------------------------------------------\n";
  }

};

