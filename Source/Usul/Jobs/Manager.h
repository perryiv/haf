
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

#ifndef _USUL_JOBS_QUEUE_MANAGER_H_
#define _USUL_JOBS_QUEUE_MANAGER_H_

#include "Usul/Config/Config.h"
#include "Usul/Export/Export.h"
#include "Usul/Jobs/Queue.h"
#include "Usul/Atomic/Container.h"

#include "boost/noncopyable.hpp"

#include <map>


namespace Usul {
namespace Jobs {


class USUL_EXPORT Manager : public boost::noncopyable
{
public:

  typedef Usul::Jobs::Queue::RefPtr QueuePtr;
  typedef Usul::Atomic::Container < std::map < std::string, QueuePtr > > Queues;

  // Singleton interface.
  static Manager &      instance();
  static void           destroy();

  // Return the queue, which may be null.
  QueuePtr &            operator [] ( const std::string & );

  // Remove the queue.
  void                  remove ( const std::string & );

  // Return number of queues.
  unsigned int          size() const;

private:

  Manager();
  ~Manager();

  Queues _queues;
};


} // namespace Jobs
} // namespace Usul


#endif // _USUL_JOBS_QUEUE_MANAGER_H_
