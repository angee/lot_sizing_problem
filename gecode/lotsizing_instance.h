//
// Created by andrea on 12.09.19.
//

#ifndef LOT_SIZING_PROBLEM_LOTSIZING_INSTANCE_H
#define LOT_SIZING_PROBLEM_LOTSIZING_INSTANCE_H

#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

class LotSizingInstance {
 private:
  const int num_periods;
  const int num_types;
  const int num_orders;

  /** cost for storing an order for one time unit */
  const int inventory_cost;
  /** due period for each order*/
  const std::vector<int> due_period_per_order;
  /** the number of orders per item type */
  const std::vector<int> num_orders_per_type;
  /** the change costs for changing from type i to type j*/
  const std::vector<std::vector<int>> change_costs;

 public:
  LotSizingInstance(int _num_periods,
                    int _num_types,
                    int _num_orders,
                    int _inventory_costs,
                    std::vector<int> _due_period_per_order,
                    std::vector<int> _num_orders_per_type,
                    std::vector<std::vector<int>> _change_costs) :
      num_periods(_num_periods),
      num_types(_num_types),
      num_orders(_num_orders),
      inventory_cost(_inventory_costs),
      due_period_per_order(std::move(_due_period_per_order)),
      num_orders_per_type(std::move(_num_orders_per_type)),
      change_costs(std::move(_change_costs)) {}

  void print() const;
};

class LotSizingInstanceReader {

 private:
  int num_periods{};
  int num_types{};
  int num_orders{};

  /** cost for storing an order for one time unit */
  int inventory_cost{};
  /** due period for each order*/
  std::vector<int> due_period_per_order;
  /** the number of orders per item type */
  std::vector<int> num_orders_per_type;
  /** the change costs for changing from type i to type j*/
  std::vector<std::vector<int>> change_costs;

 public:
  explicit LotSizingInstanceReader(std::string input_filename) {
    readInputFile(input_filename);
  }

  LotSizingInstance generateInstance();

 private:
  void readInputFile(const std::string &input_filename);

  void readOrders(std::ifstream &infile);

  void readInventoryCost(std::ifstream &infile);

  void readChangeCosts(std::ifstream &infile);

};

#endif //LOT_SIZING_PROBLEM_LOTSIZING_INSTANCE_H
