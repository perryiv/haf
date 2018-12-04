
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Pair of unary predicates.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_PREDICATES_UNARY_PAIR_H_
#define _USUL_PREDICATES_UNARY_PAIR_H_


namespace Usul {
namespace Predicates {


///////////////////////////////////////////////////////////////////////////////
//
//  Pair of unary predicates.
//
///////////////////////////////////////////////////////////////////////////////

template < class Pred1, class Operation, class Pred2 >
struct UnaryPair
{
  UnaryPair ( Pred1 p1, Operation op, Pred2 p2 ) : 
    _p1 ( p1 ), _p2 ( p2 ), _op ( op )
  {
  }
  template < class T > bool operator() ( const T &t ) const
  {
    return _op ( _p1 ( t ), _p2 ( t ) );
  }
private:
  Pred1 _p1;
  Pred2 _p2;
  Operation _op;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to make a pair of unary predicates.
//
///////////////////////////////////////////////////////////////////////////////

template < class Pred1, class Operation, class Pred2 > 
inline UnaryPair < Pred1, Operation, Pred2 > unaryPair ( Pred1 p1, Operation op, Pred2 p2 )
{
  return UnaryPair < Pred1, Operation, Pred2 > ( p1, op, p2 );
}


} // namespace Predicates
} // namespace Usul


#endif // _USUL_PREDICATES_UNARY_PAIR_H_
