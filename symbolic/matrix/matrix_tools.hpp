/*
 *    This file is part of CasADi.
 *
 *    CasADi -- A symbolic framework for dynamic optimization.
 *    Copyright (C) 2010 by Joel Andersson, Moritz Diehl, K.U.Leuven. All rights reserved.
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

#ifndef MATRIX_TOOLS_HPP
#define MATRIX_TOOLS_HPP

#include "matrix.hpp"
#include <algorithm>

#include "../options_functionality.hpp"

#include "sparsity_tools.hpp"

namespace CasADi{

  /// Transpose of a matrix
  template<typename DataType>
  Matrix<DataType> transpose(const Matrix<DataType> &x);

  /** \brief  Matrix product of two matrices
   *
   * With optional sp_z you can specify the sparsity of the result
   * A typical use case might be where the product is only constructed to 
   * inspect the trace of it. sp_z diagonal will be more efficient then. 
   */
  template<typename DataType>
  Matrix<DataType> mul(const Matrix<DataType> &x, const Matrix<DataType> &y, const Sparsity& sp_z=Sparsity());

  /// Matrix product of n matrices
  template<typename DataType>
  Matrix<DataType> mul(const std::vector< Matrix<DataType> > &args);

  template<typename DataType>
  DataType det(const Matrix<DataType>& a);

  template<typename DataType>
  DataType getMinor(const Matrix<DataType> &x, int i, int j);

  template<typename DataType>
  DataType cofactor(const Matrix<DataType> &x, int i, int j);

  template<typename DataType>
  Matrix<DataType> adj(const Matrix<DataType>& a);

  template<typename DataType>
  Matrix<DataType> inv(const Matrix<DataType>& a);

  template<typename DataType>
  Matrix<DataType> reshape(const Matrix<DataType>& a, int nrow, int ncol);

  template<typename DataType>
  Matrix<DataType> reshape(const Matrix<DataType>& a, std::pair<int,int> rc);

  template<typename DataType>
  Matrix<DataType> reshape(const Matrix<DataType>& a, const Sparsity& sp);

  template<typename DataType>
  DataType trace(const Matrix<DataType>& a);

  /** \brief  make a vector
      Reshapes/vectorizes the Matrix<DataType> such that the shape becomes (expr.numel(),1).
      Columns are stacked on top of each other.
      Same as reshape(expr, expr.numel(),1)
  
      a c \n
      b d \n
    
      turns into
    
      a \n
      b \n
      c \n
      d \n
    
  */
  template<typename DataType>
  Matrix<DataType> vec(const Matrix<DataType>& a);

  /** \brief Returns a flattened version of the Matrix, preserving only nonzeros
   */
  template<typename DataType>
  Matrix<DataType> vecNZ(const Matrix<DataType>& a);

  /** \brief Construct a matrix from a list of list of blocks.
   */
  template<typename DataType>
  Matrix<DataType> blockcat(const std::vector< std::vector<Matrix<DataType> > > &v);

#ifndef SWIG
  /** \brief Construct a matrix from 4 blocks
   */
  template<typename DataType>
  Matrix<DataType> blockcat(const Matrix<DataType> &A,const Matrix<DataType> &B,const Matrix<DataType> &C,const Matrix<DataType> &D);
#endif // SWIG

  /** \brief Concatenate a list of matrices vertically
   * Alternative terminology: vertical stack, vstack, vertical append, [a;b]
   *
   *   horzcat(horzsplit(x,...)) = x
   */
  template<typename DataType>
  Matrix<DataType> horzcat(const std::vector<Matrix<DataType> > &v);

  /** \brief  split vertically, retaining groups of cols
   * \param offset List of all start cols for each group
   *      the last col group will run to the end.
   *
   *   horzcat(horzsplit(x,...)) = x
   */
  template<typename DataType>
  std::vector<Matrix<DataType> > horzsplit(const Matrix<DataType> &v, const std::vector<int>& offset);

  /** \brief  split vertically, retaining fixed-sized groups of cols
   * \param incr Size of each group of cols
   *
   *   horzcat(horzsplit(x,...)) = x
   */
  template<typename DataType>
  std::vector<Matrix<DataType> > horzsplit(const Matrix<DataType> &v, int incr=1);

  /** \brief Concatenate a list of matrices horizontally
   * Alternative terminology: horizontal stack, hstack, horizontal append, [a b]
   *
   *   vertcat(vertsplit(x,...)) = x
   */
  template<typename DataType>
  Matrix<DataType> vertcat(const std::vector<Matrix<DataType> > &v);

  /** \brief  split horizontally, retaining groups of rows
   * \param output_offset List of all start rows for each group
   *      the last row group will run to the end.
   *
   *   vertcat(vertsplit(x,...)) = x
   */
  template<typename DataType>
  std::vector<Matrix<DataType> > vertsplit(const Matrix<DataType> &v, const std::vector<int>& offset);

  /** \brief  split horizontally, retaining fixed-sized groups of rows
   * \param incr Size of each group of rows
   *
   *   vertcat(vertsplit(x,...)) = x
   */
  template<typename DataType>
  std::vector<Matrix<DataType> > vertsplit(const Matrix<DataType> &v, int incr=1);


  /** \brief  chop up into blocks
   * \brief vert_offset Defines the boundaries of the block rows
   * \brief horz_offset Defines the boundaries of the block columns
   *
   *   blockcat(blocksplit(x,...,...)) = x
   */
  template<typename DataType>
  std::vector< std::vector< Matrix<DataType> > > blocksplit(const Matrix<DataType>& x, const std::vector<int>& vert_offset, const std::vector<int>& horz_offset);

  /** \brief  chop up into blocks
   * \brief vert_incr Defines the increment for block boundaries in row dimension
   * \brief horz_incr Defines the increment for block boundaries in column dimension
   *
   *   blockcat(blocksplit(x,...,...)) = x
   */
  template<typename DataType>
  std::vector< std::vector< Matrix<DataType> > > blocksplit(const Matrix<DataType>& x, int vert_incr = 1, int horz_incr = 1);

