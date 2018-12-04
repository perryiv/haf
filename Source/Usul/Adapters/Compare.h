
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Predicates for comparing other predicates.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ADAPTERS_COMPARE_H_
#define _USUL_ADAPTERS_COMPARE_H_


namespace Usul {
namespace Adapters {


///////////////////////////////////////////////////////////////////////////////
//
//  Equal
//
///////////////////////////////////////////////////////////////////////////////

template < class F1, class F2 > struct Equal
{
  Equal ( F1 f1, F2 f2 ) : _f1 ( f1 ), _f2 ( f2 ){}
  Equal ( const Equal &f ) : _f1 ( f._f1 ), _f2 ( f._f2 ){}
  bool operator () ()
  {
    return ( _f1() == _f2() );
  }
private:
  Equal &operator = ( const Equal & );
  F1 _f1;
  F2 _f2;
};


///////////////////////////////////////////////////////////////////////////////
//
//  NotEqual
//
///////////////////////////////////////////////////////////////////////////////

template < class F1, class F2 > struct NotEqual
{
  NotEqual ( F1 f1, F2 f2 ) : _f1 ( f1 ), _f2 ( f2 ){}
  NotEqual ( const NotEqual &f ) : _f1 ( f._f1 ), _f2 ( f._f2 ){}
  bool operator () ()
  {
    return ( _f1() == _f2() );
  }
private:
  NotEqual &operator = ( const NotEqual & );
  F1 _f1;
  F2 _f2;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Less
//
///////////////////////////////////////////////////////////////////////////////

template < class F1, class F2 > struct Less
{
  Less ( F1 f1, F2 f2 ) : _f1 ( f1 ), _f2 ( f2 ){}
  Less ( const Less &f ) : _f1 ( f._f1 ), _f2 ( f._f2 ){}
  bool operator () ()
  {
    return ( _f1() < _f2() );
  }
private:
  Less &operator = ( const Less & );
  F1 _f1;
  F2 _f2;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Greater
//
///////////////////////////////////////////////////////////////////////////////

template < class F1, class F2 > struct Greater
{
  Greater ( F1 f1, F2 f2 ) : _f1 ( f1 ), _f2 ( f2 ){}
  Greater ( const Greater &f ) : _f1 ( f._f1 ), _f2 ( f._f2 ){}
  bool operator () ()
  {
    return ( _f1() > _f2() );
  }
private:
  Greater &operator = ( const Greater & );
  F1 _f1;
  F2 _f2;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Greater than or equal.
//
///////////////////////////////////////////////////////////////////////////////

template < class F1, class F2 > struct GreaterOrEqual
{
  GreaterOrEqual ( F1 f1, F2 f2 ) : _f1 ( f1 ), _f2 ( f2 ){}
  GreaterOrEqual ( const GreaterOrEqual &f ) : _f1 ( f._f1 ), _f2 ( f._f2 ){}
  bool operator () ()
  {
    return ( _f1() >= _f2() );
  }
private:
  GreaterOrEqual &operator = ( const GreaterOrEqual & );
  F1 _f1;
  F2 _f2;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Less than or equal.
//
///////////////////////////////////////////////////////////////////////////////

template < class F1, class F2 > struct LessOrEqual
{
  LessOrEqual ( F1 f1, F2 f2 ) : _f1 ( f1 ), _f2 ( f2 ){}
  LessOrEqual ( const LessOrEqual &f ) : _f1 ( f._f1 ), _f2 ( f._f2 ){}
  bool operator () ()
  {
    return ( _f1() <= _f2() );
  }
private:
  LessOrEqual &operator = ( const LessOrEqual & );
  F1 _f1;
  F2 _f2;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions.
//
///////////////////////////////////////////////////////////////////////////////

template < class F1, class F2 > inline Equal < F1, F2 > equal ( F1 f1, F2 f2 )
{
  return Equal < F1, F2 > ( f1, f2 );
}
template < class F1, class F2 > inline NotEqual < F1, F2 > notEqual ( F1 f1, F2 f2 )
{
  return NotEqual < F1, F2 > ( f1, f2 );
}
template < class F1, class F2 > inline Less < F1, F2 > less ( F1 f1, F2 f2 )
{
  return Less < F1, F2 > ( f1, f2 );
}
template < class F1, class F2 > inline Greater < F1, F2 > greater ( F1 f1, F2 f2 )
{
  return Greater < F1, F2 > ( f1, f2 );
}
template < class F1, class F2 > inline LessOrEqual < F1, F2 > lessOrEqual ( F1 f1, F2 f2 )
{
  return LessOrEqual < F1, F2 > ( f1, f2 );
}
template < class F1, class F2 > inline GreaterOrEqual < F1, F2 > greaterOrEqual ( F1 f1, F2 f2 )
{
  return GreaterOrEqual < F1, F2 > ( f1, f2 );
}


} // namespace Adapters
} // namespace Usul


#endif // _USUL_ADAPTERS_COMPARE_H_
