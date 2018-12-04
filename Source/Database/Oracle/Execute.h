
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for executing oracle sql commands.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DATABASE_ORACLE_EXECUTE_QUERIES_H_
#define _DATABASE_ORACLE_EXECUTE_QUERIES_H_

#include "Database/Oracle/Connection.h"
#include "Database/Oracle/Result.h"

#include "boost/shared_ptr.hpp"

#include <map>
#include <string>
#include <vector>


namespace Database {
namespace Oracle {


class DATABASE_ORACLE_EXPORT Execute
{
public:

  typedef Database::Oracle::Result Result;
  typedef Database::Oracle::Connection Connection;
  typedef std::map<std::string,std::string> Input;
  typedef std::vector<std::string> Commands;
  typedef boost::shared_ptr<std::ofstream> OutputStreamPtr;
  typedef std::map<std::string,OutputStreamPtr> OutputStreams;

  Execute ( int argc, char **argv );
  ~Execute();

  void                    run();

  void                    writeError  ( const std::string &s );
  void                    writeUsage() const;

  struct Usage : public std::invalid_argument
  {
    Usage() : std::invalid_argument ( "No arguments" ){}
  };

protected:

  void                    _checkInput();

  Result::RefPtr          _executeCommand ( Connection::RefPtr, const std::string &sql );
  void                    _executeCommands();
  void                    _executeCommands ( Connection::RefPtr, std::ostream &out );

  unsigned int            _getNumRowsToPrefetch() const;
  std::ostream &          _getStream ( const std::string &name );

  void                    _processCommandLine();

  void                    _writeResult ( Result::RefPtr result, std::ostream &out );

private:

  Input _input;
  Commands _commands;
  OutputStreams _streams;
};


} // namespace Oracle
} // namespace Database


#endif // _DATABASE_ORACLE_EXECUTE_QUERIES_H_
