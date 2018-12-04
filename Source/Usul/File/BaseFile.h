
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base file class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FILE_BASE_FILE_CLASS_H_
#define _USUL_FILE_BASE_FILE_CLASS_H_

#include "Usul/Base/Object.h"
#include "Usul/Types/Types.h"

#include <string>


namespace Usul {
namespace File {


class USUL_EXPORT BaseFile : public Usul::Base::Object
{
public:

  // Useful typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Types::UInt64 SizeType;

  // Smart-pointer definitions.
  USUL_REFERENCED_CLASS ( BaseFile );

  // Constructor.
  BaseFile ( const std::string &name );

  // Get the size of the file.
  SizeType                    size() const;
  static SizeType             size ( const std::string &file );

protected:

  // Use reference counting.
  virtual ~BaseFile();

private:

  // No copying or assignment.
  BaseFile ( const BaseFile & );
  BaseFile &operator = ( const BaseFile & );
};


} // namespace File
} // namespace Usul


#endif // _USUL_FILE_BASE_FILE_CLASS_H_
