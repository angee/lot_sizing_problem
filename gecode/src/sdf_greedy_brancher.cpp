//
// Created by andrea on 28.09.19.
//

#include <sdf_greedy_brancher.h>

Gecode::Choice *SdfGreedyBranching::choice(Space &) {
  int current_period = findPeriodWithSmallestBestDomain();
  std::vector<int> potential_orders;
  // if the predecessor is assigned, we select orders based on the change costs from predecessor to this period.
  if (current_period > 0 && production_by_order[current_period - 1].assigned()) {
    // since space is not yet failed, there must be some possible order values in the list
    potential_orders =
        instance.getOrdersDueAfterPeriodOrderedByChangeCost(current_period,
                                                            production_by_order[current_period - 1].val());
  } else {
    potential_orders = instance.getOrdersDueAfterPeriodOrderedByDuePeriod(current_period);
  }

  // remove all order values that are not in the variable's domain
  auto it = potential_orders.begin();
  while (it != potential_orders.end()) {
    if (!production_by_order[current_period].in(*it)) {
      it = potential_orders.erase(it);
    } else it++;
  }
  int order = potential_orders[0]; // pick the first (best) order
  return new Choice(*this, current_period, order);
}

bool SdfGreedyBranching::status(const Space &) const {
  for (auto period : production_by_order) {
    if (!period.assigned()) {
      return true;
    }
  }
  return false;
}

int SdfGreedyBranching::findPeriodWithSmallestBestDomain() const {
  // We want to pick the period with the smallest domain size. Tie: prefer the one whose
  // predecessor period is already set, if applicable, otherwise random.
  int smallestDomainPeriod = -1;
  unsigned minDomainSize = instance.getOrders() + 2;
  for (int period = 0; period < production_by_order.size(); period++) {
    if (!production_by_order[period].assigned()) {
      unsigned domainSize = production_by_order[period].size();
      if (domainSize < minDomainSize) {
        minDomainSize = domainSize;
        smallestDomainPeriod = period;
      }
        // if we encounter another period with the same domain size, prefer the one whose predecessor is assigned
        // (for calculating the change costs).
      else if (domainSize == minDomainSize) {
        if (period > 0 && smallestDomainPeriod > 0
            && production_by_order[period - 1].assigned()) {
          smallestDomainPeriod = period;
        }
      }
    }
  }
  return smallestDomainPeriod;
}