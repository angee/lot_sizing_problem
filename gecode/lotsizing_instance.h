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

  /** prints a summary of the instance setup */
  void print() const;

  int getPeriods() const {
    return num_periods;
  }

  int getTypes() const {
    return num_types;
  }

  int getOrders() const {
    return num_orders;
  }

  int getInventoryCost() const {
    return inventory_cost;
  }

  int getDuePeriod(const int order) {
    if(order < 0 || order >= num_orders) {
      std::stringstream error_msg;
      error_msg << "Invalid order number: " << order;
      throw std::runtime_error(error_msg.str());
    }
    return due_period_per_order[getTypeOfOrder(order)];
  }

  int getChangeCosts(int orderFrom, int orderTo) const {
    if(orderFrom < 0 || orderFrom >= num_orders) {
      std::stringstream error_msg;
      error_msg << "Invalid order number: " << orderFrom;
      throw std::runtime_error(error_msg.str());
    }
    else if(orderTo < 0 || orderTo >= num_orders) {
      std::stringstream error_msg;
      error_msg << "Invalid order number: " << orderTo;
      throw std::runtime_error(error_msg.str());
    }
    return change_costs[getTypeOfOrder(orderFrom)][getTypeOfOrder(orderTo)];
  }

  /** returns the maximal change cost between two item types */
  int calculateMaxChangeCost() const;

  /** returns the maximum (latest) due period of an order */
  int calculateMaxDuePeriod() const;


 private:
  int getTypeOfOrder(int order) const;

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