#ifndef SWIG
  template<typename DataType>
  Matrix<DataType> vertcat(const Matrix<DataType> &x, const Matrix<DataType> &y);

  template<typename DataType>
  Matrix<DataType> horzcat(const Matrix<DataType> &x, const Matrix<DataType> &y);
#endif // SWIG

  template<typename DataType>
  /** \brief  concatenate vertically while vectorizing all arguments with vec */
  Matrix<DataType> veccat(const std::vector< Matrix<DataType> >& comp);

  template<typename DataType>
  /** \brief  concatenate vertically while vectorizing all arguments with vecNZ */
  Matrix<DataType> vecNZcat(const std::vector< Matrix<DataType> >& comp);

  /** \brief Inner product of two matrices
      Equals
      \code
      sumAll(x*y)
      \endcode
      with x and y matrices of the same dimension
  */
  template<typename DataType>
  Matrix<DataType> inner_prod(const Matrix<DataType> &x, const Matrix<DataType> &y); // inner product

  /** \brief Outer product of two vectors
      Equals
      \code
      x*transpose(y)
      \endcode
      with x and y vectors
  */
  template<typename DataType>
  Matrix<DataType> outer_prod(const Matrix<DataType> &x, const Matrix<DataType> &y);

  /** \brief  QR factorization using the modified Gram-Schmidt algorithm 
   * More stable than the classical Gram-Schmidt, but may break down if the rows of A are nearly linearly dependent
   * See J. Demmel: Applied Numerical Linear Algebra (algorithm 3.1.). 
   * Note that in SWIG, Q and R are returned by value. */
  template<typename DataType>
#ifndef SWIG
  void qr(const Matrix<DataType>& A, Matrix<DataType>& Q, Matrix<DataType>& R);
#else // SWIG
  void qr(const Matrix<DataType>& A, Matrix<DataType>& OUTPUT, Matrix<DataType>& OUTPUT);
#endif

  /** \brief Computes the nullspace of a matrix A
  *
  * Finds Z m-by-(m-n) such that AZ = 0 
  * with A n-by-m with m > n
  *
  * Assumes A is full rank
  *
  * Inspired by Numerical Methods in Scientific Computing by Ake Bjorck
  */
  template<typename DataType>
  Matrix<DataType> nullspace(const Matrix<DataType>& A);

  /** \brief  Solve a system of equations: A*x = b 
      The solve routine works similar to Matlab's backslash when A is square and nonsingular. The algorithm
      used is the following:
      1. A simple forward or backward substitution if A is upper or lower triangular
      2. If the linear system is at most 3-by-3, form the inverse via minor expansion and multiply
      3. Permute the variables and equations as to get a (structurally) nonzero diagonal, then perform
      a QR factorization without pivoting and solve the factorized system.

      Note 1: If there are entries of the linear system known to be zero, these will be removed. Elements
      that are very small, or will evaluate to be zero, can still cause numerical errors, due to the lack
      of pivoting (which is not possible since cannot compare the size of entries)

      Note 2: When permuting the linear system, a BLT (block lower triangular) transformation is formed.
      Only the permutation part of this is however used. An improvement would be to solve block-by-block
      if there are multiple BLT blocks.
  
  */
  template<typename DataType>
  Matrix<DataType> solve(const Matrix<DataType>& A, const Matrix<DataType>& b);
  
  /** \brief Computes the Moore-Penrose pseudo-inverse
  * 
  * If the matrix A is fat (size2>size1), mul(A,pinv(A)) is unity.
  * If the matrix A is slender (size1<size2), mul(pinv(A),A) is unity.
  *
  */
  template<typename DataType>
  Matrix<DataType> pinv(const Matrix<DataType>& A);
  
  /** \brief Solve a system of equations: A*x = b 
  */
  Matrix<double> solve(const Matrix<double>& A, const Matrix<double>& b, linearSolverCreator lsolver, const Dictionary& dict = Dictionary());
  
  
  /** \brief Computes the Moore-Penrose pseudo-inverse
  * 
  * If the matrix A is fat (size1>size2), mul(A,pinv(A)) is unity.
  * If the matrix A is slender (size2<size1), mul(pinv(A),A) is unity.
  *
  */
  Matrix<double> pinv(const Matrix<double>& A,linearSolverCreator lsolver, const Dictionary& dict = Dictionary());
  
  /** \brief Kronecker tensor product
  *
  * Creates a block matrix in which each element (i,j) is a_ij*b 
  */
  template<typename DataType>
  Matrix<DataType> kron(const Matrix<DataType>& a, const Matrix<DataType>& b);

  /** \brief  Frobenius norm  */
  template<typename DataType>
  Matrix<DataType> norm_F(const Matrix<DataType> &x);

  /** \brief  2-norm  */
  template<typename DataType>
  Matrix<DataType> norm_2(const Matrix<DataType> &x);

  /** \brief 1-norm  */
  template<typename DataType>
  Matrix<DataType> norm_1(const Matrix<DataType> &x);

  /** \brief Infinity-norm */
  template<typename DataType>
  Matrix<DataType> norm_inf(const Matrix<DataType> &x);

  /// Return summation of all elements
  template<typename DataType>
  Matrix<DataType> sumAll(const Matrix<DataType> &x); 

  /** \brief Return a col-wise summation of elements */
  template<typename DataType>
  Matrix<DataType> sumCols(const Matrix<DataType> &x);

  /** \brief Return a row-wise summation of elements */
  template<typename DataType>
  Matrix<DataType> sumRows(const Matrix<DataType> &x);

#ifdef SWIG
  /// Returns true only if every element in the matrix is true
  template<typename DataType>
  DataType all(const Matrix<DataType> &x); 

  /// Returns true if any element in the matrix is true
  template<typename DataType>
  DataType any(const Matrix<DataType> &x); 
