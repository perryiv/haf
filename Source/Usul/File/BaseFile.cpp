
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

#include "Usul/File/BaseFile.h"

#include "boost/filesystem.hpp"

using namespace Usul::File;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

BaseFile::BaseFile ( const std::string &name ) : BaseClass ( name )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

BaseFile::~BaseFile()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the size of the file.
//
///////////////////////////////////////////////////////////////////////////////

BaseFile::SizeType BaseFile::size ( const std::string &file )
{
  const SizeType size ( boost::filesystem::file_size ( file ) );
  return size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the size of the file.
//
///////////////////////////////////////////////////////////////////////////////

BaseFile::SizeType BaseFile::size() const
{
  const SizeType size ( BaseFile::size ( this->name() ) );
  return size;
}
