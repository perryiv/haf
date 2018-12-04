
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the file-document.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_FILE_DOCUMENT_H_
#define _USUL_INTERFACES_FILE_DOCUMENT_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul { namespace Documents { class FileDocument; } }


namespace Usul {
namespace Interfaces {


struct IFileDocument : public Usul::Interfaces::IUnknown
{
  // Typedefs.
  typedef Usul::Documents::FileDocument FileDocument;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IFileDocument );

  // Id for this interface.
  enum { IID = 1709903123u };

  // Return the document.
  virtual const FileDocument *  fileDocument() const = 0;
  virtual FileDocument *        fileDocument() = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACES_FILE_DOCUMENT_H_