#endif //SWIG

  /** \brief Repeat matrix A n times vertically and m times horizontally */
  template<typename DataType>
  Matrix<DataType> repmat(const Matrix<DataType> &A, int n, int m);

  /** \brief  Evaluate a polynomial with coefficeints p in x */
  template<typename DataType>
  Matrix<DataType> polyval(const Matrix<DataType>& p, const Matrix<DataType>& x);

  /** \brief   Get the diagonal of a matrix or construct a diagonal
      When the input is square, the diagonal elements are returned.
      If the input is vector-like, a diagonal matrix is constructed with it. */
  template<typename DataType>
  Matrix<DataType> diag(const Matrix<DataType> &A);

  /** \brief   Construct a matrix with given block on the diagonal */
  template<typename DataType>
  Matrix<DataType> blkdiag(const std::vector< Matrix<DataType> > &A);

  /** \brief  Unite two matrices no overlapping sparsity */
  template<typename DataType>
  Matrix<DataType> unite(const Matrix<DataType>& A, const Matrix<DataType>& B);

#ifndef SWIGOCTAVE
  /** \brief  Make a matrix dense */
  template<typename DataType>
  Matrix<DataType> full(const Matrix<DataType>& A);

  /** \brief  Make a matrix sparse by removing numerical zeros*/
  template<typename DataType>
  Matrix<DataType> sparse(const Matrix<DataType>& A, double tol=0);
#endif // SWIGOCTAVE

  /// same as: res += mul(A,v)
  template<typename DataType>
  void addMultiple(const Matrix<DataType>& A, const std::vector<DataType>& v, std::vector<DataType>& res, bool trans_A=false);

  /// Get a pointer to the data contained in the vector
  template<typename DataType>
  DataType* getPtr(Matrix<DataType> &v);
  
  /// Get a pointer to the data contained in the vector
  template<typename DataType>
  const DataType* getPtr(const Matrix<DataType> &v);

  /** \brief Create a new matrix with a given sparsity pattern but with the nonzeros taken from an existing matrix */
  template<typename DataType>
  Matrix<DataType> project(const Matrix<DataType>& A, const Sparsity& sparsity);

  /// Obtain the structural rank of a sparsity-pattern
  template<typename DataType>
  int sprank(const Matrix<DataType>& A);

#ifndef WITHOUT_PRE_1_9_X
  /** \brief [DEPRECATED]
   */
  //@{
  template<typename DataType> void makeDense(Matrix<DataType>& A){ A.densify();}
  template<typename DataType> Matrix<DataType> densify(const Matrix<DataType>& A){ Matrix<DataType> ret(A); ret.densify(); return ret;}
  template<typename DataType> void makeSparse(Matrix<DataType>& A, double tol=0){ A.sparsify(tol);}
  template<typename DataType> bool isDense(const Matrix<DataType>& ex){ return ex.isDense();}
  template<typename DataType> bool isEmpty(const Matrix<DataType>& ex){ return ex.isEmpty(); }
  template<typename DataType> bool isTril(const Matrix<DataType> &A){ return A.isTril(); }
  template<typename DataType> bool isTriu(const Matrix<DataType> &A){ return A.isTriu(); }
  template<typename DataType> bool isScalar(const Matrix<DataType>& ex){ return ex.isScalar();}
  template<typename DataType> bool isRegular(const Matrix<DataType>& ex){ return ex.isRegular();}
  template<typename DataType> bool isConstant(const Matrix<DataType>& ex){ return ex.isConstant();}
  template<typename DataType> bool isInteger(const Matrix<DataType>& ex){ return ex.isInteger();}
  template<typename DataType> bool isZero(const Matrix<DataType>& ex){ return ex.isZero();}
  template<typename DataType> bool isOne(const Matrix<DataType>& ex){ return ex.isOne();}
  template<typename DataType> bool isMinusOne(const Matrix<DataType>& ex){ return ex.isMinusOne();}
  template<typename DataType> bool isIdentity(const Matrix<DataType>& ex){ return ex.isIdentity();}
  template<typename DataType> int nnz(const Matrix<DataType>& ex) { return ex.size();}
  template<typename DataType> bool hasNonStructuralZeros(const Matrix<DataType>& A){ return A.hasNonStructuralZeros();}
  template<typename DataType>  Matrix<DataType> trans(const Matrix<DataType> &x){ return transpose(x);}
 //@}
#endif

} // namespace CasADi

// Global namespace

#ifndef SWIG
#include <iterator>

namespace CasADi{
  // Implementations

  template<typename DataType>
  Matrix<DataType> transpose(const Matrix<DataType> &x){
    return x.T();
  }

  template<typename DataType>
  Matrix<DataType> mul(const Matrix<DataType> &x, const Matrix<DataType> &y, const Sparsity &sp_z){
    return x.mul(y,sp_z);
  }

  template<typename DataType>
  Matrix<DataType> mul(const std::vector< Matrix<DataType> > &args){
    casadi_assert_message(args.size()>=1,"mul(std::vector< Matrix<DataType> > &args): supplied list must not be empty.");
    if (args.size()==1) return args[0];
    Matrix<DataType> ret = args[0].mul(args[1]);
    for (int i=2;i<args.size();++i) {
      ret = ret.mul(args[i]);
    }
    return ret;
  }

