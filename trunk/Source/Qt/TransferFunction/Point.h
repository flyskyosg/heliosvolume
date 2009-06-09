
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
//  Point of a transfer function.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __TRANSFER_FUNCTION_POINT_H__
#define __TRANSFER_FUNCTION_POINT_H__

#include "QtGui/QGraphicsItem"

class TransferFunctionWidget;

class Point : public QGraphicsItem
{
public:
  typedef QGraphicsItem BaseClass;
  
  Point ( TransferFunctionWidget* parent = 0x0 );
  virtual ~Point();
  
  /// Set/get the point to the left.
  void left ( Point* point );
  Point* left() const;
  
  /// Set/get the point to the right.
  void right ( Point* point );
  Point* right() const;
  
  virtual QRectF boundingRect() const;
  void paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );

protected:

  void _clampToRectangle ( const QRectF& rectangle );
  void _clampToLeftAndRight();

  QVariant itemChange ( GraphicsItemChange change, const QVariant &value );

  void mouseMoveEvent ( QGraphicsSceneMouseEvent *event );
  void mousePressEvent ( QGraphicsSceneMouseEvent *event );
  void mouseReleaseEvent ( QGraphicsSceneMouseEvent *event );

private:
  
  double _radius;
  Point *_left;
  Point *_right;

  TransferFunctionWidget *_parent;
};

#endif // __TRANSFER_FUNCTION_POINT_H__
