#include "lotsizing_instance.h"
#include <iostream>
#include <algorithm>

void LotSizingInstance::print() const {
  std::cout << "#Periods = " << num_periods << std::endl;
  std::cout << "#ItemTypes = " << num_types << std::endl;
  std::cout << "#Orders = " << num_orders << std::endl;
  std::cout << "inventory_cost = " << inventory_cost << std::endl;
  std::cout << "due_periods = ";
  for (auto i = due_period_per_order.begin(); i != due_period_per_order.end(); ++i)
    std::cout << *i << " ";
  std::cout << std::endl;
  std::cout << "num_order_per_item = ";
  for (auto i = num_orders_per_type.begin(); i != num_orders_per_type.end(); ++i)
    std::cout << *i << " ";
  std::cout << std::endl;
  std::cout << "change_costs = \n";
  for (auto i = change_costs.begin(); i != change_costs.end(); ++i) {
    for (auto j = (*i).begin(); j != (*i).end(); ++j) {
      std::cout << *j << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

int LotSizingInstance::getTypeOfOrder(const int order) const {
  int order_cnt = 0;
  int order_per_type_cnt = 0;
  int type = 0;
  // count the number of orders per type until we reached the order number
  while (order_cnt < order) {
    if (order_per_type_cnt == num_orders_per_type[type]) {
      type++;
      order_per_type_cnt = 0;
    } else {
      order_per_type_cnt++;
    }
    order_cnt++;
  }
  return type;
}

int LotSizingInstance::calculateMaxChangeCost() const {
  int maximum = 0;
  for (auto vec : change_costs) {
    int local_max = *std::max_element(vec.begin(), vec.end());
    if (local_max > maximum) {
      maximum = local_max;
    }
  }
  return maximum;
}

int LotSizingInstance::calculateMaxDuePeriod() const {
  return *std::max_element(due_period_per_order.begin(), due_period_per_order.end());
}

int LotSizingInstance::calculateUpperBoundForObjective() const {
  // change costs: for each order, pick the max cost
  int max_change_costs = 0;
  for (unsigned order = 0; order < num_orders; order++) {
    int item_type = getTypeOfOrder(order);
    int max_change_cost_for_order = *std::max_element(change_costs[item_type].begin(), change_costs[item_type].end());
    max_change_costs += max_change_cost_for_order;
  }
  // maximal inventory costs: every order is picked at the worst point
  int max_inventory_cost = 0;
  for (unsigned order = 0; order < num_orders; order++) {
    max_inventory_cost += (due_period_per_order[order] - 1);
  }
  return max_change_costs + max_inventory_cost;
}

void LotSizingInstanceReader::readInputFile(const std::string &input_filename) {
  std::ifstream infile(input_filename);
  if (!infile.good()) {
    std::stringstream error_msg;
    error_msg << "Cannot open file: " << input_filename;
    throw std::runtime_error(error_msg.str());
  }
  infile >> num_periods;
  infile >> num_types;

  readOrders(infile);
  readInventoryCost(infile);
  readChangeCosts(infile);
}

void LotSizingInstanceReader::readOrders(std::ifstream &infile) {
  int order_cnt = 0;
  int type_cnt = 0;

  std::string line;
  while (type_cnt < num_types && std::getline(infile, line)) {
    if (line.size() == 0)
      continue;
    std::stringstream iss(line);
    int order_cnt_per_type = 0;
    int period_cnt = 0;
    int hasOrder;
    // read the line for the item type
    while (period_cnt < num_periods && iss >> hasOrder) {
      if (hasOrder == 1) {
        due_period_per_order.push_back(period_cnt + 1);
        order_cnt++;
        order_cnt_per_type++;
      }
      period_cnt++;
    }
    num_orders_per_type.push_back(order_cnt_per_type);
    type_cnt++;
  }
  num_orders = order_cnt;
}

void LotSizingInstanceReader::readInventoryCost(std::ifstream &infile) {
  std::string line;
  bool has_read_inventory_cost = false;
  while (!has_read_inventory_cost && std::getline(infile, line)) {
    if (line.size() == 0)
      continue;
    std::stringstream iss(line);
    iss >> inventory_cost;
    has_read_inventory_cost = true;
  }
}

void LotSizingInstanceReader::readChangeCosts(std::ifstream &infile) {
  std::string line;
  int type_from = 0;
  while (type_from < num_types && std::getline(infile, line)) {
    if (line.size() == 0)
      continue;
    change_costs.push_back(std::vector<int>()); // initialise the vector for the type
    std::stringstream iss(line);
    int type_to = 0;
    int change_cost;
    while (type_to < num_types && iss >> change_cost) {
      type_to++;
      change_costs[type_from].push_back(change_cost);
    }
    type_from++;
  }
}

LotSizingInstance LotSizingInstanceReader::generateInstance() {
  return LotSizingInstance(num_periods,
                           num_types,
                           num_orders,
                           inventory_cost,
                           due_period_per_order,
                           num_orders_per_type,
                           change_costs);
}