  template<typename DataType>
  DataType det(const Matrix<DataType>& a){
    int n = a.size2();
    casadi_assert_message(n == a.size1(),"matrix must be square");

    // Trivial return if scalar
    if(a.isScalar()) return a.toScalar();

    // Trivial case 2 x 2
    if(n==2) return a.elem(0,0) * a.elem(1,1) - a.elem(0,1) * a.elem(1,0);
  
    // Return expression
    Matrix<DataType> ret = 0;
  
    // Find out which is the best direction to expand along

    // Build up an IMatrix with ones on the non-zeros
    Matrix<int> sp = IMatrix(a.sparsity(),1);
  
    // Have a count of the nonzeros for each row
    Matrix<int> row_count = sumCols(sp);
  
    // A blank row? determinant is structurally zero
    if (!row_count.isDense()) return 0;

    // Have a count of the nonzeros for each col
    Matrix<int> col_count = sumRows(sp).T();
  
    // A blank col? determinant is structurally zero
    if (!row_count.isDense()) return 0;
  
    int min_row = std::distance(row_count.data().begin(), std::min_element(row_count.data().begin(),row_count.data().end()));
    int min_col = std::distance(col_count.data().begin(), std::min_element(col_count.data().begin(),col_count.data().end()));
  
    if (min_row <= min_col) {
      // Expand along row j
      int j = row_count.sparsity().row(min_row);
    
      Matrix<DataType> row = a(j,range(n));

      std::vector< int > col_i = row.sparsity().getCol();

      for(int k=0; k<row.size(); ++k) {
        // Sum up the cofactors
        ret += row.at(k)*cofactor(a,col_i.at(k),j);
      }
      return ret.toScalar();
    } else {
      // Expand along col i
      int i = col_count.sparsity().row(min_col);

      Matrix<DataType> col = a(range(n),i);
    
      const std::vector< int > &row_i = col.sparsity().row();

      for(int k=0; k<col.size(); ++k) {
        // Sum up the cofactors
        ret += col.at(k)*cofactor(a,i,row_i.at(k));
      }
      return ret.toScalar();
    }
 
  }

  template<typename DataType>
  DataType getMinor(const Matrix<DataType> &x, int i, int j){
    int n = x.size2();
    casadi_assert_message(n == x.size1(), "getMinor: matrix must be square");

    // Trivial return if scalar
    if(n==1) return 1;

    // Remove col i and row j
    Matrix<DataType> M = Matrix<DataType>::sparse(n-1,n-1);
  
    std::vector<int> col = x.sparsity().getCol();
    const std::vector<int> &row = x.sparsity().row();

    for(int k=0;k<x.size();++k) {
      int i1 = col[k];
      int j1 = row[k];

      if(i1 == i || j1 == j)
        continue;

      int i2 = (i1<i)?i1:i1-1;
      int j2 = (j1<j)?j1:j1-1;

      M(j2,i2) = x(j1,i1);
    }
    return det(M);
  }

  template<typename DataType>
  DataType cofactor(const Matrix<DataType> &x, int i, int j){

    // Calculate the i,j minor
    DataType minor_ij = getMinor(x,i,j);
    // Calculate the cofactor
    int sign_i = 1-2*((i+j) % 2);

    return sign_i * minor_ij;
  }

  template<typename DataType>
  Matrix<DataType> adj(const Matrix<DataType>& a){
    int n = a.size2();
    casadi_assert_message(n == a.size1(),"adj: matrix must be square");

    // Temporary placeholder
    DataType temp;
  
    // Cofactor matrix
    Matrix<DataType> C = Matrix<DataType>::sparse(n,n);
    for(int i=0; i<n; ++i)
      for(int j=0; j<n; ++j) {
        temp = cofactor(a,i,j);
        if (!casadi_limits<DataType>::isZero(temp))
          C(j,i) = temp;
      }
  
    return C.T();
  }

  template<typename DataType>
  Matrix<DataType> inv(const Matrix<DataType>& a){
    // laplace formula
    return adj(a)/det(a);
  }

  template<typename DataType>
  Matrix<DataType> reshape(const Matrix<DataType>& a, int nrow, int ncol){
    Sparsity sp = a.sparsity().reshape(nrow,ncol);
    return Matrix<DataType>(sp,a.data());
  }

  template<typename DataType>
  Matrix<DataType> reshape(const Matrix<DataType>& a, std::pair<int,int> rc){
    return reshape(a,rc.first,rc.second);
  }

  template<typename DataType>
  Matrix<DataType> reshape(const Matrix<DataType>& x, const Sparsity& sp){
    // quick return if already the right shape
    if(sp==x.sparsity())
      return x;
  
    // make sure that the number of zeros agree
    casadi_assert(x.size()==sp.size());
  
    return Matrix<DataType>(sp,x.data());
  }

  template<typename DataType>
  DataType trace(const Matrix<DataType>& a){
    casadi_assert_message(a.size2() == a.size1(), "trace: must be square");
    DataType res=0;
    for (int i=0; i< a.size2(); i ++) {
      res+=a.elem(i,i);
    }
    return res;
  }

  template<typename DataType>
  Matrix<DataType> vec(const Matrix<DataType>& a){
    Matrix<DataType> ret = reshape(a,a.numel(),1);
    return ret;
  }

  template<typename DataType>
  Matrix<DataType> vecNZ(const Matrix<DataType>& a){
    return Matrix<DataType>(a.data());
  }
  
  template<typename DataType>
  Matrix<DataType> blockcat(const std::vector< std::vector<Matrix<DataType> > > &v) {
    std::vector< Matrix<DataType> > ret;
    for(int i=0; i<v.size(); ++i)
      ret.push_back(horzcat(v[i]));
    return vertcat(ret);
  }

  template<typename DataType>
  Matrix<DataType> blockcat(const Matrix<DataType> &A,const Matrix<DataType> &B,const Matrix<DataType> &C,const Matrix<DataType> &D) {
    return vertcat(horzcat(A,B),horzcat(C,D));
  }

  template<typename DataType>
  Matrix<DataType> horzcat(const std::vector<Matrix<DataType> > &v){
    Matrix<DataType> ret;
    for(int i=0; i<v.size(); ++i)
      ret.appendColumns(v[i]);
    return ret;
  }

