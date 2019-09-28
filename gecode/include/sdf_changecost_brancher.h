/**
 * \brief Branching Heuristic for the Discrete Lotsizing Problem:
 *
 * Smallest domain first variable selection combined with cheapest change cost
 * or shortest due period value selection
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
 */

#ifndef LOT_SIZING_PROBLEM_SDF_CHANGECOST_BRANCHER_H
#define LOT_SIZING_PROBLEM_SDF_CHANGECOST_BRANCHER_H

#include <gecode/int.hh>
#include <lotsizing_instance.h>

using namespace Gecode;

class SdfGreedyBranching : public Brancher {
  /// Views for the the production order
  ViewArray<Int::IntView> production_by_order;
  /// the problem instance
  const LotSizingInstance instance;

  class Choice : public Gecode::Choice {
   public:
    /// period: the position in production order array
    int period;
    /// order: the value assigned to the element of the variable array
    int order;
    /** Initialize choice for brancher \a b, position \a pos (period),
     *  and value \a val (order).
     */
    Choice(const Brancher &b, int pos, int val)
        : Gecode::Choice(b, 2), period(pos), order(val) {}

    /// Archive into \a e
    virtual void archive(Archive &e) const {
      Gecode::Choice::archive(e);
      e << period << order;
    }
  };

 public:
  SdfGreedyBranching(Home home, ViewArray<Int::IntView> &production, const LotSizingInstance &instance0) :
      Brancher(home), production_by_order(production), instance(instance0) {
  }
  // copy constructor
  SdfGreedyBranching(Space &home, SdfGreedyBranching &gb) : Brancher(home, gb),
                                                            production_by_order(gb.production_by_order),
                                                            instance(gb.instance) {
    production_by_order.update(home, gb.production_by_order);
  }

  static void post(Home home, ViewArray<Int::IntView> &p, const LotSizingInstance instance) {
    (void) new(home) SdfGreedyBranching(home, p, instance);
  }

  /// Copy brancher
  virtual Actor *copy(Space &home) {
    return new(home) SdfGreedyBranching(home, *this);
  }

  /// Check status of brancher, return true if alternatives left
  virtual bool status(const Space &) const;

  /// Return choice
  virtual Gecode::Choice *choice(Space &);

  /// Return choice
  virtual const Gecode::Choice *choice(const Space &, Archive &e) {
    int pos, val;
    e >> pos >> val;
    return new Choice(*this, pos, val);
  }

  /// Perform commit for choice \a _c and alternative \a a
  virtual ExecStatus commit(Space &home, const Gecode::Choice &_c,
                            unsigned int a) {
    const Choice &c = static_cast<const Choice &>(_c);
    if (a == 0)
      return me_failed(production_by_order[c.period].eq(home, c.order)) ? ES_FAILED : ES_OK;
    else
      return me_failed(production_by_order[c.period].nq(home, c.order)) ? ES_FAILED : ES_OK;
  }

  /// Print explanation
  virtual void print(const Space &, const Gecode::Choice &_c,
                     unsigned int a,
                     std::ostream &o) const {
    const Choice &c = static_cast<const Choice &>(_c);
    o << "prod[" << c.period << "] "
      << ((a == 0) ? "=" : "!=")
      << " " << c.order;
  }

 private:
  /**
   * Returns the smallest domain index (int) and -1 if all variables are assigned. If there are two variables with the
   * same domain, then the variable is preferred whose predecessor variable is already assigned (to calculate the
   * cheapest change costs).
   * @return
   */
  int findPeriodWithSmallestBestDomain() const;
};

/// Post branching (assumes that \a s is sorted)
inline void sdfGreedyBranching(Home home,
                               const IntVarArgs &production_by_order,
                               const LotSizingInstance instance) {
  ViewArray<Int::IntView> production(home, production_by_order);
  return SdfGreedyBranching::post(home, production, instance);
}

#endif //LOT_SIZING_PROBLEM_SDF_CHANGECOST_BRANCHER_H
