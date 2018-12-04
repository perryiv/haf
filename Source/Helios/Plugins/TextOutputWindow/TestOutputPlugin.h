
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The plugin class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TEXT_OUTPUT_WINDOW_COMPONENT_CLASS_H_
#define _TEXT_OUTPUT_WINDOW_COMPONENT_CLASS_H_

#include "Helios/Plugins/TextOutputWindow/CompileGuard.h"

#include "Usul/Atomic/Container.h"
#include "Usul/Atomic/Integer.h"
#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Jobs/Queue.h"
#include "Usul/Threads/Check.h"

#include "boost/noncopyable.hpp"

#include <list>
#include <string>

class QDockWidget;
class QTextEdit;


namespace Helios {
namespace Plugins {
namespace TextOutputWindow {


class TestOutputPlugin : 
  public Usul::Base::Object,
  public boost::noncopyable,
  public Usul::Interfaces::IPlugin
{
public:

  // Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef std::list < std::string > StringList;
  typedef Usul::Jobs::BaseJob Job;
  typedef Usul::Jobs::Queue::JobSet Jobs;

  // Smart-pointer definitions.
  USUL_REFERENCED_CLASS ( TestOutputPlugin );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Default construction.
  TestOutputPlugin();

  // Usul::Interfaces::IPlugin
  virtual std::string             pluginDescription() const;
  virtual std::string             pluginName() const { return "Text Output Window"; }
  virtual void                    pluginInitialize();
  virtual void                    pluginFinalize();

protected: 

  // Use reference counting.
  virtual ~TestOutputPlugin();

  void                            _appendLines ( Job::RefPtr );

  void                            _readOutputFile ( Job::RefPtr );

private:

  void                            _clear();

  void                            _destroy();

  QDockWidget *_dock;
  QTextEdit *_text;
  Usul::Atomic::Integer < unsigned int > _offset;
  Usul::Atomic::Container < StringList > _lines;
  Usul::Atomic::Container < std::string > _file;
  Usul::Atomic::Container < Jobs > _jobs;
  Usul::Threads::Check _threadCheck;
};


} // namespace TextOutputWindow
} // namespace Plugins
} // namespace Helios


#endif // _TEXT_OUTPUT_WINDOW_COMPONENT_CLASS_H_
