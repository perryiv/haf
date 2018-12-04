
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for reading a binary file.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/File/BinaryInputFile.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/LastError.h"

#include <fstream>
#include <limits>
#include <stdexcept>

using namespace Usul::File;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

BinaryInputFile::BinaryInputFile ( const std::string &name ) : BaseClass ( name, true )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

BinaryInputFile::~BinaryInputFile()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the bytes.
//
///////////////////////////////////////////////////////////////////////////////

void BinaryInputFile::_read ( Buffer &buffer, SizeType size )
{
  // Size the given buffer.
  buffer.resize ( static_cast < Buffer::size_type > ( size ), '\0' );

  // Read the bytes.
  {
    Guard guard ( this );
    this->_getFile().read ( &buffer[0], buffer.size() );
  }
}
