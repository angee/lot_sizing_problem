/**
 * @authors Andrea Rendl, Satalia
 */
#include <static_greedy_brancher.h>

using namespace Gecode;

bool StaticGreedyBranching::status(const Gecode::Space &home) const {
  if (!potential_orders.empty()) {
    return true;
  }
  for (int period = current_period; period < production_by_order.size(); period++) {
    if (!production_by_order[period].assigned()) {
      current_period = period;
      if (period == 0) { // initialise the orders, ordered by due date
        potential_orders = instance.getOrdersDueAfterPeriodOrderedByDuePeriod(period);
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
