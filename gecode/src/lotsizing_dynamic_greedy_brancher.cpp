#include <lotsizing_dynamic_greedy_brancher.h>

bool DynamicGreedyBranching::status(const Space &) const {
  // we still have options for the current period
  if (!potential_orders.empty()) {
    return true;
  }
  // else: we need to pick a new period. We want to pick the one with the smallest domain size. Tie: prefer the one whose
  // previous period is already set, if applicable, otherwise random.
  unsigned int smallestDomainPeriod = -1;
  unsigned minDomainSize = instance.getOrders() + 1;
  bool previousPeriodIsAssigned =
      false;  // set to true if for the smallest domain period the predecessor period is assigned
  for (int period = 0; period < production_by_order.size(); period++) {
    if (!production_by_order[period].assigned()) {
      unsigned domainSize = production_by_order[period].size();
      if (domainSize < minDomainSize) {
        minDomainSize = domainSize;
        smallestDomainPeriod = period;
        previousPeriodIsAssigned = (period == 0) ? false : production_by_order[period - 1].assigned();
      }
        // if we encounter another period with the same domain size, prefer the one whoes predecessor is assigned
        // (for calculating the change costs). Note that this will prefer smallest domains in the later part of the
        // production order. Change to prefer smallest domains in the early part of the production order
      else if (domainSize == minDomainSize) {
        if (period > 0 && production_by_order[period - 1].assigned()) {
          previousPeriodIsAssigned = true;
          smallestDomainPeriod = period;
        }
      }
    }
  }

  // all variables are assigned -> there is nothing left to do
  if (smallestDomainPeriod == -1) {
    return false;
  }

  current_period = smallestDomainPeriod;
  // We found a variables with a smallest domain siez and whose predecessor has already been assigned. This means that
  // we will apply the greedy approach in selecting values, based on the change costs from previous to this period.
  if (previousPeriodIsAssigned) {
    if (current_period == 0) { // initialise the orders
      //for (int order = -1; order < instance.getOrders(); order++) {
      //  potential_orders.push_back(order);
      //}
      potential_orders = instance.getOrdersDueAfterPeriodOrderedByDuePeriod(current_period);
    } else {
      // since space is not yet failed, there must be some possible order values in the list
      potential_orders =
          instance.getOrdersDueAfterPeriodOrderedByChangeCost(current_period,
                                                              production_by_order[current_period - 1].val());
    }

  }
    // apply a different approach for selecting values. We don't know the previous order
  else {
    potential_orders = instance.getOrdersDueAfterPeriodOrderedByDuePeriod(current_period);
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
