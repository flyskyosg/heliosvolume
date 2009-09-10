
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
//  The TransferFunctionComponent class.
//
///////////////////////////////////////////////////////////////////////////////

#include "TransferFunctionComponent.h"
#include "TransferFunctionWidget.h"

#include "OsgVolume/ITransferFunction1DList.h"

#include "Usul/Components/Factory.h"
#include "Usul/Interfaces/Qt/IMainWindow.h"
#include "Usul/Interfaces/IQtDockWidgetMenu.h"

#include "Usul/Documents/Manager.h"

#include "QtGui/QDockWidget"
#include "QtGui/QMainWindow"

USUL_DECLARE_COMPONENT_FACTORY ( TransferFunctionComponent );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( TransferFunctionComponent, TransferFunctionComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TransferFunctionComponent::TransferFunctionComponent() : BaseClass(),
  _caller(),
  _dock ( 0x0 ),
  _transferFunctionWidget ( 0x0 )
{
  // We want to be notified when the active view changes.
  Usul::Documents::Manager::instance().addActiveDocumentListener ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TransferFunctionComponent::~TransferFunctionComponent()
{
  // Remove this from the list of active view listeners.
  Usul::Documents::Manager::instance().removeActiveDocumentListener ( this );

  if ( 0x0 != _transferFunctionWidget )
  {
    // Clear the transfer function.
    _transferFunctionWidget->setTransferFunction ( 0x0 );
  }

  if ( 0x0 != _dock )
  {
    Usul::Interfaces::Qt::IMainWindow::QueryPtr mainWindow ( _caller );

    if( mainWindow.valid() )
    {
      // Remove the DockWidget from the MainWindow.
      QMainWindow * main ( mainWindow->mainWindow() );
      main->removeDockWidget ( _dock );
    }

    _dock->setWidget ( 0x0 );

    // Delete the DockWidget.
    delete _dock;   
  }

  //// Qt deletes the clip planes.
  _transferFunctionWidget = 0x0;
  _dock = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *TransferFunctionComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPluginInitialize::IID:
    return static_cast< Usul::Interfaces::IPluginInitialize*>(this);
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin*>(this);
  case Usul::Interfaces::IActiveDocumentListener::IID:
    return static_cast < Usul::Interfaces::IActiveDocumentListener* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the tab for the scene tree.
//
///////////////////////////////////////////////////////////////////////////////

void TransferFunctionComponent::initializePlugin ( Usul::Interfaces::IUnknown *caller )
{
  _caller = caller;

  Usul::Interfaces::Qt::IMainWindow::QueryPtr mainWindow ( caller );

  if ( mainWindow.valid() )
  {
    QMainWindow * main ( mainWindow->mainWindow() );

    // Build the docking window.
    _dock = new QDockWidget ( QObject::tr ( "Transfer Function" ), main );
    _dock->setAllowedAreas ( Qt::AllDockWidgetAreas );

    // Create the widget
    _transferFunctionWidget = new TransferFunctionWidget ( _dock );
    _transferFunctionWidget->setEnabled ( false );

    // Add the dock to the main window.
    _dock->setWidget( _transferFunctionWidget );
    main->addDockWidget ( Qt::LeftDockWidgetArea, _dock );

    // Set the object name.
    _dock->setObjectName ( "TransferFunctionWidget" );

    // Add toggle to the menu.
    Usul::Interfaces::IQtDockWidgetMenu::QueryPtr dwm ( caller );
    if ( dwm.valid () )
      dwm->addDockWidgetMenu ( _dock );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  The active document has changed.
//
///////////////////////////////////////////////////////////////////////////////

void TransferFunctionComponent::activeDocumentChanged ( Usul::Interfaces::IUnknown *oldDoc, Usul::Interfaces::IUnknown *newDoc )
{
  if ( 0x0 == _transferFunctionWidget )
    return;

  OsgVolume::ITransferFunction1DList::QueryPtr tfList ( newDoc );
  if ( tfList.valid() )
  {
    _transferFunctionWidget->setEnabled ( true );
    _transferFunctionWidget->setTransferFunction ( tfList->getTransferFunction1D ( tfList->getActiveTransferFunction() ) );
  }
  else
  {
    _transferFunctionWidget->setEnabled ( false );
  }
}
