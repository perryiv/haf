
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A smart pointer.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_POINTERS_SMART_POINTER_H_
#define _USUL_POINTERS_SMART_POINTER_H_

#include "Usul/Config/Config.h"

// We need to undefine "check" when compiling against Carbon on OS X.
#ifdef __OBJC__
#undef check
#endif

#ifdef __CARBON__
#undef check
#endif


namespace Usul {
namespace Pointers {


template
<
  class T,
  class ConfigType
>
struct SmartPointer
{
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Useful typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef ConfigType Config;
  typedef typename Config::AccessPolicy NullAccess;
  typedef typename Config::AssignmentPolicy AssignmentPolicy;
  typedef typename Config::ConstructorPolicy ConstructorPolicy;
  typedef typename Config::ReferencePolicy ReferencePolicy;
  typedef typename Config::DestructorPolicy DestructorPolicy;
  typedef T ValueType;
  typedef ValueType element_type;
  typedef SmartPointer < T, Config > ThisType;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Default constructor. Needed for containers.
  //
  /////////////////////////////////////////////////////////////////////////////

  SmartPointer() : _p ( 0x0 )
  {
    ConstructorPolicy::check ( _p );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor from raw pointer.
  //
  /////////////////////////////////////////////////////////////////////////////

  SmartPointer ( T *t ) : _p ( t )
  {
    ConstructorPolicy::check ( _p );
    if ( _p )
    {
      ReferencePolicy::ref ( _p );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor from different smart-pointer type.
  //
  //  Had to pass a copy to get it to compile. Passing a copy is, in general, 
  //  a good thing, so keeping it this way.
  //
  //  Typical use is:
  //
  //    MyClass::RefPtr a ( new MyClass );
  //    MyClass::ValidRefPtr b ( a );
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class P, class C > SmartPointer ( SmartPointer < P, C > p ) : _p ( p.get() )
  {
    ConstructorPolicy::check ( _p );
    if ( _p )
    {
      ReferencePolicy::ref ( _p );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Copy constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  SmartPointer ( const ThisType &p ) : _p ( p._p )
  {
    ConstructorPolicy::check ( _p );
    if ( _p )
    {
      ReferencePolicy::ref ( _p );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Destructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  ~SmartPointer()
  {
    DestructorPolicy::check ( _p );
    if ( _p )
    {
      ReferencePolicy::unref ( _p );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get internal pointer.
  //
  /////////////////////////////////////////////////////////////////////////////

  const T *get() const
  {
    NullAccess::check ( _p );
    return _p;
  }
  T *get()
  {
    NullAccess::check ( _p );
    return _p;
  }

  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Dereference.
  //
  /////////////////////////////////////////////////////////////////////////////

  const T& operator * () const
  {
    return *(this->get());
  }
  T& operator * ()
  {
    return *(this->get());
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  For pointer syntax.
  //
  /////////////////////////////////////////////////////////////////////////////

  const T* operator -> () const
  {
    return this->get();
  }
  T* operator -> ()
  {
    return this->get();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Is the internal pointer valid?
  //
  /////////////////////////////////////////////////////////////////////////////

  bool valid() const
  {
    return 0x0 != _p;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment. Note: no assignment to raw pointers.
  //
  /////////////////////////////////////////////////////////////////////////////

  ThisType &operator = ( const ThisType &p )
  {
    this->_set ( p._p );
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Equality.
  //
  /////////////////////////////////////////////////////////////////////////////

  bool operator == ( const ThisType &p ) const
  {
    return _p == p.get();
  }
  bool operator == ( const T *p ) const
  {
    return _p == p;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Inequality.
  //
  /////////////////////////////////////////////////////////////////////////////

  bool operator != ( const ThisType &p ) const
  {
    return _p != p.get();
  }
  bool operator != ( const T *p ) const
  {
    return _p != p;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  For sorting in containers.
  //
  /////////////////////////////////////////////////////////////////////////////

  bool operator < ( const ThisType &p ) const
  {
    return _p < p.get();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Release control of the internal pointer and return it.
  //
  /////////////////////////////////////////////////////////////////////////////

  T* release()
  {
    // Save a pointer.
    T* saved = _p;

    // If we have a valid pointer...
    if ( _p )
    {
      // Unreference it but make sure it is not deleted.
      ReferencePolicy::unref ( _p, false );

      // Make our internal pointer null;
      _p = 0x0;
    }

    // Return the saved pointer.
    return saved;
  }

protected:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the internal pointer.
  //
  /////////////////////////////////////////////////////////////////////////////

  void _set ( T *p )
  {
    // Check the policy.
    AssignmentPolicy::check ( p );

    // If the given pointer is the same address then just return.
    if ( _p == p )
    {
      return;
    }

    // Make a copy of the current one.
    T *old ( _p );

    // Copy the given pointer.
    _p = p;

    // If the given pointer is not null then reference it.
    if ( _p )
    {
      ReferencePolicy::ref ( _p );
    }

    // If the old one is not null then release it. Make sure we do this last 
    // because the given pointer "p" could be a child (indirect or direct) of 
    // "old". If we release "old" before we reference "p" then "p" may get 
    // deleted when it should not have.
    if ( old )
    {
      ReferencePolicy::unref ( old );
    }
  }


private:

  // Assignment to raw pointers is not permitted.
  ThisType &operator = ( T *p );

  // Do not permit "if ( !ptr )"
  bool operator ! ();

  // No automatic casting.
  operator T *();
  operator const T *() const;

  T *_p;
};


} // namespace Pointers
} // namespace Usul


///////////////////////////////////////////////////////////////////////////////
//
//  For compatibility with boost::bind.
//
///////////////////////////////////////////////////////////////////////////////

namespace boost 
{
  template < class T, class C > const T *get_pointer ( const Usul::Pointers::SmartPointer < T, C > &p )
  {
    return p.get();
  }
  template < class T, class C > T *get_pointer ( Usul::Pointers::SmartPointer < T, C > &p )
  {
    return p.get();
  }
}


#endif // _USUL_POINTERS_SMART_POINTER_H_
