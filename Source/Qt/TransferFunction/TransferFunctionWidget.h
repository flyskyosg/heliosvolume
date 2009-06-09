
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Widget to edit transfer functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __TRANSFER_FUNCTION_WIDGET_H__
#define __TRANSFER_FUNCTION_WIDGET_H__

#include "OsgVolume/TransferFunction1D.h"

#include "QtGui/QGraphicsView"

class QGraphicsScene;
class Point;

class TransferFunctionWidget : public QGraphicsView
{
  Q_OBJECT;
  
public:
  typedef QGraphicsView BaseClass;
  
  TransferFunctionWidget ( QWidget* parent = 0x0 );
  virtual ~TransferFunctionWidget();
  
  void forceRedraw();

  void setTransferFunction ( OsgVolume::TransferFunction1D* tf );
  
protected:

  virtual void drawBackground ( QPainter *painter, const QRectF &rect );
  virtual void drawItems ( QPainter *painter, int numItems, QGraphicsItem *items[], const QStyleOptionGraphicsItem options[] );

  virtual void mousePressEvent ( QMouseEvent * event );
  virtual void mouseReleaseEvent ( QMouseEvent *event );

  void _drawLineStrip ( QPainter *painter );

  Point* _findPointOnLeft ( QPointF position );
  
  void _updateTransferFunction();

private:
  
  QGraphicsScene *_scene;
  Point *_first;
  Point *_last;
  
  OsgVolume::TransferFunction1D::RefPtr _transferFunction;
};


#endif // __TRANSFER_FUNCTION_WIDGET_H__
