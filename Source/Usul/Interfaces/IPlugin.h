
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for plugin
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_PLUGIN_H_
#define _USUL_INTERFACES_PLUGIN_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>


namespace Usul {
namespace Interfaces {


struct IPlugin : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IPlugin );

  // Id for this interface.
  enum { IID = 2497111645u };

  // Get a description string.
  virtual std::string               pluginDescription() const = 0;

  // Get the name of the plugin.
  virtual std::string               pluginName() const = 0;

  // Tell the plugin to initialize.
  virtual void                      pluginInitialize() = 0;

  // Tell the plugin to get ready to be released.
  virtual void                      pluginFinalize() = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACES_PLUGIN_H_
