
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Some handy smart-pointer definitions and macros.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_HANDY_SMART_POINTER_DEFINITIONS_H_
#define _USUL_HANDY_SMART_POINTER_DEFINITIONS_H_

#include "Usul/Config/Config.h"
#include "Usul/Strings/Format.h"
#include "Usul/Pointers/DynamicCastPointer.h"
#include "Usul/Pointers/QueryPointer.h"

#include <stdexcept>
#include <typeinfo>


namespace Usul {
namespace Pointers {
namespace Configs {


///////////////////////////////////////////////////////////////////////////////
//
//  Class that does nothing.
//
///////////////////////////////////////////////////////////////////////////////

struct DoNothing
{
  template < class T > static void check ( const T *t )
  {
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Class that throws if given a null pointer.
//
///////////////////////////////////////////////////////////////////////////////

struct NullThrows
{
  template < class T > static void check ( const T *t )
  {
    if ( 0x0 == t )
    {
      const std::type_info &info ( typeid ( T ) );
      const std::string name ( ( 0x0 == info.name() ) ? "" : info.name() );
      throw std::runtime_error ( Usul::Strings::format 
        ( "Error 3251620222: Accessing null pointer ", t, " of type '", name, "'" ) );
    }
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Policy class with reference counting.
//
///////////////////////////////////////////////////////////////////////////////

struct RefCountingPolicy
{
  template < class T > static void ref ( T *ptr )
  {
    if ( ptr )
    {
      ptr->ref();
    }
  }
  template < class T > static void unref ( T *ptr, bool allowDelete = true )
  {
    if ( ptr )
    {
      ptr->unref ( allowDelete );
    }
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Policy class that always unreferences without deleting.
//
///////////////////////////////////////////////////////////////////////////////

struct NoDeleteRefCountingPolicy : public RefCountingPolicy
{
  typedef RefCountingPolicy BaseClass;

  // Need default argument to compile.
  template < class T > static void unref ( T *ptr, bool allowDelete = false )
  {
    // Pass false rather than given value.
    BaseClass::unref ( ptr, false );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Smart-pointer configuration:
//  - Reference counting.
//  - Null is ok.
//
///////////////////////////////////////////////////////////////////////////////

struct RefCountingNullOk
{
  typedef Usul::Pointers::Configs::RefCountingPolicy ReferencePolicy;
  typedef Usul::Pointers::Configs::DoNothing AccessPolicy;
  typedef Usul::Pointers::Configs::DoNothing AssignmentPolicy;
  typedef Usul::Pointers::Configs::DoNothing ConstructorPolicy;
  typedef Usul::Pointers::Configs::DoNothing DestructorPolicy;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Smart-pointer configuration:
//  - Reference counting.
//  - Null is ok.
//  - No deleting when unreferencing.
//
///////////////////////////////////////////////////////////////////////////////

struct NoDeleteRefCountingNullOk : public RefCountingNullOk
{
  typedef Usul::Pointers::Configs::NoDeleteRefCountingPolicy ReferencePolicy;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Smart-pointer configuration:
//  - Reference counting.
//  - Null throws an exception.
//
///////////////////////////////////////////////////////////////////////////////

struct RefCountingNullThrows
{
  typedef Usul::Pointers::Configs::RefCountingPolicy ReferencePolicy;
  typedef Usul::Pointers::Configs::NullThrows AccessPolicy;
  typedef Usul::Pointers::Configs::NullThrows AssignmentPolicy;
  typedef Usul::Pointers::Configs::NullThrows ConstructorPolicy;
  typedef Usul::Pointers::Configs::DoNothing DestructorPolicy;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Smart-pointer configuration:
//  - Reference counting.
//  - Null access throws an exception.
//
///////////////////////////////////////////////////////////////////////////////

struct RefCountingNullAccessThrows
{
  typedef Usul::Pointers::Configs::RefCountingPolicy ReferencePolicy;
  typedef Usul::Pointers::Configs::NullThrows AccessPolicy;
  typedef Usul::Pointers::Configs::DoNothing AssignmentPolicy;
  typedef Usul::Pointers::Configs::DoNothing ConstructorPolicy;
  typedef Usul::Pointers::Configs::DoNothing DestructorPolicy;
};


} // namespace Configs
} // namespace Pointers
} // namespace Usul


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for declaring ref-counting smart-pointer members.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_DECLARE_REF_POINTERS(class_name) \
  typedef Usul::Pointers::SmartPointer < class_name, Usul::Pointers::Configs::RefCountingNullOk > RefPtr; \
  typedef Usul::Pointers::SmartPointer < class_name, Usul::Pointers::Configs::RefCountingNullThrows > ValidRefPtr; \
  typedef Usul::Pointers::SmartPointer < class_name, Usul::Pointers::Configs::RefCountingNullAccessThrows > ValidAccessRefPtr; \
  typedef Usul::Pointers::SmartPointer < class_name, Usul::Pointers::Configs::NoDeleteRefCountingNullOk > NoDeleteRefPtr

#define USUL_DECLARE_QUERY_POINTERS(class_name) \
  USUL_DECLARE_REF_POINTERS ( class_name ); \
  typedef Usul::Pointers::QueryPointer < class_name, Usul::Pointers::Configs::RefCountingNullOk > QueryPtr; \
  typedef Usul::Pointers::QueryPointer < class_name, Usul::Pointers::Configs::RefCountingNullThrows > ValidQueryPtr; \
  typedef Usul::Pointers::QueryPointer < class_name, Usul::Pointers::Configs::RefCountingNullAccessThrows > ValidAccessQueryPtr; \
  typedef Usul::Pointers::QueryPointer < class_name, Usul::Pointers::Configs::NoDeleteRefCountingNullOk > NoDeleteQueryPtr

#define USUL_DECLARE_CAST_POINTERS(class_name) \
  typedef Usul::Pointers::DynamicCastPointer < class_name, Usul::Pointers::Configs::RefCountingNullOk > CastPtr; \
  typedef Usul::Pointers::DynamicCastPointer < class_name, Usul::Pointers::Configs::RefCountingNullThrows > ValidCastPtr; \
  typedef Usul::Pointers::DynamicCastPointer < class_name, Usul::Pointers::Configs::RefCountingNullAccessThrows > ValidAccessCastPtr; \
  typedef Usul::Pointers::DynamicCastPointer < class_name, Usul::Pointers::Configs::NoDeleteRefCountingNullOk > NoDeleteCastPtr


#endif // _USUL_HANDY_SMART_POINTER_DEFINITIONS_H_
