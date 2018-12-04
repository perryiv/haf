
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Compose many function calls into a single functor call.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ADAPTERS_COMPOSE_H_
#define _USUL_ADAPTERS_COMPOSE_H_


namespace Usul {
namespace Adapters {


///////////////////////////////////////////////////////////////////////////////
//
//  A pair of functors.
//
///////////////////////////////////////////////////////////////////////////////

template < class F1, class F2 > struct FunctorPair
{
  FunctorPair ( F1 f1, F2 f2 ) : _f1 ( f1 ), _f2 ( f2 ){}
  FunctorPair ( const FunctorPair &f ) : _f1 ( f._f1 ), _f2 ( f._f2 ){}
  void operator () ()
  {
    _f1();
    _f2();
  }
  template < class Arg > void operator () ( Arg &a )
  {
    _f1 ( a );
    _f2 ( a );
  }
private:
  FunctorPair &operator = ( const FunctorPair & );
  F1 _f1;
  F2 _f2;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Compose functors.
//
///////////////////////////////////////////////////////////////////////////////

template < class F1, class F2 > 
inline FunctorPair < F1, F2 > 
compose ( F1 f1, F2 f2 )
{
  typedef FunctorPair < F1, F2 > Fun12;
  return Fun12 ( f1, f2 );
}
template < class F1, class F2, class F3 > 
inline FunctorPair < FunctorPair < F1, F2 >, F3 >
compose ( F1 f1, F2 f2, F3 f3 )
{
  typedef FunctorPair < F1, F2 > Fun12;
  typedef FunctorPair < Fun2, F3 > Fun3;
  return Fun3 ( Fun12 ( f1, f2 ), f3 );
}
template < class F1, class F2, class F3, class F4 > 
inline FunctorPair < FunctorPair < F1, F2 >, FunctorPair < F3, F4 > >
compose ( F1 f1, F2 f2, F3 f3, F4 f4 )
{
  typedef FunctorPair < F1, F2 > Fun12;
  typedef FunctorPair < F3, F4 > Fun34;
  typedef FunctorPair < Fun12, Fun34 > Fun1234;
  return Fun1234 ( Fun12 ( f1, f2 ), Fun34 ( f3, f4 ) );
}


} // namespace Adapters
} // namespace Usul


#endif // _USUL_ADAPTERS_COMPOSE_H_
