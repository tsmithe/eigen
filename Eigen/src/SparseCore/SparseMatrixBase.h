// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2008-2011 Gael Guennebaud <gael.guennebaud@inria.fr>
//
// Eigen is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// Eigen is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// Eigen. If not, see <http://www.gnu.org/licenses/>.

#ifndef EIGEN_SPARSEMATRIXBASE_H
#define EIGEN_SPARSEMATRIXBASE_H

/** \ingroup SparseCore_Module
  *
  * \class SparseMatrixBase
  *
  * \brief Base class of any sparse matrices or sparse expressions
  *
  * \tparam Derived
  *
  * This class can be extended with the help of the plugin mechanism described on the page
  * \ref TopicCustomizingEigen by defining the preprocessor symbol \c EIGEN_SPARSEMATRIXBASE_PLUGIN.
  */
template<typename Derived> class SparseMatrixBase : public EigenBase<Derived>
{
  public:

    typedef typename internal::traits<Derived>::Scalar Scalar;
    typedef typename internal::packet_traits<Scalar>::type PacketScalar;
    typedef typename internal::traits<Derived>::StorageKind StorageKind;
    typedef typename internal::traits<Derived>::Index Index;
    typedef typename internal::add_const_on_value_type_if_arithmetic<
                         typename internal::packet_traits<Scalar>::type
                     >::type PacketReturnType;

    typedef SparseMatrixBase StorageBaseType;
    typedef EigenBase<Derived> Base;
    
    template<typename OtherDerived>
    Derived& operator=(const EigenBase<OtherDerived> &other)
    {
      other.derived().evalTo(derived());
      return derived();
    }

    enum {

      RowsAtCompileTime = internal::traits<Derived>::RowsAtCompileTime,
        /**< The number of rows at compile-time. This is just a copy of the value provided
          * by the \a Derived type. If a value is not known at compile-time,
          * it is set to the \a Dynamic constant.
          * \sa MatrixBase::rows(), MatrixBase::cols(), ColsAtCompileTime, SizeAtCompileTime */

      ColsAtCompileTime = internal::traits<Derived>::ColsAtCompileTime,
        /**< The number of columns at compile-time. This is just a copy of the value provided
          * by the \a Derived type. If a value is not known at compile-time,
          * it is set to the \a Dynamic constant.
          * \sa MatrixBase::rows(), MatrixBase::cols(), RowsAtCompileTime, SizeAtCompileTime */


      SizeAtCompileTime = (internal::size_at_compile_time<internal::traits<Derived>::RowsAtCompileTime,
                                                   internal::traits<Derived>::ColsAtCompileTime>::ret),
        /**< This is equal to the number of coefficients, i.e. the number of
          * rows times the number of columns, or to \a Dynamic if this is not
          * known at compile-time. \sa RowsAtCompileTime, ColsAtCompileTime */

      MaxRowsAtCompileTime = RowsAtCompileTime,
      MaxColsAtCompileTime = ColsAtCompileTime,

      MaxSizeAtCompileTime = (internal::size_at_compile_time<MaxRowsAtCompileTime,
                                                      MaxColsAtCompileTime>::ret),

      IsVectorAtCompileTime = RowsAtCompileTime == 1 || ColsAtCompileTime == 1,
        /**< This is set to true if either the number of rows or the number of
          * columns is known at compile-time to be equal to 1. Indeed, in that case,
          * we are dealing with a column-vector (if there is only one column) or with
          * a row-vector (if there is only one row). */

      Flags = internal::traits<Derived>::Flags,
        /**< This stores expression \ref flags flags which may or may not be inherited by new expressions
          * constructed from this one. See the \ref flags "list of flags".
          */

      CoeffReadCost = internal::traits<Derived>::CoeffReadCost,
        /**< This is a rough measure of how expensive it is to read one coefficient from
          * this expression.
          */

      IsRowMajor = Flags&RowMajorBit ? 1 : 0,

      #ifndef EIGEN_PARSED_BY_DOXYGEN
      _HasDirectAccess = (int(Flags)&DirectAccessBit) ? 1 : 0 // workaround sunCC
      #endif
    };

    /** \internal the return type of MatrixBase::adjoint() */
    typedef typename internal::conditional<NumTraits<Scalar>::IsComplex,
                        CwiseUnaryOp<internal::scalar_conjugate_op<Scalar>, Eigen::Transpose<const Derived> >,
                        Transpose<const Derived>
                     >::type AdjointReturnType;


    typedef SparseMatrix<Scalar, Flags&RowMajorBit ? RowMajor : ColMajor> PlainObject;

#define EIGEN_CURRENT_STORAGE_BASE_CLASS Eigen::SparseMatrixBase
#   include "../plugins/CommonCwiseUnaryOps.h"
#   include "../plugins/CommonCwiseBinaryOps.h"
#   include "../plugins/MatrixCwiseUnaryOps.h"
#   include "../plugins/MatrixCwiseBinaryOps.h"
#   ifdef EIGEN_SPARSEMATRIXBASE_PLUGIN
#     include EIGEN_SPARSEMATRIXBASE_PLUGIN
#   endif
#   undef EIGEN_CURRENT_STORAGE_BASE_CLASS
#undef EIGEN_CURRENT_STORAGE_BASE_CLASS

#ifndef EIGEN_PARSED_BY_DOXYGEN
    /** This is the "real scalar" type; if the \a Scalar type is already real numbers
      * (e.g. int, float or double) then \a RealScalar is just the same as \a Scalar. If
      * \a Scalar is \a std::complex<T> then RealScalar is \a T.
      *
      * \sa class NumTraits
      */
    typedef typename NumTraits<Scalar>::Real RealScalar;

    /** \internal the return type of coeff()
      */
    typedef typename internal::conditional<_HasDirectAccess, const Scalar&, Scalar>::type CoeffReturnType;

    /** \internal Represents a matrix with all coefficients equal to one another*/
    typedef CwiseNullaryOp<internal::scalar_constant_op<Scalar>,Matrix<Scalar,Dynamic,Dynamic> > ConstantReturnType;

    /** type of the equivalent square matrix */
    typedef Matrix<Scalar,EIGEN_SIZE_MAX(RowsAtCompileTime,ColsAtCompileTime),
                          EIGEN_SIZE_MAX(RowsAtCompileTime,ColsAtCompileTime)> SquareMatrixType;

    inline const Derived& derived() const { return *static_cast<const Derived*>(this); }
    inline Derived& derived() { return *static_cast<Derived*>(this); }
    inline Derived& const_cast_derived() const
    { return *static_cast<Derived*>(const_cast<SparseMatrixBase*>(this)); }
#endif // not EIGEN_PARSED_BY_DOXYGEN

    /** \returns the number of rows. \sa cols(), RowsAtCompileTime */
    inline Index rows() const { return derived().rows(); }
    /** \returns the number of columns. \sa rows(), ColsAtCompileTime*/
    inline Index cols() const { return derived().cols(); }
    /** \returns the number of coefficients, which is \a rows()*cols().
      * \sa rows(), cols(), SizeAtCompileTime. */
    inline Index size() const { return rows() * cols(); }
    /** \returns the number of nonzero coefficients which is in practice the number
      * of stored coefficients. */
    inline Index nonZeros() const { return derived().nonZeros(); }
    /** \returns true if either the number of rows or the number of columns is equal to 1.
      * In other words, this function returns
      * \code rows()==1 || cols()==1 \endcode
      * \sa rows(), cols(), IsVectorAtCompileTime. */
    inline bool isVector() const { return rows()==1 || cols()==1; }
    /** \returns the size of the storage major dimension,
      * i.e., the number of columns for a columns major matrix, and the number of rows otherwise */
    Index outerSize() const { return (int(Flags)&RowMajorBit) ? this->rows() : this->cols(); }
    /** \returns the size of the inner dimension according to the storage order,
      * i.e., the number of rows for a columns major matrix, and the number of cols otherwise */
    Index innerSize() const { return (int(Flags)&RowMajorBit) ? this->cols() : this->rows(); }

    bool isRValue() const { return m_isRValue; }
    Derived& markAsRValue() { m_isRValue = true; return derived(); }

    SparseMatrixBase() : m_isRValue(false) { /* TODO check flags */ }

    
    template<typename OtherDerived>
    Derived& operator=(const ReturnByValue<OtherDerived>& other)
    {
      other.evalTo(derived());
      return derived();
    }


    template<typename OtherDerived>
    inline Derived& operator=(const SparseMatrixBase<OtherDerived>& other)
    {
      return assign(other.derived());
    }

    inline Derived& operator=(const Derived& other)
    {
//       if (other.isRValue())
//         derived().swap(other.const_cast_derived());
//       else
      return assign(other.derived());
    }

  protected:

    template<typename OtherDerived>
    inline Derived& assign(const OtherDerived& other)
    {
      const bool transpose = (Flags & RowMajorBit) != (OtherDerived::Flags & RowMajorBit);
      const Index outerSize = (int(OtherDerived::Flags) & RowMajorBit) ? other.rows() : other.cols();
      if ((!transpose) && other.isRValue())
      {
        // eval without temporary
        derived().resize(other.rows(), other.cols());
        derived().setZero();
        derived().reserve((std::max)(this->rows(),this->cols())*2);
        for (Index j=0; j<outerSize; ++j)
        {
          derived().startVec(j);
          for (typename OtherDerived::InnerIterator it(other, j); it; ++it)
          {
            Scalar v = it.value();
            if (v!=Scalar(0))
              derived().insertBackByOuterInner(j,it.index()) = v;
          }
        }
        derived().finalize();
      }
      else
      {
        assignGeneric(other);
      }
      return derived();
    }

    template<typename OtherDerived>
    inline void assignGeneric(const OtherDerived& other)
    {
      //const bool transpose = (Flags & RowMajorBit) != (OtherDerived::Flags & RowMajorBit);
      eigen_assert(( ((internal::traits<Derived>::SupportedAccessPatterns&OuterRandomAccessPattern)==OuterRandomAccessPattern) ||
                  (!((Flags & RowMajorBit) != (OtherDerived::Flags & RowMajorBit)))) &&
                  "the transpose operation is supposed to be handled in SparseMatrix::operator=");

      enum { Flip = (Flags & RowMajorBit) != (OtherDerived::Flags & RowMajorBit) };

      const Index outerSize = other.outerSize();
      //typedef typename internal::conditional<transpose, LinkedVectorMatrix<Scalar,Flags&RowMajorBit>, Derived>::type TempType;
      // thanks to shallow copies, we always eval to a tempary
      Derived temp(other.rows(), other.cols());

      temp.reserve((std::max)(this->rows(),this->cols())*2);
      for (Index j=0; j<outerSize; ++j)
      {
        temp.startVec(j);
        for (typename OtherDerived::InnerIterator it(other.derived(), j); it; ++it)
        {
          Scalar v = it.value();
          if (v!=Scalar(0))
            temp.insertBackByOuterInner(Flip?it.index():j,Flip?j:it.index()) = v;
        }
      }
      temp.finalize();

      derived() = temp.markAsRValue();
    }

  public:

    template<typename Lhs, typename Rhs>
    inline Derived& operator=(const SparseSparseProduct<Lhs,Rhs>& product);

    template<typename Lhs, typename Rhs>
    inline void _experimentalNewProduct(const Lhs& lhs, const Rhs& rhs);

    friend std::ostream & operator << (std::ostream & s, const SparseMatrixBase& m)
    {
      if (Flags&RowMajorBit)
      {
        for (Index row=0; row<m.outerSize(); ++row)
        {
          Index col = 0;
          for (typename Derived::InnerIterator it(m.derived(), row); it; ++it)
          {
            for ( ; col<it.index(); ++col)
              s << "0 ";
            s << it.value() << " ";
            ++col;
          }
          for ( ; col<m.cols(); ++col)
            s << "0 ";
          s << std::endl;
        }
      }
      else
      {
        if (m.cols() == 1) {
          Index row = 0;
          for (typename Derived::InnerIterator it(m.derived(), 0); it; ++it)
          {
            for ( ; row<it.index(); ++row)
              s << "0" << std::endl;
            s << it.value() << std::endl;
            ++row;
          }
          for ( ; row<m.rows(); ++row)
            s << "0" << std::endl;
        }
        else
        {
          SparseMatrix<Scalar, RowMajorBit> trans = m.derived();
          s << trans;
        }
      }
      return s;
    }

    template<typename OtherDerived>
    Derived& operator+=(const SparseMatrixBase<OtherDerived>& other);
    template<typename OtherDerived>
    Derived& operator-=(const SparseMatrixBase<OtherDerived>& other);

    Derived& operator*=(const Scalar& other);
    Derived& operator/=(const Scalar& other);

    #define EIGEN_SPARSE_CWISE_PRODUCT_RETURN_TYPE \
      CwiseBinaryOp< \
        internal::scalar_product_op< \
          typename internal::scalar_product_traits< \
            typename internal::traits<Derived>::Scalar, \
            typename internal::traits<OtherDerived>::Scalar \
          >::ReturnType \
        >, \
        Derived, \
        OtherDerived \
      >

    template<typename OtherDerived>
    EIGEN_STRONG_INLINE const EIGEN_SPARSE_CWISE_PRODUCT_RETURN_TYPE
    cwiseProduct(const MatrixBase<OtherDerived> &other) const;

    // sparse * sparse
    template<typename OtherDerived>
    const typename SparseSparseProductReturnType<Derived,OtherDerived>::Type
    operator*(const SparseMatrixBase<OtherDerived> &other) const;

    // sparse * diagonal
    template<typename OtherDerived>
    const SparseDiagonalProduct<Derived,OtherDerived>
    operator*(const DiagonalBase<OtherDerived> &other) const;

    // diagonal * sparse
    template<typename OtherDerived> friend
    const SparseDiagonalProduct<OtherDerived,Derived>
    operator*(const DiagonalBase<OtherDerived> &lhs, const SparseMatrixBase& rhs)
    { return SparseDiagonalProduct<OtherDerived,Derived>(lhs.derived(), rhs.derived()); }

    /** dense * sparse (return a dense object unless it is an outer product) */
    template<typename OtherDerived> friend
    const typename DenseSparseProductReturnType<OtherDerived,Derived>::Type
    operator*(const MatrixBase<OtherDerived>& lhs, const Derived& rhs)
    { return typename DenseSparseProductReturnType<OtherDerived,Derived>::Type(lhs.derived(),rhs); }

    /** sparse * dense (returns a dense object unless it is an outer product) */
    template<typename OtherDerived>
    const typename SparseDenseProductReturnType<Derived,OtherDerived>::Type
    operator*(const MatrixBase<OtherDerived> &other) const;

    template<typename OtherDerived>
    Derived& operator*=(const SparseMatrixBase<OtherDerived>& other);

    #ifdef EIGEN2_SUPPORT
    // deprecated
    template<typename OtherDerived>
    typename internal::plain_matrix_type_column_major<OtherDerived>::type
    solveTriangular(const MatrixBase<OtherDerived>& other) const;

    // deprecated
    template<typename OtherDerived>
    void solveTriangularInPlace(MatrixBase<OtherDerived>& other) const;
    #endif // EIGEN2_SUPPORT

    template<int Mode>
    inline const SparseTriangularView<Derived, Mode> triangularView() const;

    template<unsigned int UpLo> inline const SparseSelfAdjointView<Derived, UpLo> selfadjointView() const;
    template<unsigned int UpLo> inline SparseSelfAdjointView<Derived, UpLo> selfadjointView();

    template<typename OtherDerived> Scalar dot(const MatrixBase<OtherDerived>& other) const;
    template<typename OtherDerived> Scalar dot(const SparseMatrixBase<OtherDerived>& other) const;
    RealScalar squaredNorm() const;
    RealScalar norm()  const;

    Transpose<Derived> transpose() { return derived(); }
    const Transpose<const Derived> transpose() const { return derived(); }
    const AdjointReturnType adjoint() const { return transpose(); }

    // sub-vector
    SparseInnerVectorSet<Derived,1> row(Index i);
    const SparseInnerVectorSet<Derived,1> row(Index i) const;
    SparseInnerVectorSet<Derived,1> col(Index j);
    const SparseInnerVectorSet<Derived,1> col(Index j) const;
    SparseInnerVectorSet<Derived,1> innerVector(Index outer);
    const SparseInnerVectorSet<Derived,1> innerVector(Index outer) const;

    // set of sub-vectors
    SparseInnerVectorSet<Derived,Dynamic> subrows(Index start, Index size);
    const SparseInnerVectorSet<Derived,Dynamic> subrows(Index start, Index size) const;
    SparseInnerVectorSet<Derived,Dynamic> subcols(Index start, Index size);
    const SparseInnerVectorSet<Derived,Dynamic> subcols(Index start, Index size) const;
    
    SparseInnerVectorSet<Derived,Dynamic> middleRows(Index start, Index size);
    const SparseInnerVectorSet<Derived,Dynamic> middleRows(Index start, Index size) const;
    SparseInnerVectorSet<Derived,Dynamic> middleCols(Index start, Index size);
    const SparseInnerVectorSet<Derived,Dynamic> middleCols(Index start, Index size) const;
    SparseInnerVectorSet<Derived,Dynamic> innerVectors(Index outerStart, Index outerSize);
    const SparseInnerVectorSet<Derived,Dynamic> innerVectors(Index outerStart, Index outerSize) const;

      /** \internal use operator= */
      template<typename DenseDerived>
      void evalTo(MatrixBase<DenseDerived>& dst) const
      {
        dst.setZero();
        for (Index j=0; j<outerSize(); ++j)
          for (typename Derived::InnerIterator i(derived(),j); i; ++i)
            dst.coeffRef(i.row(),i.col()) = i.value();
      }

      Matrix<Scalar,RowsAtCompileTime,ColsAtCompileTime> toDense() const
      {
        return derived();
      }

    template<typename OtherDerived>
    bool isApprox(const SparseMatrixBase<OtherDerived>& other,
                  RealScalar prec = NumTraits<Scalar>::dummy_precision()) const
    { return toDense().isApprox(other.toDense(),prec); }

    template<typename OtherDerived>
    bool isApprox(const MatrixBase<OtherDerived>& other,
                  RealScalar prec = NumTraits<Scalar>::dummy_precision()) const
    { return toDense().isApprox(other,prec); }

    /** \returns the matrix or vector obtained by evaluating this expression.
      *
      * Notice that in the case of a plain matrix or vector (not an expression) this function just returns
      * a const reference, in order to avoid a useless copy.
      */
    inline const typename internal::eval<Derived>::type eval() const
    { return typename internal::eval<Derived>::type(derived()); }

    Scalar sum() const;

  protected:

    bool m_isRValue;
};

#endif // EIGEN_SPARSEMATRIXBASE_H
