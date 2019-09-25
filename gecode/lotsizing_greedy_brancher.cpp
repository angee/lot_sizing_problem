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
#include "lotsizing_greedy_brancher.h";

using namespace Gecode;

bool GreedyBranching::status(const Gecode::Space &home) const {
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