  template<typename DataType>
  std::vector<Matrix<DataType> > horzsplit(const Matrix<DataType> &v, const std::vector<int>& offset) {
    // Consistency check
    casadi_assert(offset.size()>=1);
    casadi_assert(offset.front()==0);
    casadi_assert_message(offset.back()<=v.size2(),"horzsplit(const Matrix<DataType> &v, const std::vector<int>& offset): Last elements of offset (" << offset.back() << ") must be at maximum the number of cols in v (" << v.size2() << ")");
    casadi_assert(isMonotone(offset));
  
    std::vector<Matrix<DataType> > ret;
  
    // Obtain sparsity pattern
    const std::vector<int> & colind = v.sparsity().colind();
    const std::vector<int> & row = v.sparsity().row();
  
    for(int i=0; i<offset.size(); ++i) {
      int start = offset[i];
      int stop = i+1 < offset.size() ? offset[i+1] : v.size2(); 
  
      // colind for the submatrix: a portion of the original colind, 
      // but with a common offset substracted such that colind_s[0]==0
      std::vector<int> colind_s(stop-start+1,-colind[start]);
      std::transform(colind.begin()+start,colind.begin()+stop+1,colind_s.begin(),colind_s.begin(),std::plus<int>());
    
      // row for the submatrix: a portion of the original row
      std::vector<int> row_s(colind[stop]-colind[start]);
      std::copy(row.begin()+colind[start],row.begin()+colind[stop],row_s.begin());
    
      Sparsity s = Sparsity(v.size1(),stop-start,colind_s,row_s);
      Matrix<DataType> r(s);
    
      // data for the submatrix: a portion of the original data
      std::copy(v.begin()+colind[start],v.begin()+colind[stop],r.begin());
    
      // Append submatrix to list
      ret.push_back(r);
    }
    return ret;
  }

  template<typename DataType>
  std::vector<Matrix<DataType> > horzsplit(const Matrix<DataType> &v, int incr) {
    casadi_assert(incr>=1);
    return horzsplit(v,range(0,v.size2(),incr));
  }


  template<typename DataType>
  Matrix<DataType> vertcat(const std::vector<Matrix<DataType> > &v){
    Matrix<DataType> ret;
    for(int i=0; i<v.size(); ++i)
      ret.appendColumns(v[i].T());
    return ret.T();  
  }

  template<typename DataType>
  std::vector< Matrix<DataType> > vertsplit(const Matrix<DataType>& x, const std::vector<int>& offset){
    std::vector< Matrix<DataType> > ret = horzsplit(x.T(),offset);
    Matrix<DataType> (*transposeT)(const Matrix<DataType>& x) = transpose;
    std::transform(ret.begin(),ret.end(),ret.begin(),transposeT);
    return ret;
  }
  
  template<typename DataType>
  std::vector< Matrix<DataType> > vertsplit(const Matrix<DataType>& x, int incr){
    casadi_assert(incr>=1);
    return vertsplit(x,range(0,x.size1(),incr));
  }

  template<typename DataType>
  std::vector< std::vector< Matrix<DataType> > > blocksplit(const Matrix<DataType>& x, const std::vector<int>& vert_offset, const std::vector<int>& horz_offset) {
    std::vector< Matrix<DataType> > rows = vertsplit(x,vert_offset);
    std::vector< std::vector< Matrix<DataType> > > ret;
    for (int i=0;i<rows.size();++i) {
      ret.push_back(horzsplit(rows[i],horz_offset));
    }
    return ret;
  }

  template<typename DataType>
  std::vector< std::vector< Matrix<DataType> > > blocksplit(const Matrix<DataType>& x, int vert_incr, int horz_incr) {
    casadi_assert(horz_incr>=1);
    casadi_assert(vert_incr>=1);
    return blocksplit(x,range(0,x.size1(),vert_incr),range(0,x.size2(),horz_incr));
  }

  template<typename DataType>
  Matrix<DataType> horzcat(const Matrix<DataType> &x, const Matrix<DataType> &y){
    Matrix<DataType> xy = x;
    xy.appendColumns(y);
    return xy;
  }

  template<typename DataType>
  Matrix<DataType> vertcat(const Matrix<DataType> &x, const Matrix<DataType> &y){
    return horzcat(x.T(),y.T()).T();
  }
  
  template<typename DataType>
  Matrix<DataType> veccat(const std::vector< Matrix<DataType> >& comp) {
    Matrix<DataType> (&f)(const Matrix<DataType>&) = vec;
    return vertcat(applymap(f,comp));
  }
  
  template<typename DataType>
  Matrix<DataType> vecNZcat(const std::vector< Matrix<DataType> >& comp) {
    Matrix<DataType> (&f)(const Matrix<DataType>&) = vecNZ;
    return vertcat(applymap(f,comp));
  }

  template<typename DataType>
  Matrix<DataType> inner_prod(const Matrix<DataType> &x, const Matrix<DataType> &y){
    casadi_assert_message(x.shape()==y.shape(), "inner_prod: Dimension mismatch");
    return sumAll(x*y);
  }

  template<typename DataType>
  Matrix<DataType> outer_prod(const Matrix<DataType> &x, const Matrix<DataType> &y){
    return mul(x,y.T());  
  }

  template<typename DataType>
  Matrix<DataType> sumAll(const Matrix<DataType> &x) {
    // Quick return if empty
    if (x.isEmpty()) return Matrix<DataType>::sparse(1,1);
    // Sum non-zero elements
    DataType res=0;
    for(int k=0; k<x.size(); k++){
      res += x.data()[k];
    }
    return res;
  }

  template<typename DataType>
  Matrix<DataType> sumCols(const Matrix<DataType> &x) {
    return mul(x,Matrix<DataType>::ones(x.size2(),1));
  }

  template<typename DataType>
  Matrix<DataType> sumRows(const Matrix<DataType> &x) {
    return mul(Matrix<DataType>::ones(1,x.size1()),x);
  }

  template<typename DataType>
  DataType all(const Matrix<DataType> &x) {
    if (!x.isDense()) return false;
    DataType ret=1;
    for (int i=0;i<x.size();++i) {
      ret = ret && x.at(i)==1;
    }
    return ret;
  }

  template<typename DataType>
  DataType any(const Matrix<DataType> &x) {
    if (!x.isDense()) return false;
    DataType ret=0;
    for (int i=0;i<x.size();++i) {
      ret = ret || x.at(i)==1;
    }
    return ret;
  }


  template<typename DataType>
  Matrix<DataType> norm_1(const Matrix<DataType>& x){
    return sumAll(fabs(x));
  }

