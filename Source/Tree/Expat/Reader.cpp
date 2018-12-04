
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  XML reader class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Tree/Expat/Reader.h"

#include "Usul/Errors/Assert.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/LastError.h"
#include "Usul/Types/Types.h"

#include "boost/algorithm/string/replace.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/bind.hpp"
#include "boost/filesystem/operations.hpp"

#include "expat.h"

#include <fstream>
#include <stdexcept>
#include <vector>

using namespace Tree::Expat;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Reader::Reader() : 
  _threadCheck(),
  _nodeStack(),
  _stop ( true ),
  _root()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Reader::~Reader()
{
  USUL_TRY_BLOCK
  {
    this->_nodeStackClear();
    _root = Node::RefPtr();
  }
  USUL_DEFINE_CATCH_BLOCKS ( "2892959270" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when an element is started.
//
///////////////////////////////////////////////////////////////////////////////

void Reader::_elementStartCB ( void *userData, const char *name, const char **atts )
{
  Reader *reader ( reinterpret_cast < Reader * > ( userData ) );
  if ( 0x0 != reader )
  {
    Usul::Functions::noThrow
      ( boost::bind ( &Reader::_elementStart, reader, name, atts ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when an element is ended.
//
///////////////////////////////////////////////////////////////////////////////

void Reader::_elementEndCB ( void *userData, const char *name )
{
  Reader *reader ( reinterpret_cast < Reader * > ( userData ) );
  if ( 0x0 != reader )
  {
    reader->_elementEnd ( name );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when an element is started.
//
///////////////////////////////////////////////////////////////////////////////

void Reader::_elementStart ( const char *n, const char **a )
{
  _threadCheck.throwIfDifferentThread();

  // Handle bad input.
  if ( 0x0 == n )
  {
    _stop = true;
    return;
  }

  // Get the name.
  const std::string name ( n );
  if ( true == name.empty() )
  {
    _stop = true;
    return;
  }

  // Get the current node. It will be null if this is the first one.
  Node::RefPtr current ( this->_nodeStackTop() );

  // Make a new node and push it onto the stack.
  Node::RefPtr node ( new Node ( name ) );
  this->_nodeStackPush ( node );

  // Append it to the current node if it's valid.
  if ( true == current.valid() )
  {
    current->append ( node );
  }

  // Otherwise, this is the first node.
  else
  {
    _root = node;
  }

  // Add the attributes.
  Node::Attributes attributes;
  if ( 0x0 != a )
  {
    while ( 0x0 != a[0] )
    {
      // Get the attribute.
      const std::string attrName ( a[0] );
      ++a;
      if ( false == attrName.empty() )
      {
        // Increment and get the value.
        if ( 0x0 != a[0] )
        {
          const std::string attrValue ( a[0] );
          ++a;
          if ( false == attrValue.empty() )
          {
            attributes[attrName] = attrValue;
          }
        }
      }
    }
  }

  // Set the node's attributes.
  node->attributes ( attributes );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when an element is ended.
//
///////////////////////////////////////////////////////////////////////////////

void Reader::_elementEnd ( const char * )
{
  _threadCheck.throwIfDifferentThread();

  // Get the current node.
  Node::RefPtr node ( this->_nodeStackTop() );
  if ( false == node.valid() )
  {
    _stop = true;
    return;
  }

  // Trim leading and trailing white-space from the value.
  // This happens because the value handler _elementValueCB() 
  // gets called for every character in the file.
  node->value ( boost::algorithm::trim_copy ( node->value() ) );

  // Done with this node.
  this->_nodeStackPop();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when an element's value is read. This function may be called many 
//  time in sequence for the same value (if it is really long).
//
///////////////////////////////////////////////////////////////////////////////

void Reader::_elementValueCB ( void *userData, const char *value, int length )
{
  Reader *reader ( reinterpret_cast < Reader * > ( userData ) );
  if ( 0x0 != reader )
  {
    Usul::Functions::noThrow
      ( boost::bind ( &Reader::_elementValue, reader, value, length ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when an element's value is read. This function may be called many 
//  time in sequence for the same value (if it is really long).
//
///////////////////////////////////////////////////////////////////////////////

void Reader::_elementValue ( const char *v, int length )
{
  _threadCheck.throwIfDifferentThread();

  // Handle bad input.
  if ( 0x0 == v )
  {
    return;
  }

  // Get the value. Not null-terminated!
  // See http://www.xml.com/lpt/a/47#chardatahandler
  const std::string value ( v, v + length );
  if ( true == value.empty() )
  {
    return;
  }

  // Get the current node.
  Node::RefPtr node ( this->_nodeStackTop() );
  if ( false == node.valid() )
  {
    _stop = true;
    return;
  }

  // We append to handle repeated calls for the pieces of the same value.
  node->value ( node->value() + value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the stack.
//
///////////////////////////////////////////////////////////////////////////////

void Reader::_nodeStackClear()
{
  _threadCheck.throwIfDifferentThread();
  while ( false == _nodeStack.empty() )
  {
    _nodeStack.pop();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return stack size.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Reader::_nodeStackSize() const
{
  _threadCheck.throwIfDifferentThread();
  return _nodeStack.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return top of the stack.
//
///////////////////////////////////////////////////////////////////////////////

Tree::Node::RefPtr Reader::_nodeStackTop() const
{
  _threadCheck.throwIfDifferentThread();
  if ( false == _nodeStack.empty() )
  {
    return _nodeStack.top();
  }
  else
  {
    return Node::RefPtr();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Push the node onto the stack.
//
///////////////////////////////////////////////////////////////////////////////

void Reader::_nodeStackPush ( Node::RefPtr node )
{
  _threadCheck.throwIfDifferentThread();
  if ( true == node.valid() )
  {
    _nodeStack.push ( node );
  }
  else
  {
    USUL_ASSERT ( false );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pop the top node off the stack.
//
///////////////////////////////////////////////////////////////////////////////

void Reader::_nodeStackPop()
{
  _threadCheck.throwIfDifferentThread();
  if ( false == _nodeStack.empty() )
  {
    _nodeStack.pop();
  }
  else
  {
    USUL_ASSERT ( false );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load contents of file and build the document. 
//
///////////////////////////////////////////////////////////////////////////////

Tree::Node::RefPtr Reader::read ( const std::string &file,
                                  IUnknown::RefPtr caller,
                                  unsigned int bufferSize )
{
  _threadCheck.throwIfDifferentThread();

  // A file that does not exist is an error.
  if ( false == boost::filesystem::exists ( file ) )
  {
    throw std::runtime_error ( Usul::Strings::format 
      ( "Error 3837597118: Given file does not exist: ", file ) );
  }

  // Empty files are ok.
  const boost::uintmax_t size ( boost::filesystem::file_size ( file ) );
  if ( 0 == size )
  {
    return Node::RefPtr();
  }

  // Open the file for reading.
  std::ifstream in ( file.c_str(), std::ifstream::in | std::ifstream::binary );
  if ( false == in.is_open() )
  {
    throw std::runtime_error ( Usul::Strings::format 
      ( "Error 2816436608: Cannot open '", file, "' for reading" ) );
  }

  // Call the other one.
  return this->read ( in, size, caller, bufferSize );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load contents of file and build the document. 
//
///////////////////////////////////////////////////////////////////////////////

Tree::Node::RefPtr Reader::read ( 
  std::istream &in, Usul::Types::UInt64 remaining, 
  IUnknown::RefPtr caller, unsigned int bufferSize )
{
  _threadCheck.throwIfDifferentThread();

  // Initialize members.
  _stop = false;
  this->_nodeStackClear();
  _root = Node::RefPtr();

  // Empty streams are ok.
  if ( 0 == remaining )
  {
    return Node::RefPtr();
  }

  // Make the parser.
  ::XML_Parser parser ( ::XML_ParserCreate ( 0x0 ) );
  if ( 0x0 == parser )
  {
    throw std::runtime_error ( "Error 1441149759: Failed to create XML parser" );
  }

  // Always delete the parser.
  Usul::Scope::Caller::RefPtr deleteParser ( Usul::Scope::makeCaller (
    boost::bind ( ::XML_ParserFree, boost::ref ( parser ) ) ) );

  // Set the element handler.
  ::XML_SetElementHandler ( parser, 
    &Reader::_elementStartCB, &Reader::_elementEndCB );

  // Set the value handler.
  ::XML_SetCharacterDataHandler ( parser, &Reader::_elementValueCB );

  // Set the user data.
  ::XML_SetUserData ( parser, this );

  // The buffers used below.
  typedef std::vector<char> Buffer;
  Buffer buffer ( Usul::Math::maximum<unsigned int> ( bufferSize, 256 ), '\0' );
  const Buffer::size_type last ( buffer.size() - 1 );
  std::string chunk;

  // Loop through the file,
  while ( EOF != in.peek() )
  {
    // See if we have been stopped.
    if ( true == _stop )
    {
      const int line ( ::XML_GetCurrentLineNumber ( parser ) );
      throw std::runtime_error ( Usul::Strings::format ( 
        "Error 2340382839: Parser stopped near line ", line ) );
    }

    // Determine how much to read. It will be the minimum of the 
    // buffer's size and the remaining bytes in the stream.
    // Note: using tellg() is unreliable.
    const std::streamsize readThisMuch ( 
      Usul::Math::minimum<std::streamsize> ( 
        static_cast<std::streamsize> ( last - 1 ), 
        static_cast<std::streamsize> ( remaining ) ) );

    // Has to be positive.
    if ( readThisMuch <= 0 )
    {
      throw Usul::Exceptions::Error ( "1415918987", 
        "Internal error calculating remaining bytes in stream" );
    }

    // Initialize the buffer.
    std::fill ( buffer.begin(), buffer.end(), '\0' );

    // Try to fill the buffer.
    in.read ( &buffer[0], readThisMuch );

    // Copy to a string.
    chunk = ( ( '\0' == buffer[0] ) ? "" : &buffer[0] );

    // Since we may be reading in binary, fix the line endings, in this order.
    boost::algorithm::replace_all ( chunk, "\r\n", "\n" );
    std::replace ( chunk.begin(), chunk.end(), '\r', '\n' );

    // Handle the end-of-file character.
    std::replace ( chunk.rbegin(), chunk.rend(), static_cast<char> ( EOF ), '\n' );

    // Was this the last chunk?
    const int isFinal ( EOF == in.peek() );

    // Parse the buffer.
    if ( 0 == ::XML_Parse ( parser, chunk.c_str(), chunk.size(), isFinal ) )
    {
      const int line ( ::XML_GetCurrentLineNumber ( parser ) );
      const int column ( ::XML_GetCurrentColumnNumber ( parser ) );
      const char *s ( ::XML_ErrorString ( ::XML_GetErrorCode ( parser ) ) );
      const std::string message ( ( 0x0 == s ) ? "unknown error" : s );

      throw std::runtime_error ( Usul::Strings::format ( 
        "Error 1353693920: XML parse error",
        ", line ", line, ", column ", column, ", ", message ) );
    }

    // Update the remaining amount.
    remaining -= static_cast < Usul::Types::UInt64 > ( readThisMuch );
  }

  // Should be true.
  USUL_ASSERT ( 0 == this->_nodeStackSize() );

  // Return the root if it is valid.
  if ( true == _root.valid() )
  {
    const std::string name ( _root->name() );
    if ( false == name.empty() )
    {
      return _root;
    }
  }

  // If we get to here then it did not work.
  return Node::RefPtr();
}
