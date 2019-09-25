#ifndef LOT_SIZING_PROBLEM_LOTSIZING_GREEDY_BRANCHER_H
#define LOT_SIZING_PROBLEM_LOTSIZING_GREEDY_BRANCHER_H

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
 *
 */

#include <gecode/int.hh>
#include "lotsizing_instance.h"

using namespace Gecode;

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
  virtual bool status(const Space &) const;

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
    o << "prod[" << c.period << "] "
      << ((a == 0) ? "=" : "!=")
      << " " << c.order;
  }
};

#endif //LOT_SIZING_PROBLEM_LOTSIZING_GREEDY_BRANCHER_H
