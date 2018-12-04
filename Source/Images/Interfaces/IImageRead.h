
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for reading and returning a new image.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGES_LIBRARY_INTERFACES_READ_H_
#define _IMAGES_LIBRARY_INTERFACES_READ_H_

#include "Usul/Interfaces/IUnknown.h"

#include "Images/Core/BaseImage.h"

#include <string>


namespace Images {
namespace Interfaces {


struct IImageRead : public Usul::Interfaces::IUnknown
{
  USUL_DECLARE_QUERY_POINTERS ( IImageRead );
  typedef Images::Core::BaseImage::RefPtr ImagePtr;
  enum { IID = 1702348953u };

  // Can we read the image file?
  virtual bool          canReadImageFile ( const std::string &file ) = 0;

  // Read the image and return it.
  virtual ImagePtr      readImageFile ( 
                          const std::string &file, 
                          Usul::Interfaces::IUnknown::RefPtr caller ) = 0;
};


} // End namespace Interfaces
} // End namespace Images


#endif // _IMAGES_LIBRARY_INTERFACES_READ_H_
