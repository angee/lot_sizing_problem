#ifndef LOT_SIZING_PROBLEM_LOTSIZING_INSTANCE_H
#define LOT_SIZING_PROBLEM_LOTSIZING_INSTANCE_H

/**
 * \brief Discrete Lotsizing Problem Instance
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

#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <iostream>

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

  // copy constructor
  LotSizingInstance(const LotSizingInstance &l) :
      num_periods(l.num_periods),
      num_types(l.num_types),
      num_orders(l.num_orders),
      inventory_cost(l.inventory_cost),
      due_period_per_order(l.due_period_per_order),
      num_orders_per_type(l.num_orders_per_type),
      change_costs(l.change_costs) {
  }

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

  int getDuePeriod(int order) const {
    if (order < 0 || order >= num_orders) {
      std::stringstream error_msg;
      error_msg << "Invalid order number: " << order;
      throw std::runtime_error(error_msg.str());
    }
    return due_period_per_order[order];
  }

  int getChangeCosts(int orderFrom, int orderTo) const {
    // the costs for idle time is zero
    if (orderFrom == -1 | orderTo == -1) {
      return 0.;
    }
    if (orderFrom < 0 || orderFrom >= num_orders) {
      std::stringstream error_msg;
      error_msg << "Invalid from-order number: " << orderFrom;
      throw std::runtime_error(error_msg.str());
    } else if (orderTo < 0 || orderTo >= num_orders) {
      std::stringstream error_msg;
      error_msg << "Invalid to-order number: " << orderTo;
      throw std::runtime_error(error_msg.str());
    }
    return change_costs[getTypeOfOrder(orderFrom)][getTypeOfOrder(orderTo)];
  }

  int getNumberOfOrders(int item_type) const {
    if(item_type < 0 || item_type >= num_types) {
      std::stringstream error_msg;
      error_msg << "Invalid item-type number: " << item_type;
      throw std::runtime_error(error_msg.str());
    }
    return num_orders_per_type[item_type];
  }

  int getOrderNumber(int item_type, int order_of_type) const;

  /** returns the maximal change cost between two item types */
  int calculateMaxChangeCost() const;

  /** returns the maximum (latest) due period of an order */
  int calculateMaxDuePeriod() const;

  /** returns the upper bound for the objective */
  int calculateUpperBoundForObjective() const;

  /** returns the item type of the given order */
  int getTypeOfOrder(int order) const;

  std::vector<int> getOrdersDueAfterPeriodOrderedByChangeCost(int period, int previous_order) const;

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
