/**
 * @authors Andrea Rendl, Satalia
 */
#include <lotsizing.h>

LotSizing::LotSizing(const InstanceOptions &opt)
    : IntMinimizeScript(opt), options(opt), rnd(opt.seed()), percentage(opt.relax()),
    // read the instance
      instance((LotSizingInstanceReader(opt.instance())).generateInstance()),
    // initialise the variable arrays
      production_by_order(*this, instance.getPeriods(), -1, instance.getOrders() - 1),
      objective(*this, 0, instance.calculateUpperBoundForObjective()) {

  start = std::chrono::high_resolution_clock::now(); // start counting time
  //instance.print(); // print the instance for debug reasons

  // HELPER VARIABLES

  /** For each order, the time period in which it is produced:<br>
    array[Orders] of var Periods: production_period; */
  IntVarArgs production_period(*this, instance.getOrders(), 0, instance.getPeriods() - 1);
  /** the inventory periods that are required for the production plan
      (i.e. the number of periods the order is completed before the due date)
    array[Orders] of var 0..max(due_period): inventory_periods; */
  IntVarArgs inventory_periods(*this, instance.getOrders(), 0, instance.calculateMaxDuePeriod());
  /** the change cost for changing the machine setup from period p to p+1
    array[1..nb_periods-1] of var 0..max(change_cost): change_cost_for_period; */
  IntVarArgs change_cost_for_period(*this, instance.getPeriods() - 1, 0, instance.calculateMaxChangeCost());
  /** the order in which orders are produced
    array[Periods] of var Orders0: production_order; */
  IntVarArgs production_order(*this, instance.getPeriods(), 0, instance.getOrders());

  /// CONSTRAINTS

  // (1) the number of times each order is produced
  if (opt.model() == MODEL_ONE_COUNT) { // use one main count constraint
    IntSetArgs occurrence(instance.getOrders() + 1);
    int num_idle_periods = instance.getPeriods() - instance.getOrders();
    occurrence[0] = IntSet({num_idle_periods});
    for (unsigned order = 1; order <= instance.getOrders(); order++) {
      occurrence[order] = IntSet({1});
    }
    count(*this, production_by_order, occurrence, IntArgs::create(instance.getOrders() + 1, -1), IPL_DOM);
  } else { // MODEL_N_COUNT: use several count constraints
    for (unsigned order = 0; order < instance.getOrders(); order++) {
      count(*this, production_by_order, order, IRT_EQ, 1, IPL_DOM); // each order is produced exactly once
    }
    count(*this, production_by_order, -1, IRT_EQ, instance.getPeriods() - instance.getOrders(), IPL_DOM); // idle times
  }

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

  //// SEARCH

  // branching instructions
  switch (opt.branching()) {
    case BRANCH_SDF_RANDOM:branch(*this, production_by_order, INT_VAR_SIZE_MIN(), INT_VAL_RND(rnd));
      break;
    case BRANCH_STATIC_GREEDY:greedyBranching(*this, production_by_order, instance);
      break;
    case BRANCH_HYBRID_GREEDY:hybridGreedyBranching(*this, production_by_order, instance);
      break;
    case BRANCH_SDF_GREEDY:sdfGreedyBranching(*this, production_by_order, instance);
      break;
    case BRANCH_STATIC_GREEDY_FIXED_PERIOD:
      // terrible hack to parameterise this search! Note that this brancher ist still a bit buggy
      int start_period = instance.getPeriods() - 1 < opt.iterations() ? instance.getPeriods() / 2 : opt.iterations();
      staticGreedyFixedPeriodBranching(*this, production_by_order, instance, start_period);
      break;
  }
}

void LotSizing::print(std::ostream &os) const {
  os << "production_by_order = [";
  for (unsigned period = 0; period < instance.getPeriods(); period++) {
    os << production_by_order[period].val() << ", ";
  }
  os << "];\n";
  os << "production_by_item_type = [";
  for (unsigned period = 0; period < instance.getPeriods(); period++) {
    int order = production_by_order[period].val();
    if (order != -1) {
      os << instance.getTypeOfOrder(order) << ", ";
    } else {
      os << order << ", ";
    }
  }
  os << "];\n";
  os << "objective = " << objective.val() << ";\n";
  auto stop = high_resolution_clock::now();
  auto elapsed =
      std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
  os << "time = " << elapsed.count() << " ms;\n";
  os << "------------------------------------------------------------------------------------\n";
}

bool LotSizing::slave(const MetaInfo &mi) {
  //std::cout << "Slave\n";
  if ((mi.type() == MetaInfo::RESTART) &&
      (mi.restart() > 0) && (percentage > 0.0)) {
    const LotSizing &l = static_cast<const LotSizing &>(*mi.last());
    relax(*this, production_by_order, l.production_by_order, rnd, percentage);
    return false;
  } else {
    return true;
  }
}