  template<typename DataType>
  Matrix<DataType> norm_2(const Matrix<DataType>& x){
    if(x.isVector()){
      return norm_F(x);
    } else {
      casadi_error("2-norms currently only supported for vectors. Did you intend to calculate a Frobenius norms (norm_F)?");
    }
  }

  template<typename DataType>
  Matrix<DataType> norm_F(const Matrix<DataType>& x){
    return sqrt(1.0*sumAll(x*x));
  }

  template<typename DataType>
  Matrix<DataType> norm_inf(const Matrix<DataType>& x){
    // Get largest element by absolute value
    DataType s = 0;
    for(typename std::vector<DataType>::const_iterator i=x.begin(); i!=x.end(); ++i){
      s = fmax(s,DataType(abs(*i)));
    }
    
    return s;
  }

  template<typename DataType>
  void qr(const Matrix<DataType>& A, Matrix<DataType>& Q, Matrix<DataType> &R){
    // The following algorithm is taken from J. Demmel: Applied Numerical Linear Algebra (algorithm 3.1.)
    casadi_assert_message(A.size1()>=A.size2(), "qr: fewer rows than columns");

    // compute Q and R column by column
    Q = R = Matrix<DataType>();
    for(int i=0; i<A.size2(); ++i){
      // Initialize qi to be the i-th column of A
      Matrix<DataType> ai = A(ALL,i);
      Matrix<DataType> qi = ai;
      // The i-th column of R
      Matrix<DataType> ri = Matrix<DataType>::sparse(A.size2(),1);
  
      // subtract the projection of qi in the previous directions from ai
      for(int j=0; j<i; ++j){
      
        // Get the j-th column of Q
        Matrix<DataType> qj = Q(ALL,j);

        ri(j,0) = mul(qi.T(),qj); // Modified Gram-Schmidt
        // ri[j] = inner_prod(qj,ai); // Classical Gram-Schmidt
     
        // Remove projection in direction j
        if (ri.hasNZ(j,0))
          qi -= ri(j,0) * qj;
      }

      // Normalize qi
      ri(i,0) = norm_2(qi);
      qi /= ri(i,0);

      // Update R and Q
      Q.appendColumns(qi);
      R.appendColumns(ri);
    }
  }
  
  template<typename DataType>
  Matrix<DataType> nullspace(const Matrix<DataType>& A) {
    int n = A.size1();
    int m = A.size2();
    
    Matrix<DataType> X = A;
    
    casadi_assert_message(m>=n,"nullspace(A): expecting a flat matrix (more columns than rows), but got " << A.dimString() << ".");
    
    Matrix<DataType> seed = DMatrix::eye(m)(Slice(0,m),Slice(n,m));

    std::vector< Matrix<DataType> > us;
    std::vector< Matrix<DataType> > betas;
    
    Matrix<DataType> beta;
    
    for (int i=0;i<n;++i) {
      Matrix<DataType> x = X(i,Slice(i,m));
      Matrix<DataType> u = Matrix<DataType>(x);
      Matrix<DataType> sigma = sqrt(sumCols(x*x));
      const Matrix<DataType>& x0 = x(0,0);
      u(0,0) = 1;
      
      Matrix<DataType> b = -copysign(sigma,x0);
      
      u(Slice(0),Slice(1,m-i))*= 1/(x0-b);
      beta = 1-x0/b;
      
      X(Slice(i,n),Slice(i,m))-= beta*mul(mul(X(Slice(i,n),Slice(i,m)),u.T()),u);
      us.push_back(u);
      betas.push_back(beta);
    }
    
    for (int i=n-1;i>=0;--i) {
      seed(Slice(i,m),Slice(0,m-n)) -= betas[i]*mul(us[i].T(),mul(us[i],seed(Slice(i,m),Slice(0,m-n))));
    }
    
    return seed;

  }

  template<typename DataType>
  Matrix<DataType> solve(const Matrix<DataType>& A, const Matrix<DataType>& b){
    // check dimensions
    casadi_assert_message(A.size1() == b.size1(),"solve Ax=b: dimension mismatch: b has " << b.size1() << " rows while A has " << A.size1() << ".");
    casadi_assert_message(A.size1() == A.size2(),"solve: A not square but " << A.dimString());

    if(A.isTril()){
      // forward substitution if lower triangular
      Matrix<DataType> x = b;
      const std::vector<int> & Arow = A.row();
      const std::vector<int> & Acolind = A.colind();
      const std::vector<DataType> & Adata = A.data();
      for(int i=0; i<A.size2(); ++i){ // loop over columns forwards
        for(int k=0; k<b.size2(); ++k){ // for every right hand side
          if(!x.hasNZ(i,k)) continue;
          x(i,k) /= A(i,i);
          for(int kk=Acolind[i+1]-1; kk>=Acolind[i] && Arow[kk]>i; --kk){
            int j = Arow[kk]; 
            x(j,k) -= Adata[kk]*x(i,k);
          }
        }
      }
      return x;
    } else if(A.isTriu()){
      // backward substitution if upper triangular
      Matrix<DataType> x = b;
      const std::vector<int> & Arow = A.row();
      const std::vector<int> & Acolind = A.colind();
      const std::vector<DataType> & Adata = A.data();
      for(int i=A.size2()-1; i>=0; --i){ // loop over columns backwards
        for(int k=0; k<b.size2(); ++k){ // for every right hand side
          if(!x.hasNZ(i,k)) continue;
          x(i,k) /= A(i,i);
          for(int kk=Acolind[i]; kk<Acolind[i+1] && Arow[kk]<i; ++kk){ 
            int j = Arow[kk];
            x(j,k) -= Adata[kk]*x(i,k);
          }
        }
      }
      return x;
    } else if(A.hasNonStructuralZeros()){

      // If there are structurally nonzero entries that are known to be zero, remove these and rerun the algorithm
      Matrix<DataType> A_sparse = A;
      A_sparse.sparsify();
      return solve(A_sparse,b);

    } else {
    
      // Make a BLT transformation of A
      std::vector<int> rowperm, colperm, rowblock, colblock, coarse_rowblock, coarse_colblock;
      A.sparsity().dulmageMendelsohn(rowperm, colperm, rowblock, colblock, coarse_rowblock, coarse_colblock);

      // Permute the right hand side
      Matrix<DataType> bperm = b(rowperm,ALL);

      // Permute the linear system
      Matrix<DataType> Aperm = A(rowperm,colperm);

      // Solution
      Matrix<DataType> xperm;

      // Solve permuted system
      if(Aperm.isTril()){
      
        // Forward substitution if lower triangular
        xperm = solve(Aperm,bperm);
      
      } else if(A.size2()<=3){
      
        // Form inverse by minor expansion and multiply if very small (up to 3-by-3)
        xperm = mul(inv(Aperm),bperm);

      } else {
      
        // Make a QR factorization
        Matrix<DataType> Q,R;
        qr(Aperm,Q,R);

        // Solve the factorized system (note that solve will now be fast since it is triangular)
        xperm = solve(R,mul(Q.T(),bperm));
      }

      // get the inverted column permutation
      std::vector<int> inv_colperm(colperm.size());
      for(int k=0; k<colperm.size(); ++k)
        inv_colperm[colperm[k]] = k;

      // Permute back the solution and return
      Matrix<DataType> x = xperm(inv_colperm,ALL);
      return x;
    }
  }
  
