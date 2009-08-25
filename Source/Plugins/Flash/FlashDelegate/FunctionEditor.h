
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

#ifndef __FLASH_FUNCTION_EDITOR_H__
#define __FLASH_FUNCTION_EDITOR_H__

#include "ui_FunctionEditor.h"

#include "QtGui/QWidget"
#include "QtGui/QRadioButton"
#include "QtGui/QCheckBox"

#include <string>

class FunctionEditor : public QDialog,
                       private Ui::FunctionEditor
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;  

  FunctionEditor ( QWidget *parent = 0x0 );
  virtual ~FunctionEditor();

  int                   getSelectedRadio();

private:

  QButtonGroup*                  _buttons;

private slots:
  
  void radioClicked( QAbstractButton * button );
};



#endif // __FLASH_FUNCTION_EDITOR_H__
