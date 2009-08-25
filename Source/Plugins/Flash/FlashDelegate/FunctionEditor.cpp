
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Input dialog for files.
//
///////////////////////////////////////////////////////////////////////////////

#include "FunctionEditor.h"
#include "Flash/FlashModel/IFlashDocument.h"

#include "Usul/Convert/Convert.h"
#include "Usul/Documents/Manager.h"

#include "QtGui/QScrollArea"


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FunctionEditor::FunctionEditor ( QWidget *parent ) : BaseClass ( parent )
{
  // Initialize code from Designer.
  this->setupUi ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FunctionEditor::~FunctionEditor()
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  Change the material color and set the current radio button
//
///////////////////////////////////////////////////////////////////////////////

void FunctionEditor::radioClicked( QAbstractButton * button )
{
  
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the selected radio in integer code from the interface
//
///////////////////////////////////////////////////////////////////////////////

int FunctionEditor::getSelectedRadio()
{

  if( true == logRadio->isChecked() )
  {
    return Flash::IFlashDocument::LOG_FUNCTION;
  }

  if( true == absRadio->isChecked() )
  {
    return Flash::IFlashDocument::ABS_FUNCTION;
  }

  if( true == this->scalarRadio->isChecked() )
  {
    std::string strval ( _scalar->text().toStdString() );
    double value ( Usul::Convert::Type< std::string, double >::convert( strval ) );

      // get the document
    Flash::IFlashDocument::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

    // Check for a valid document
    if( false == document.valid() )
    {
      std::cout << "Error in FlashDelegateComponent::_editFunctions().  Document is invalid!" << std::endl;
      return -1;
    }

    // set the scalar value in the document
    document->scalar( value );

    return Flash::IFlashDocument::SCALAR_MULT_FUNCTION;
  }

  return -1;
}