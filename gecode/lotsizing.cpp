#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <fstream>
#include <sstream>
#include <string>

#include "lotsizing_instance.h"

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


class LotSizing : public IntMinimizeScript {
 protected:
  LotSizingInstanceReader reader;
  LotSizingInstance instance;

  IntVarArray production_by_order;
  IntVarArray production_period;
  IntVarArray inventory_periods;
  IntVarArray change_cost_for_period;
  IntVarArray production_order; // FIXME: can it be an IntVarArgs array?

 public:
  LotSizing(const InstanceOptions &opt) : IntMinimizeScript(opt), reader(opt.instance()), instance(reader.generateInstance()) { //, instance((LotSizingInstanceReader(opt.instance)).generateInstance()) {
    //LotSizingInstanceReader reader(opt.instance());
    //instance = reader.generateInstance();
    instance.print();
    // TODO: problem model
  }

  virtual Space *copy(void) {
    // TODO
  }

  virtual IntVar cost(void) const {
    // TODO
  }

};

