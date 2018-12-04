
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A map of job-queues.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Config/Config.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Functions/NoThrow.h"

using namespace Usul::Jobs;


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the singleton at startup to avoid problems with construction 
//  in multi-threaded environments.
//
///////////////////////////////////////////////////////////////////////////////

namespace
{
  struct Init
  {
    Init()
    {
      Usul::Jobs::Manager::instance();
    }
    ~Init()
    {
      Usul::Jobs::Manager::destroy();
    }
  } _init;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Singleton instance.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{
  namespace Jobs
  {
    namespace Detail
    {
      Usul::Jobs::Manager *_instance ( 0x0 );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the instance.
//
///////////////////////////////////////////////////////////////////////////////

Manager& Manager::instance()
{
  if ( 0x0 == Usul::Jobs::Detail::_instance )
  {
    Usul::Jobs::Detail::_instance = new Manager();
  }
  return *Usul::Jobs::Detail::_instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy the single instance. Not thread safe.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::destroy()
{
  Manager *m ( Usul::Jobs::Detail::_instance );
  Usul::Jobs::Detail::_instance = 0x0;
  delete m;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Manager::Manager() : _queues()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Manager::~Manager()
{
  USUL_TRY_BLOCK
  {
    // Clear the queues.
    _queues.clear();
  }
  USUL_DEFINE_CATCH_BLOCKS ( "2784919173" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the queue.
//
///////////////////////////////////////////////////////////////////////////////

Manager::QueuePtr &Manager::operator [] ( const std::string &name )
{
  Queues::Guard guard ( _queues.mutex() );
  Queues::ValueType &queues ( _queues.getReference() );
  QueuePtr &queue ( queues[name] );
  return queue;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the queue.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::remove ( const std::string &name )
{
  Queues::Guard guard ( _queues.mutex() );
  Queues::ValueType &queues ( _queues.getReference() );
  queues.erase ( name );
}