  template<typename DataType>
  Matrix<DataType> pinv(const Matrix<DataType>& A) {
    if (A.size2()>=A.size1()) {
      return solve(mul(A,A.T()),A).T();
    } else {
      return solve(mul(A.T(),A),A.T());
    }
  }
  
  template<typename DataType>
  Matrix<DataType> kron(const Matrix<DataType>& a, const Matrix<DataType>& b) {
    const Sparsity &a_sp = a.sparsity();
    Matrix<DataType> filler = Matrix<DataType>::sparse(b.shape());
    std::vector< std::vector< Matrix<DataType> > > blocks(a.size1(),std::vector< Matrix<DataType> >(a.size2(),filler));
    for (int i=0;i<a.size1();++i) {
      for (int j=0;j<a.size2();++j) {
        int k = a_sp.getNZ(i,j);
        if (k!=-1) {
          blocks[i][j] = a[k]*b;
        }
      }
    }
    return blockcat(blocks);
  }

  template<typename DataType>
  Matrix<DataType> repmat(const Matrix<DataType> &A, int n, int m){
    // First concatenate horizontally
    Matrix<DataType> col = horzcat(std::vector<Matrix<DataType> >(m, A));
  
    // Then vertically
    return vertcat(std::vector<Matrix<DataType> >(n, col));
  }

  template<typename DataType>
  Matrix<DataType> diag(const Matrix<DataType>&A){
    // Nonzero mapping
    std::vector<int> mapping;
    // Get the sparsity
    Sparsity sp = A.sparsity().getDiag(mapping);
  
    Matrix<DataType> ret = Matrix<DataType>(sp);
  
    for (int k=0;k<mapping.size();k++) ret[k] = A[mapping[k]];
    return ret;
  }

  /** \brief   Construct a matrix with given block on the diagonal */
  template<typename DataType>
  Matrix<DataType> blkdiag(const std::vector< Matrix<DataType> > &A) {
    std::vector<DataType> data;
  
    std::vector<Sparsity> sp;
    for (int i=0;i<A.size();++i) {
      data.insert(data.end(),A[i].data().begin(),A[i].data().end());
      sp.push_back(A[i].sparsity());
    }
  
    return Matrix<DataType>(blkdiag(sp),data);
  
  }

  template<typename DataType>
  Matrix<DataType> unite(const Matrix<DataType>& A, const Matrix<DataType>& B){
    // Join the sparsity patterns
    std::vector<unsigned char> mapping;
    Sparsity sp = A.sparsity().patternUnion(B.sparsity(),mapping);
  
    // Create return matrix
    Matrix<DataType> ret(sp);
  
    // Copy sparsity
    int elA=0, elB=0;
    for(int k=0; k<mapping.size(); ++k){
      if(mapping[k]==1){
        ret.data()[k] = A.data()[elA++];
      } else if(mapping[k]==2){
        ret.data()[k] = B.data()[elB++];
      } else {
        throw CasadiException("Pattern intersection not empty");
      }
    }
  
    casadi_assert(A.size()==elA);
    casadi_assert(B.size()==elB);
  
    return ret;
  }

  template<typename DataType>
  Matrix<DataType> full(const Matrix<DataType>& A){    
    Matrix<DataType> ret = A;
    ret.densify();
    return ret;
  }

  template<typename DataType>
  Matrix<DataType> sparse(const Matrix<DataType>& A, double tol){
    Matrix<DataType> ret(A);
    ret.sparsify(tol);
    return ret;
  }
  
  template<typename DataType>
  Matrix<DataType> polyval(const Matrix<DataType>& p, const Matrix<DataType>& x){
    casadi_assert_message(p.isDense(),"polynomial coefficients vector must be dense");
    casadi_assert_message(p.isVector() && p.size()>0,"polynomial coefficients must be a vector");
    Matrix<DataType> ret = p[0];
    for(int i=1; i<p.size(); ++i){
      ret = ret*x + p[i];
    }
    return ret;
  }

  template<typename DataType>
  void addMultiple(const Matrix<DataType>& A, const std::vector<DataType>& v, std::vector<DataType>& res, bool trans_A){
    // Get dimension and sparsity
    int d1=A.size2(), d2=A.size1();
    const std::vector<int> &colind=A.colind();
    const std::vector<int> &row=A.row();
    const std::vector<DataType>& data = A.data();

    // Assert consistent dimensions
    if(trans_A){
      casadi_assert(v.size()==d1);
      casadi_assert(res.size()==d2);
    } else {
      casadi_assert(v.size()==d2);
      casadi_assert(res.size()==d1);
    }

    // Carry out multiplication
    for(int i=0; i<d1; ++i){ // loop over cols
      for(int el=colind[i]; el<colind[i+1]; ++el){ // loop over the non-zero elements
        int j=row[el];  // row
        // Add scalar product
        if(trans_A){
          res[j] += v[i]*data[el];
        } else {
          res[i] += v[j]*data[el];
        }
      }
    }
  }

