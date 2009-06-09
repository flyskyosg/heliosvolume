
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TRANSFER_FUNTION_COMPONENT_CLASS_H_
#define _TRANSFER_FUNTION_COMPONENT_CLASS_H_

#include "OsgTools/Configure/OSG.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/IPluginInitialize.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IActiveDocumentListener.h"

class QDockWidget;
class TransferFunctionWidget;

class TransferFunctionComponent : public Usul::Base::Referenced,
                           public Usul::Interfaces::IPluginInitialize,
                           public Usul::Interfaces::IPlugin,
                           public Usul::Interfaces::IActiveDocumentListener
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( TransferFunctionComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  TransferFunctionComponent();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IPlugin
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual std::string getPluginName() const { return "Transfer Function Qt"; }

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IPluginInitialize
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void initializePlugin ( Usul::Interfaces::IUnknown *caller = 0x0 );

protected:

  /// The active document has changed.
  virtual void                          activeDocumentChanged ( Usul::Interfaces::IUnknown *oldView, Usul::Interfaces::IUnknown *newView );

  // Do not copy.
  TransferFunctionComponent ( const TransferFunctionComponent & );
  TransferFunctionComponent &operator = ( const TransferFunctionComponent & );

  /// Use reference counting.
  virtual ~TransferFunctionComponent();

  Usul::Interfaces::IUnknown::QueryPtr _caller;

  QDockWidget *_dock;
  TransferFunctionWidget *_transferFunctionWidget;
};


#endif // _TRANSFER_FUNTION_COMPONENT_CLASS_H_
