/**
 * @authors Andrea Rendl, Satalia
 */
#include <static_greedy_from_period_brancher.h>

using namespace Gecode;

bool StaticGreedyFromPeriodBranching::status(const Gecode::Space &home) const {
  if (!potential_orders.empty()) {
    return true;
  }
  if (current_period == production_by_order.size() - 1) { // we reached the end -> go to the start now
    current_period = 0;
  }

  for (int period = current_period; period < production_by_order.size(); period++) {

    if (!production_by_order[period].assigned()) {
      current_period = period;
      if (period == 0 || !production_by_order[period - 1].assigned()) { // initialise the orders, ordered by due date
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
    } else {
      if (period == production_by_order.size() - 1 && !allVarsAreAssigned()) {
        period = -1; // go again to the start
      } else if (period == start_period - 1 && allVarsAreAssigned()) {
        return false; // we reached the end of our search
      }
    }
  }
  return false;
}

bool StaticGreedyFromPeriodBranching::allVarsAreAssigned() const {
  for (auto period : production_by_order) {
    if (!period.assigned())
      return false;
  }
  return true;
}
