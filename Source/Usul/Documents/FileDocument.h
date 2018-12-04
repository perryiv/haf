
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV and Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for documents that read and write a file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_DOCUMENTS_FILE_DOCUMENT_CLASS_H_
#define _USUL_DOCUMENTS_FILE_DOCUMENT_CLASS_H_

#include "Usul/Documents/Document.h"
#include "Usul/Atomic/Object.h"
#include "Usul/Interfaces/IDocumentFilters.h"
#include "Usul/Interfaces/IFileDocument.h"
#include "Usul/Interfaces/IRead.h"

#include <string>


namespace Usul {
namespace Documents {


class USUL_EXPORT FileDocument : 
  public Document,
  public Usul::Interfaces::IFileDocument,
  public Usul::Interfaces::IRead
{
public:

  // Typedefs
  typedef Document BaseClass;
  typedef std::pair < std::string, bool > FileInfo;
  typedef Usul::Atomic::Object < FileInfo > FileName;
  typedef Usul::Interfaces::IDocumentFilters::Filters Filters;
  typedef Usul::Interfaces::IDocumentFilters::Filter Filter;
  typedef Usul::Interfaces::IUnknown IUnknown;

  // Smart-pointer definitions.
  USUL_REFERENCED_CLASS ( FileDocument );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Return true if this document can do it.
  virtual bool                    canExport ( const std::string & ) const { return false; }
  virtual bool                    canInsert ( const std::string & ) const { return false; }
  virtual bool                    canOpen   ( const std::string & ) const { return false; }
  virtual bool                    canSave   ( const std::string & ) const { return false; }

  // Destroy the document.
  virtual void                    destroyObject();

  // Usul::Interfaces::IFileDocument
  virtual const FileDocument *    fileDocument() const;
  virtual FileDocument *          fileDocument();

  // Get/set the file name.
  void                            fileName ( FileInfo & );
  FileInfo                        fileName() const;

  // Get the filters that correspond to what this document can do.
  virtual Filters                 filtersExport() const { return Filters(); }
  virtual Filters                 filtersInsert() const { return Filters(); }
  virtual Filters                 filtersOpen()   const { return Filters(); }
  virtual Filters                 filtersSave()   const { return Filters(); }

  // Is the current file name valid?
  bool                            isFileNameValid() const;

  // Read the file and add it to existing content.
  virtual void                    read ( const std::string &file, IUnknown::RefPtr caller = IUnknown::RefPtr ( 0x0 ) );

  // Return a string suitable for a GUI window's title bar.
  virtual std::string             title() const;

  // Write the document to given file name. Does not rename this document.
  virtual void                    write ( const std::string &file, IUnknown::RefPtr caller = IUnknown::RefPtr ( 0x0 ) );

protected:

  // Constructor
  FileDocument ( const std::string &file = std::string() );

  // Use reference counting.
  virtual ~FileDocument();

private:

  void                            _destroy();

  FileName _fileName;
};


} // namespace Documents
} // namespace Usul


#endif // _USUL_DOCUMENTS_FILE_DOCUMENT_CLASS_H_