  template<typename DataType>
  DataType* getPtr(Matrix<DataType> &v){
    if(v.isEmpty())
      return 0;
    else
      return &v.front();
  }
  
  template<typename DataType>
  const DataType* getPtr(const Matrix<DataType> &v){
    if(v.isEmpty())
      return 0;
    else
      return &v.front();
  }

  template<typename DataType>
  Matrix<DataType> project(const Matrix<DataType>& A, const Sparsity& sparsity){
    // Check dimensions
    if(!(A.isEmpty() && sparsity.numel()==0)){
      casadi_assert_message(A.size2()==sparsity.size2() && A.size1()==sparsity.size1(),
                            "Shape mismatch. Expecting " << A.dimString() << ", but got " << 
                            sparsity.dimString() << " instead.");
    }
    
    // Return value
    Matrix<DataType> ret(sparsity,0);
    
    // Get the elements of the known matrix
    std::vector<int> known_ind = A.sparsity().getElements(false);
      
    // Find the corresponding nonzeros in the return matrix
    sparsity.getNZInplace(known_ind);
      
    // Set the element values
    const std::vector<DataType>& A_data = A.data();
    std::vector<DataType>& ret_data = ret.data();
    for(int k=0; k<known_ind.size(); ++k){
      if(known_ind[k]!=-1){
        ret_data[known_ind[k]] = A_data[k];
      }
    }
    return ret;
  }

  template<typename DataType>
  int sprank(const Matrix<DataType>& A) {
    return rank(A.sparsity());
  }
  
} // namespace CasADi


#endif //SWIG




#ifdef SWIG

// map the template name to the instantiated name
#define MTT_INST(DataType,function_name)                       \
  %template(function_name) CasADi::function_name <DataType >;

#ifndef WITHOUT_PRE_1_9_X
#define MATRIX_TOOLS_TEMPLATES_PRE_1_9_X(DataType)     \
  MTT_INST(DataType,trans)                             \
  MTT_INST(DataType,isConstant)                        \
  MTT_INST(DataType,isDense)                           \
  MTT_INST(DataType,isEmpty)                           \
  MTT_INST(DataType,isInteger)                         \
  MTT_INST(DataType,isScalar)                          \
  MTT_INST(DataType,isTril)                            \
  MTT_INST(DataType,isTriu)                            \
  MTT_INST(DataType,makeDense)                         \
  MTT_INST(DataType,densify)                           \
  MTT_INST(DataType,makeSparse)                        \
  MTT_INST(DataType,hasNonStructuralZeros)             \
  MTT_INST(DataType,isZero)                            \
  MTT_INST(DataType,isOne)                             \
  MTT_INST(DataType,isMinusOne)                        \
  MTT_INST(DataType,isIdentity)                        \
  MTT_INST(DataType,nnz)                               
#else
#define MATRIX_TOOLS_TEMPLATES_PRE_1_9_X(DataType)
#endif

// Define template instanciations
#define MATRIX_TOOLS_TEMPLATES_COMMON(DataType)        \
  MATRIX_TOOLS_TEMPLATES_PRE_1_9_X(DataType)           \
  MTT_INST(DataType,transpose)                         \
  MTT_INST(DataType,mul)                               \
  MTT_INST(DataType,det)                               \
  MTT_INST(DataType,getMinor)                          \
  MTT_INST(DataType,cofactor)                          \
  MTT_INST(DataType,adj)                               \
  MTT_INST(DataType,inv)                               \
  MTT_INST(DataType,reshape)                           \
  MTT_INST(DataType,vec)                               \
  MTT_INST(DataType,vecNZ)                             \
  MTT_INST(DataType,blockcat)                          \
  MTT_INST(DataType,blocksplit)                        \
  MTT_INST(DataType,vertcat)                           \
  MTT_INST(DataType,vertsplit)                         \
  MTT_INST(DataType,horzcat)                           \
  MTT_INST(DataType,horzsplit)                         \
  MTT_INST(DataType,inner_prod)                        \
  MTT_INST(DataType,outer_prod)                        \
  MTT_INST(DataType,norm_1)                            \
  MTT_INST(DataType,norm_2)                            \
  MTT_INST(DataType,norm_inf)                          \
  MTT_INST(DataType,norm_F)                            \
  MTT_INST(DataType,qr)                                \
  MTT_INST(DataType,nullspace)                         \
  MTT_INST(DataType,solve)                             \
  MTT_INST(DataType,pinv)                              \
  MTT_INST(DataType,repmat)                            \
  MTT_INST(DataType,unite)                             \
  MTT_INST(DataType,sumRows)                           \
  MTT_INST(DataType,sumCols)                           \
  MTT_INST(DataType,sumAll)                            \
  MTT_INST(DataType,trace)                             \
  MTT_INST(DataType,diag)                              \
  MTT_INST(DataType,blkdiag)                           \
  MTT_INST(DataType,polyval)                           \
  MTT_INST(DataType,addMultiple)                       \
  MTT_INST(DataType,veccat)                            \
  MTT_INST(DataType,vecNZcat)                          \
  MTT_INST(DataType,project)                           \
  MTT_INST(DataType,sprank)                            \
  MTT_INST(DataType,kron) 
#endif //SWIG

#ifdef SWIGOCTAVE
#define MATRIX_TOOLS_TEMPLATES(DataType) MATRIX_TOOLS_TEMPLATES_COMMON(DataType)
#else
#define MATRIX_TOOLS_TEMPLATES(DataType)               \
  MATRIX_TOOLS_TEMPLATES_COMMON(DataType)              \
  MTT_INST(DataType,sparse)                            \
  MTT_INST(DataType,full)
#endif //SWIGOCTAVE

#endif // MATRIX_TOOLS_HPP
