
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Predicates for executing if-then-else logic.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ADAPTERS_IF_THEN_ELSE_H_
#define _USUL_ADAPTERS_IF_THEN_ELSE_H_


namespace Usul {
namespace Adapters {


///////////////////////////////////////////////////////////////////////////////
//
//  If-then
//
///////////////////////////////////////////////////////////////////////////////

template < class F1, class F2 > struct IfThen
{
  IfThen ( F1 f1, F2 f2 ) : _f1 ( f1 ), _f2 ( f2 ){}
  IfThen ( const IfThen &f ) : _f1 ( f._f1 ), _f2 ( f._f2 ){}
  void operator () ()
  {
    if ( _f1() )
    {
      _f2();
    }
  }
private:
  IfThen &operator = ( const IfThen & );
  F1 _f1;
  F2 _f2;
};


///////////////////////////////////////////////////////////////////////////////
//
//  If-then-else
//
///////////////////////////////////////////////////////////////////////////////

template < class F1, class F2, class F3 > struct IfThenElse
{
  IfThenElse ( F1 f1, F2 f2, F3 f3 ) : _f1 ( f1 ), _f2 ( f2 ), _f3 ( f3 ){}
  IfThenElse ( const IfThenElse &f ) : _f1 ( f._f1 ), _f2 ( f._f2 ), _f3 ( f._f3 ){}
  void operator () ()
  {
    if ( _f1() )
    {
      _f2();
    }
    else
    {
      _f3();
    }
  }
private:
  IfThenElse &operator = ( const IfThenElse & );
  F1 _f1;
  F2 _f2;
  F3 _f3;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions.
//
///////////////////////////////////////////////////////////////////////////////

template < class F1, class F2 > inline IfThen < F1, F2 > ifThen ( F1 f1, F2 f2 )
{
  return IfThen < F1, F2 > ( f1, f2 );
}
template < class F1, class F2, class F3 > inline IfThenElse < F1, F2, F3 > ifThenElse ( F1 f1, F2 f2, F3 f3 )
{
  return IfThenElse < F1, F2, F3 > ( f1, f2, f3 );
}


} // namespace Adapters
} // namespace Usul


#endif // _USUL_ADAPTERS_IF_THEN_ELSE_H_
