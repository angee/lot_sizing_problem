/**
 * @authors Andrea Rendl, Satalia
 */
#include <lotsizing_instance.h>
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
  int type = 0;
  if(order < num_orders_per_type[type]) {
    return type;
  }
  int order_cnt = num_orders_per_type[type];
  type++;
  while(order >= num_orders_per_type[type] + order_cnt) {
    order_cnt += num_orders_per_type[type];
    type++;
  }
  return type;
}

int LotSizingInstance::getOrderNumber(int item_type, int order_of_type) const {
  if (item_type < 0 || item_type >= num_types) {
    std::stringstream error_msg;
    error_msg << "invalid type number: " << item_type;
    throw std::runtime_error(error_msg.str());
  }
  if (order_of_type < 0 || order_of_type >= num_orders_per_type[item_type]) {
    std::stringstream error_msg;
    error_msg << "invalid order_of_type number: " << order_of_type;
    throw std::runtime_error(error_msg.str());
  }
  if (item_type == 0)
    return order_of_type;

  int order_number = 0;
  unsigned type_cnt = 0;
  while(type_cnt < item_type) {
    order_number += num_orders_per_type[type_cnt];
    type_cnt++;
  }
  return order_number + order_of_type;
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

std::vector<int> LotSizingInstance::getOrdersDueAfterPeriodOrderedByChangeCost(int period, int previous_order) const {
  std::vector<std::pair<int, int>> orders;
  for (unsigned order = 0; order < due_period_per_order.size(); order++) {
    if (due_period_per_order[order] > period)
      orders.push_back(std::make_pair(getChangeCosts(previous_order, order), order));
  }
  std::sort(orders.begin(), orders.end());
  std::vector<int> ordered_orders;
  for (auto pair : orders) {
    ordered_orders.push_back(pair.second);
  }
  // insert the idle move "-1" after all zero cost orders
  int i = 0;
  while (i < orders.size() && orders[i].first == 0) {
    i++;
  }
  ordered_orders.insert(ordered_orders.begin() + i, -1);
  return ordered_orders;
}

std::vector<int> LotSizingInstance::getOrdersDueAfterPeriodOrderedByDuePeriod(int period) const {
  std::vector<std::pair<int, int>> orders;
  for (unsigned order = 0; order < due_period_per_order.size(); order++) {
    if (due_period_per_order[order] >= period)
      orders.push_back(std::make_pair(getDuePeriod(order), order));
  }
  std::sort(orders.begin(), orders.end());
  std::vector<int> ordered_orders;
  for (auto pair : orders) {
    ordered_orders.push_back(pair.second);
  }
  // insert the idle move "-1" after all orders that are immediately due
  int i = 0;
  while (i < orders.size() && period - orders[i].first == 1) {
    i++;
  }
  ordered_orders.insert(ordered_orders.begin() + i, -1);
  return ordered_orders;
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
    if (line.empty())
      continue;
    std::stringstream iss(line);
    int order_cnt_per_type = 0;
    int period_cnt = 0;
    int hasOrder;
    // read the line for the item type
    while (period_cnt < num_periods && iss >> hasOrder) {
      if (hasOrder == 1) {
        due_period_per_order.push_back(period_cnt);
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
    if (line.empty())
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
    if (line.empty())
      continue;
    change_costs.emplace_back(); // initialise the vector for the type
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