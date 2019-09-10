#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <fstream>
#include <sstream>
#include <string>

using namespace Gecode;

/**
 * \brief Discrete Lotsizing Problem
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
 * Author: Andrea Rendl, July 2019
 *
 */

class LotSizingInstance {

 private:
  int num_periods;
  int num_types;
  int num_orders;

  /** due period for each order*/
  std::vector<int> due_period_per_order;
  /** the number of orders per item type */
  std::vector<int> num_orders_per_type;

 public:
  LotSizingInstance(std::string input_filename) {
    readInputFile(input_filename);
  }

 private:
  void readInputFile(const std::string input_filename) {
    std::ifstream infile(input_filename);
    if (!infile.good()) {
      std::stringstream error_msg;
      error_msg << "Cannot open file: " << input_filename;
      throw std::runtime_error(error_msg.str());
    }
    infile >> num_periods;
    infile >> num_types;

    readOrders(infile);
  }

  void readOrders(std::ifstream &infile) {
    int order_cnt = 0;
    int type_cnt = 0;

    std::string line;
    while(type_cnt < num_types && std::getline(infile, line)) {
      std::getline(infile, line);
      std::stringstream iss(line);
      int order_cnt_per_type = 0;
      // read the line for the item type
      int period_cnt = 0;
      int hasOrder;
      while(period_cnt < num_periods && iss >> hasOrder) {
        //std::cout << "hasOrder of " << period_cnt << ": " << hasOrder << std::endl;
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

 public:
  void print() const {
    std::cout << "#Periods = " << num_periods << std::endl;
    std::cout << "#ItemTypes = " << num_types << std::endl;
    std::cout << "#Orders = " << num_orders << std::endl;
    std::cout << "\ndue_periods = ";
    for (auto i = due_period_per_order.begin(); i != due_period_per_order.end(); ++i)
      std::cout << *i << " ";
    std::cout << std::endl;
    std::cout << "\nnum_order_per_item = ";
    for (auto i = num_orders_per_type.begin(); i != num_orders_per_type.end(); ++i)
      std::cout << *i << " ";
    std::cout << std::endl;
  }
};

class LotSizing : public IntMinimizeScript {
 protected:
  LotSizingInstance instance;
  IntVarArray production_by_order;
  IntVarArray production_period;
  IntVarArray inventory_periods;
  IntVarArray change_cost_for_period;
  IntVarArray production_order; // FIXME: can it be an IntVarArgs array?

 public:
  LotSizing(const InstanceOptions &opt) : IntMinimizeScript(opt), instance(opt.instance()) {
    instance.print();
    // TODO
  }

  virtual Space *copy(void) {
    // TODO
  }

  virtual IntVar cost(void) const {
    // TODO
  }

};

