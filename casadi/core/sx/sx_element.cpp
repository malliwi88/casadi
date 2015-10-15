/*
 *    This file is part of CasADi.
 *
 *    CasADi -- A symbolic framework for dynamic optimization.
 *    Copyright (C) 2010-2014 Joel Andersson, Joris Gillis, Moritz Diehl,
 *                            K.U. Leuven. All rights reserved.
 *    Copyright (C) 2011-2014 Greg Horn
 *
 *    CasADi is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 3 of the License, or (at your option) any later version.
 *
 *    CasADi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with CasADi; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#include "sx_element.hpp"
#include "../matrix/matrix.hpp"
#include <stack>
#include <cassert>
#include "../casadi_math.hpp"
#include "constant_sx.hpp"
#include "symbolic_sx.hpp"
#include "unary_sx.hpp"
#include "binary_sx.hpp"
#include "../casadi_options.hpp"
#include "../function/sx_function_internal.hpp"

using namespace std;
namespace casadi {

  // Allocate storage for the caching
  CACHING_MAP<int, IntegerSX*> IntegerSX::cached_constants_;
  CACHING_MAP<double, RealtypeSX*> RealtypeSX::cached_constants_;

  SXElement::SXElement() {
    node = casadi_limits<SXElement>::nan.node;
    node->count++;
  }

  SXElement::SXElement(SXNode* node_, bool dummy) : node(node_) {
    node->count++;
  }

  SXElement SXElement::create(SXNode* node) {
    return SXElement(node, false);
  }

  SXElement::SXElement(const SXElement& scalar) {
    node = scalar.node;
    node->count++;
  }

  SXElement::SXElement(double val) {
    int intval = static_cast<int>(val);
    if (val-intval == 0) { // check if integer
      if (intval == 0)             node = casadi_limits<SXElement>::zero.node;
      else if (intval == 1)        node = casadi_limits<SXElement>::one.node;
      else if (intval == 2)        node = casadi_limits<SXElement>::two.node;
      else if (intval == -1)       node = casadi_limits<SXElement>::minus_one.node;
      else                        node = IntegerSX::create(intval);
      node->count++;
    } else {
      if (isnan(val))              node = casadi_limits<SXElement>::nan.node;
      else if (isinf(val))         node = val > 0 ? casadi_limits<SXElement>::inf.node :
                                      casadi_limits<SXElement>::minus_inf.node;
      else                        node = RealtypeSX::create(val);
      node->count++;
    }
  }

  SXElement SXElement::sym(const std::string& name) {
    return create(new SymbolicSX(name));
  }

  SXElement::~SXElement() {
    if (--node->count == 0) delete node;
  }

  SXElement& SXElement::operator=(const SXElement &scalar) {
    // quick return if the old and new pointers point to the same object
    if (node == scalar.node) return *this;

    // decrease the counter and delete if this was the last pointer
    if (--node->count == 0) delete node;

    // save the new pointer
    node = scalar.node;
    node->count++;
    return *this;
  }

  void SXElement::assignIfDuplicate(const SXElement& scalar, int depth) {
    casadi_assert(depth>=1);
    if (!is_equal(*this, scalar, 0) && is_equal(*this, scalar, depth)) {
      *this = scalar;
    }
  }

  SXNode* SXElement::assignNoDelete(const SXElement& scalar) {
    // Return value
    SXNode* ret = node;

    // quick return if the old and new pointers point to the same object
    if (node == scalar.node) return ret;

    // decrease the counter but do not delete if this was the last pointer
    --node->count;

    // save the new pointer
    node = scalar.node;
    node->count++;

    // Return a pointer to the old node
    return ret;
  }

  SXElement& SXElement::operator=(double scalar) {
    return *this = SXElement(scalar);
  }

  void SXElement::repr(std::ostream &stream, bool trailing_newline) const {
    print(stream, trailing_newline);
  }

  void SXElement::print(std::ostream &stream, bool trailing_newline) const {
    node->print(stream);
    if (trailing_newline) stream << std::endl;
  }

  SXElement SXElement::operator-() const {
    if (isOp(OP_NEG))
      return getDep();
    else if (isZero())
      return 0;
    else if (isMinusOne())
      return 1;
    else if (isOne())
      return -1;
    else
      return UnarySX::create(OP_NEG, *this);
  }

  SXElement SXElement::zz_sign() const {
    return UnarySX::create(OP_SIGN, *this);
  }

  SXElement SXElement::zz_copysign(const SXElement &y) const {
    return BinarySX::create(OP_COPYSIGN, *this, y);
  }

  SXElement SXElement::zz_erfinv() const {
    return UnarySX::create(OP_ERFINV, *this);
  }

  bool SXElement::__nonzero__() const {
    if (isConstant()) return !isZero();
    casadi_error("Cannot compute the truth value of a CasADi SXElement symbolic expression.")
  }

  SXElement SXElement::zz_plus(const SXElement& y) const {
    // NOTE: Only simplifications that do not result in extra nodes area allowed

    if (!CasadiOptions::simplification_on_the_fly) return BinarySX::create(OP_ADD, *this, y);

    if (isZero())
      return y;
    else if (y->isZero()) // term2 is zero
      return *this;
    else if (y.isOp(OP_NEG))  // x + (-y) -> x - y
      return zz_minus(-y);
    else if (isOp(OP_NEG)) // (-x) + y -> y - x
      return y.zz_minus(getDep());
    else if (isOp(OP_MUL) && y.isOp(OP_MUL) &&
            getDep(0).isConstant() && getDep(0).getValue()==0.5 &&
            y.getDep(0).isConstant() && y.getDep(0).getValue()==0.5 &&
             is_equal(y.getDep(1), getDep(1), SXNode::eq_depth_)) // 0.5x+0.5x = x
      return getDep(1);
    else if (isOp(OP_DIV) && y.isOp(OP_DIV) &&
             getDep(1).isConstant() && getDep(1).getValue()==2 &&
             y.getDep(1).isConstant() && y.getDep(1).getValue()==2 &&
             is_equal(y.getDep(0), getDep(0), SXNode::eq_depth_)) // x/2+x/2 = x
      return getDep(0);
    else if (isOp(OP_SUB) && is_equal(getDep(1), y, SXNode::eq_depth_))
      return getDep(0);
    else if (y.isOp(OP_SUB) && is_equal(*this, y.getDep(1), SXNode::eq_depth_))
      return y.getDep(0);
    else if (isOp(OP_SQ) && y.isOp(OP_SQ) &&
             ((getDep().isOp(OP_SIN) && y.getDep().isOp(OP_COS))
              || (getDep().isOp(OP_COS) && y.getDep().isOp(OP_SIN)))
             && is_equal(getDep().getDep(), y.getDep().getDep(), SXNode::eq_depth_))
      return 1; // sin^2 + cos^2 -> 1
    else // create a new branch
      return BinarySX::create(OP_ADD, *this, y);
  }

  SXElement SXElement::zz_minus(const SXElement& y) const {
    // Only simplifications that do not result in extra nodes area allowed

    if (!CasadiOptions::simplification_on_the_fly) return BinarySX::create(OP_SUB, *this, y);

    if (y->isZero()) // term2 is zero
      return *this;
    if (isZero()) // term1 is zero
      return -y;
    if (is_equal(*this, y, SXNode::eq_depth_)) // the terms are equal
      return 0;
    else if (y.isOp(OP_NEG)) // x - (-y) -> x + y
      return *this + y.getDep();
    else if (isOp(OP_ADD) && is_equal(getDep(1), y, SXNode::eq_depth_))
      return getDep(0);
    else if (isOp(OP_ADD) && is_equal(getDep(0), y, SXNode::eq_depth_))
      return getDep(1);
    else if (y.isOp(OP_ADD) && is_equal(*this, y.getDep(1), SXNode::eq_depth_))
      return -y.getDep(0);
    else if (y.isOp(OP_ADD) && is_equal(*this, y.getDep(0), SXNode::eq_depth_))
      return -y.getDep(1);
    else if (isOp(OP_NEG))
      return -(getDep() + y);
    else // create a new branch
      return BinarySX::create(OP_SUB, *this, y);
  }

  SXElement SXElement::zz_times(const SXElement& y) const {

    if (!CasadiOptions::simplification_on_the_fly) return BinarySX::create(OP_MUL, *this, y);

    // Only simplifications that do not result in extra nodes area allowed
    if (is_equal(y, *this, SXNode::eq_depth_))
      return sq();
    else if (!isConstant() && y.isConstant())
      return y.zz_times(*this);
    else if (isZero() || y->isZero()) // one of the terms is zero
      return 0;
    else if (isOne()) // term1 is one
      return y;
    else if (y->isOne()) // term2 is one
      return *this;
    else if (y->isMinusOne())
      return -(*this);
    else if (isMinusOne())
      return -y;
    else if (y.isOp(OP_INV))
      return (*this)/y.inv();
    else if (isOp(OP_INV))
      return y/inv();
    else if (isConstant() && y.isOp(OP_MUL) && y.getDep(0).isConstant() &&
            getValue()*y.getDep(0).getValue()==1) // 5*(0.2*x) = x
      return y.getDep(1);
    else if (isConstant() && y.isOp(OP_DIV) && y.getDep(1).isConstant() &&
            getValue()==y.getDep(1).getValue()) // 5*(x/5) = x
      return y.getDep(0);
    else if (isOp(OP_DIV) && is_equal(getDep(1), y, SXNode::eq_depth_)) // ((2/x)*x)
      return getDep(0);
    else if (y.isOp(OP_DIV) &&
             is_equal(y.getDep(1), *this, SXNode::eq_depth_)) // ((2/x)*x)
      return y.getDep(0);
    else if (isOp(OP_NEG))
      return -(getDep() * y);
    else if (y.isOp(OP_NEG))
      return -(*this * y.getDep());
    else     // create a new branch
      return BinarySX::create(OP_MUL, *this, y);
  }


  bool SXElement::isDoubled() const {
    return isOp(OP_ADD) && is_equal(getDep(0), getDep(1), SXNode::eq_depth_);
  }

  SXElement SXElement::zz_rdivide(const SXElement& y) const {
    // Only simplifications that do not result in extra nodes area allowed

    if (!CasadiOptions::simplification_on_the_fly) return BinarySX::create(OP_DIV, *this, y);

    if (y->isZero()) // term2 is zero
      return casadi_limits<SXElement>::nan;
    else if (isZero()) // term1 is zero
      return 0;
    else if (y->isOne()) // term2 is one
      return *this;
    else if (y->isMinusOne())
      return -(*this);
    else if (is_equal(*this, y, SXNode::eq_depth_)) // terms are equal
      return 1;
    else if (isDoubled() && is_equal(y, 2))
      return getDep(0);
    else if (isOp(OP_MUL) && is_equal(y, getDep(0), SXNode::eq_depth_))
      return getDep(1);
    else if (isOp(OP_MUL) && is_equal(y, getDep(1), SXNode::eq_depth_))
      return getDep(0);
    else if (isOne())
      return y.inv();
    else if (y.isOp(OP_INV))
      return (*this)*y.inv();
    else if (isDoubled() && y.isDoubled())
      return getDep(0) / y->dep(0);
    else if (y.isConstant() && isOp(OP_DIV) && getDep(1).isConstant() &&
            y.getValue()*getDep(1).getValue()==1) // (x/5)/0.2
      return getDep(0);
    else if (y.isOp(OP_MUL) &&
             is_equal(y.getDep(1), *this, SXNode::eq_depth_)) // x/(2*x) = 1/2
      return BinarySX::create(OP_DIV, 1, y.getDep(0));
    else if (isOp(OP_NEG) &&
             is_equal(getDep(0), y, SXNode::eq_depth_))      // (-x)/x = -1
      return -1;
    else if (y.isOp(OP_NEG) &&
             is_equal(y.getDep(0), *this, SXNode::eq_depth_))      // x/(-x) = 1
      return -1;
    else if (y.isOp(OP_NEG) && isOp(OP_NEG) &&
             is_equal(getDep(0), y.getDep(0), SXNode::eq_depth_))  // (-x)/(-x) = 1
      return 1;
    else if (isOp(OP_DIV) && is_equal(y, getDep(0), SXNode::eq_depth_))
      return getDep(1).inv();
    else if (isOp(OP_NEG))
      return -(getDep() / y);
    else if (y.isOp(OP_NEG))
      return -(*this / y.getDep());
    else // create a new branch
      return BinarySX::create(OP_DIV, *this, y);
  }

  SXElement SXElement::inv() const {
    if (isOp(OP_INV)) {
      return getDep(0);
    } else {
      return UnarySX::create(OP_INV, *this);
    }
  }

  SX SXElement::zz_min(const SX& b) const {
    return fmin(SX(*this), b);
  }
  SX SXElement::zz_max(const SX& b) const {
    return fmax(SX(*this), b);
  }
  SX SXElement::zz_constpow(const SX& n) const {
    return SX(*this).zz_constpow(n);
  }
  SX SXElement::zz_copysign(const SX& n) const {
    return SX(*this).zz_copysign(n);
  }

  SX SXElement::zz_atan2(const SX& b) const {
    return atan2(SX(*this), b);
  }

  SXElement SXElement::zz_le(const SXElement& y) const {
    if ((y-(*this)).isNonNegative())
      return 1;
    else
      return BinarySX::create(OP_LE, *this, y);
  }

  SXElement SXElement::zz_lt(const SXElement& y) const {
    if (((*this)-y).isNonNegative())
      return 0;
    else
      return BinarySX::create(OP_LT, *this, y);
  }

  SXElement SXElement::zz_eq(const SXElement& y) const {
    if (is_equal(*this, y))
      return 1;
    else
      return BinarySX::create(OP_EQ, *this, y);
  }

  SXElement SXElement::zz_ne(const SXElement& y) const {
    if (is_equal(*this, y))
      return 0;
    else
      return BinarySX::create(OP_NE, *this, y);
  }

  SXNode* SXElement::get() const {
    return node;
  }

  const SXNode* SXElement::operator->() const {
    return node;
  }

  SXNode* SXElement::operator->() {
    return node;
  }

  SXElement if_else(const SXElement& cond, const SXElement& if_true,
                    const SXElement& if_false, bool short_circuit) {
    return if_else_zero(cond, if_true) + if_else_zero(!cond, if_false);
  }

  SXElement SXElement::binary(int op, const SXElement& x, const SXElement& y) {
    return BinarySX::create(Operation(op), x, y);
  }

  SXElement SXElement::unary(int op, const SXElement& x) {
    return UnarySX::create(Operation(op), x);
  }

  bool SXElement::isLeaf() const {
    if (!node) return true;
    return isConstant() || isSymbolic();
  }

  bool SXElement::isCommutative() const {
    if (!hasDep()) throw CasadiException("SX::isCommutative: must be binary");
    return operation_checker<CommChecker>(getOp());
  }

  bool SXElement::isConstant() const {
    return node->isConstant();
  }

  bool SXElement::isInteger() const {
    return node->isInteger();
  }

  bool SXElement::isSymbolic() const {
    return node->isSymbolic();
  }

  bool SXElement::hasDep() const {
    return node->hasDep();
  }

  bool SXElement::isZero() const {
    return node->isZero();
  }

  bool SXElement::isAlmostZero(double tol) const {
    return node->isAlmostZero(tol);
  }

  bool SXElement::isOne() const {
    return node->isOne();
  }

  bool SXElement::isMinusOne() const {
    return node->isMinusOne();
  }

  bool SXElement::isNan() const {
    return node->isNan();
  }

  bool SXElement::isInf() const {
    return node->isInf();
  }

  bool SXElement::isMinusInf() const {
    return node->isMinusInf();
  }

  const std::string& SXElement::getName() const {
    return node->getName();
  }

  int SXElement::getOp() const {
    return node->getOp();
  }

  bool SXElement::isOp(int op) const {
    return hasDep() && op==getOp();
  }

  bool SXElement::zz_is_equal(const SXElement& ex, int depth) const {
    if (node==ex.get())
      return true;
    else if (depth>0)
      return node->zz_is_equal(ex.get(), depth);
    else
      return false;
  }

  bool SXElement::isNonNegative() const {
    if (isConstant())
      return getValue()>=0;
    else if (isOp(OP_SQ) || isOp(OP_FABS))
      return true;
    else
      return false;
  }

  double SXElement::getValue() const {
    return node->getValue();
  }

  int SXElement::getIntValue() const {
    return node->getIntValue();
  }

  SXElement SXElement::getDep(int ch) const {
    casadi_assert(ch==0 || ch==1;)
      return node->dep(ch);
  }

  int SXElement::getNdeps() const {
    if (!hasDep()) throw CasadiException("SX::getNdeps: must be binary");
    return casadi_math<double>::ndeps(getOp());
  }

  size_t SXElement::__hash__() const {
    return reinterpret_cast<size_t>(node);
  }

  // node corresponding to a constant 0
  const SXElement casadi_limits<SXElement>::zero(new ZeroSX(), false);
  // node corresponding to a constant 1
  const SXElement casadi_limits<SXElement>::one(new OneSX(), false);
  // node corresponding to a constant 2
  const SXElement casadi_limits<SXElement>::two(IntegerSX::create(2), false);
  // node corresponding to a constant -1
  const SXElement casadi_limits<SXElement>::minus_one(new MinusOneSX(), false);
  const SXElement casadi_limits<SXElement>::nan(new NanSX(), false);
  const SXElement casadi_limits<SXElement>::inf(new InfSX(), false);
  const SXElement casadi_limits<SXElement>::minus_inf(new MinusInfSX(), false);

  bool casadi_limits<SXElement>::isZero(const SXElement& val) {
    return val.isZero();
  }

  bool casadi_limits<SXElement>::isAlmostZero(const SXElement& val, double tol) {
    return val.isAlmostZero(tol);
  }

  bool casadi_limits<SXElement>::isOne(const SXElement& val) {
    return val.isOne();
  }

  bool casadi_limits<SXElement>::isMinusOne(const SXElement& val) {
    return val.isMinusOne();
  }

  bool casadi_limits<SXElement>::isConstant(const SXElement& val) {
    return val.isConstant();
  }

  bool casadi_limits<SXElement>::isInteger(const SXElement& val) {
    return val.isInteger();
  }

  bool casadi_limits<SXElement>::isInf(const SXElement& val) {
    return val.isInf();
  }

  bool casadi_limits<SXElement>::isMinusInf(const SXElement& val) {
    return val.isMinusInf();
  }

  bool casadi_limits<SXElement>::isNaN(const SXElement& val) {
    return val.isNan();
  }

  SXElement SXElement::zz_exp() const {
    return UnarySX::create(OP_EXP, *this);
  }

  SXElement SXElement::zz_log() const {
    return UnarySX::create(OP_LOG, *this);
  }

  SXElement SXElement::zz_log10() const {
    return log(*this)*(1/std::log(10.));
  }

  SXElement SXElement::zz_sqrt() const {
    if (isOp(OP_SQ))
      return fabs(getDep());
    else
      return UnarySX::create(OP_SQRT, *this);
  }

  SXElement SXElement::sq() const {
    if (isOp(OP_SQRT))
      return getDep();
    else if (isOp(OP_NEG))
      return getDep().sq();
    else
      return UnarySX::create(OP_SQ, *this);
  }

  SXElement SXElement::zz_sin() const {
    return UnarySX::create(OP_SIN, *this);
  }

  SXElement SXElement::zz_cos() const {
    return UnarySX::create(OP_COS, *this);
  }

  SXElement SXElement::zz_tan() const {
    return UnarySX::create(OP_TAN, *this);
  }

  SXElement SXElement::zz_asin() const {
    return UnarySX::create(OP_ASIN, *this);
  }

  SXElement SXElement::zz_acos() const {
    return UnarySX::create(OP_ACOS, *this);
  }

  SXElement SXElement::zz_atan() const {
    return UnarySX::create(OP_ATAN, *this);
  }

  SXElement SXElement::zz_sinh() const {
    if (isZero())
      return 0;
    else
      return UnarySX::create(OP_SINH, *this);
  }

  SXElement SXElement::zz_cosh() const {
    if (isZero())
      return 1;
    else
      return UnarySX::create(OP_COSH, *this);
  }

  SXElement SXElement::zz_tanh() const {
    if (isZero())
      return 0;
    else
      return UnarySX::create(OP_TANH, *this);
  }

  SXElement SXElement::zz_atanh() const {
    if (isZero())
      return 0;
    else
      return UnarySX::create(OP_ATANH, *this);
  }

  SXElement SXElement::zz_acosh() const {
    if (isOne())
      return 0;
    else
      return UnarySX::create(OP_ACOSH, *this);
  }

  SXElement SXElement::zz_asinh() const {
    if (isZero())
      return 0;
    else
      return UnarySX::create(OP_ASINH, *this);
  }

  SXElement SXElement::zz_floor() const {
    return UnarySX::create(OP_FLOOR, *this);
  }

  SXElement SXElement::zz_ceil() const {
    return UnarySX::create(OP_CEIL, *this);
  }

  SXElement SXElement::zz_mod(const SXElement &b) const {
    return BinarySX::create(OP_FMOD, *this, b);
  }

  SXElement SXElement::zz_erf() const {
    return UnarySX::create(OP_ERF, *this);
  }

  SXElement SXElement::zz_abs() const {
    if (isOp(OP_FABS) || isOp(OP_SQ))
      return *this;
    else
      return UnarySX::create(OP_FABS, *this);
  }

  SXElement::operator SX() const {
    return SX(Sparsity::scalar(), *this, false);
  }

  SXElement SXElement::zz_min(const SXElement &b) const {
    return BinarySX::create(OP_FMIN, *this, b);
  }

  SXElement SXElement::zz_max(const SXElement &b) const {
    return BinarySX::create(OP_FMAX, *this, b);
  }

  SXElement SXElement::zz_atan2(const SXElement &b) const {
    return BinarySX::create(OP_ATAN2, *this, b);
  }

  SXElement SXElement::printme(const SXElement &b) const {
    return BinarySX::create(OP_PRINTME, *this, b);
  }

  SXElement SXElement::zz_power(const SXElement& n) const {
    if (n->isConstant()) {
      if (n->isInteger()) {
        int nn = n->getIntValue();
        if (nn == 0) {
          return 1;
        } else if (nn>100 || nn<-100) { // maximum depth
          return BinarySX::create(OP_CONSTPOW, *this, nn);
        } else if (nn<0) { // negative power
          return 1/pow(*this, -nn);
        } else if (nn%2 == 1) { // odd power
          return *this*pow(*this, nn-1);
        } else { // even power
          SXElement rt = pow(*this, nn/2);
          return rt*rt;
        }
      } else if (n->getValue()==0.5) {
        return sqrt(*this);
      } else {
        return BinarySX::create(OP_CONSTPOW, *this, n);
      }
    } else {
      return BinarySX::create(OP_POW, *this, n);
    }
  }

  SXElement SXElement::zz_constpow(const SXElement& n) const {
    return BinarySX::create(OP_CONSTPOW, *this, n);
  }

  SXElement SXElement::zz_not() const {
    if (isOp(OP_NOT)) {
      return getDep();
    } else {
      return UnarySX::create(OP_NOT, *this);
    }
  }

  SXElement SXElement::zz_and(const SXElement& y) const {
    return BinarySX::create(OP_AND, *this, y);
  }

  SXElement SXElement::zz_or(const SXElement& y) const {
    return BinarySX::create(OP_OR, *this, y);
  }

  SXElement SXElement::zz_if_else_zero(const SXElement& y) const {
    if (y->isZero()) {
      return y;
    } else if (isConstant()) {
      if (getValue()!=0) return y;
      else              return 0;
    } else {
      return BinarySX::create(OP_IF_ELSE_ZERO, *this, y);
    }
  }

  int SXElement::getTemp() const {
    return (*this)->temp;
  }

  void SXElement::setTemp(int t) {
    (*this)->temp = t;
  }

  bool SXElement::marked() const {
    return (*this)->marked();
  }

  void SXElement::mark() {
    (*this)->mark();
  }

  bool SXElement::isRegular() const {
    if (isConstant()) {
      return !(isNan() || isInf() || isMinusInf());
    } else {
      casadi_error("Cannot check regularity for symbolic SXElement");
    }
  }

} // namespace casadi

using namespace casadi;
namespace std {
/**
  const bool numeric_limits<casadi::SXElement>::is_specialized = true;
  const int  numeric_limits<casadi::SXElement>::digits = 0;
  const int  numeric_limits<casadi::SXElement>::digits10 = 0;
  const bool numeric_limits<casadi::SXElement>::is_signed = false;
  const bool numeric_limits<casadi::SXElement>::is_integer = false;
  const bool numeric_limits<casadi::SXElement>::is_exact = false;
  const int numeric_limits<casadi::SXElement>::radix = 0;
  const int  numeric_limits<casadi::SXElement>::min_exponent = 0;
  const int  numeric_limits<casadi::SXElement>::min_exponent10 = 0;
  const int  numeric_limits<casadi::SXElement>::max_exponent = 0;
  const int  numeric_limits<casadi::SXElement>::max_exponent10 = 0;

  const bool numeric_limits<casadi::SXElement>::has_infinity = true;
  const bool numeric_limits<casadi::SXElement>::has_quiet_NaN = true;
  const bool numeric_limits<casadi::SXElement>::has_signaling_NaN = false;
  const float_denorm_style has_denorm = denorm absent;
  const bool numeric_limits<casadi::SXElement>::has_denorm_loss = false;

  const bool numeric_limits<casadi::SXElement>::is_iec559 = false;
  const bool numeric_limits<casadi::SXElement>::is_bounded = false;
  const bool numeric_limits<casadi::SXElement>::is_modulo = false;

  const bool numeric_limits<casadi::SXElement>::traps = false;
  const bool numeric_limits<casadi::SXElement>::tinyness_before = false;
  const float_round_style numeric_limits<casadi::SXElement>::round_style = round_toward_zero;
*/
  SXElement numeric_limits<SXElement>::infinity() throw() {
    return casadi::casadi_limits<SXElement>::inf;
  }

  SXElement numeric_limits<SXElement>::quiet_NaN() throw() {
    return casadi::casadi_limits<SXElement>::nan;
  }

  SXElement numeric_limits<SXElement>::min() throw() {
    return SXElement(numeric_limits<double>::min());
  }

  SXElement numeric_limits<SXElement>::max() throw() {
    return SXElement(numeric_limits<double>::max());
  }

  SXElement numeric_limits<SXElement>::epsilon() throw() {
    return SXElement(numeric_limits<double>::epsilon());
  }

  SXElement numeric_limits<SXElement>::round_error() throw() {
    return SXElement(numeric_limits<double>::round_error());
  }

} // namespace std

