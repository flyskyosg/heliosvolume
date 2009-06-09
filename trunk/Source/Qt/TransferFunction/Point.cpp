
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

#include "Point.h"
#include "TransferFunctionWidget.h"

#include "QtGui/QPainter"
#include "QtGui/QStyleOption"
#include "QtGui/QGraphicsSceneMouseEvent"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Point::Point (TransferFunctionWidget* parent ) : BaseClass(),
  _radius ( 8.0 ),
  _left ( 0x0 ),
  _right ( 0x0 ),
  _parent ( parent )
{
  this->setFlag ( ItemIsMovable );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Point::~Point()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the point to the left.
//
///////////////////////////////////////////////////////////////////////////////

void Point::left ( Point* point )
{
  _left = point;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the point to the left.
//
///////////////////////////////////////////////////////////////////////////////

Point* Point::left() const
{
  return _left;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the point to the right.
//
///////////////////////////////////////////////////////////////////////////////

void Point::right ( Point* point )
{
  _right = point;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the point to the right.
//
///////////////////////////////////////////////////////////////////////////////

Point* Point::right() const
{
  return _right;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the bounding rectangle.
//
///////////////////////////////////////////////////////////////////////////////

QRectF Point::boundingRect() const
{
  const double padding ( 2 );
  const double diameter ( _radius * 2 );
  return QRectF (-_radius - padding, -_radius - padding,
                  diameter + padding, diameter + padding );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Paint.
//
///////////////////////////////////////////////////////////////////////////////

void Point::paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * /*widget*/ )
{
  QRadialGradient gradient ( 0.0, 0.0, _radius );

  // If we are selected, use a lighter color.
  if ( option->state & QStyle::State_Sunken )
  {
    gradient.setColorAt ( 0, QColor ( Qt::yellow ).light ( 120 ) );
    gradient.setColorAt ( 1, QColor ( Qt::darkYellow ).light ( 120 ) );
    painter->setPen ( QPen ( Qt::red, 0 ) );
  }
  else
  {
    gradient.setColorAt ( 0, Qt::yellow );
    gradient.setColorAt ( 1, Qt::darkYellow );
    painter->setPen ( QPen ( Qt::black, 0 ) );
  }

  painter->setBrush ( gradient );

  const double diameter ( _radius * 2 );
  painter->drawEllipse ( -_radius, -_radius, diameter, diameter);
}


///////////////////////////////////////////////////////////////////////////////
//
//  The item has changed.
//
///////////////////////////////////////////////////////////////////////////////

QVariant Point::itemChange(GraphicsItemChange change, const QVariant &value)
{
  switch (change) {
  case ItemPositionHasChanged:
    if ( 0x0 != _parent )
    {
      _parent->forceRedraw();
    }
    break;
  default:
    break;
  };

  return BaseClass::itemChange(change, value);
}


///////////////////////////////////////////////////////////////////////////////
//
//  The mouse has moved.
//
///////////////////////////////////////////////////////////////////////////////

void Point::mouseMoveEvent ( QGraphicsSceneMouseEvent *event )
{
  this->update();
  BaseClass::mouseMoveEvent(event);

  QRectF boundingBox ( this->scene()->sceneRect() );

  // Reset our x position if we are one of the end points.
  if ( 0x0 == _left )
  {
    this->setPos ( 0, this->pos().y() );
  }

  if ( 0x0 == _right )
  {
    this->setPos ( boundingBox.x() + boundingBox.width(), this->pos().y() );
  }

  this->_clampToRectangle ( boundingBox );
  this->_clampToLeftAndRight();
}


///////////////////////////////////////////////////////////////////////////////
//
//  A mouse button has been pressed.
//
///////////////////////////////////////////////////////////////////////////////

void Point::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  this->update();
  BaseClass::mousePressEvent(event);
}


///////////////////////////////////////////////////////////////////////////////
//
//  A mouse button has been released.
//
///////////////////////////////////////////////////////////////////////////////

void Point::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  this->update();
  BaseClass::mouseReleaseEvent(event);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clamp so that this point is still between the left and the right.
//
///////////////////////////////////////////////////////////////////////////////

void Point::_clampToLeftAndRight()
{
  if ( 0x0 != _left )
  {
    if ( this->pos().x() <= _left->pos().x() )
      this->setPos ( _left->pos().x() + 1, this->pos().y() );
  }

  if ( 0x0 != _right )
  {
    if ( this->pos().x() >= _right->pos().x() )
      this->setPos ( _right->pos().x() - 1, this->pos().y() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clamp the position to the given rectangle.
//
///////////////////////////////////////////////////////////////////////////////

void Point::_clampToRectangle ( const QRectF& rectangle )
{
  qreal x ( this->pos().x() );
  qreal y ( this->pos().y() );

  const qreal xMin ( rectangle.x() );
  const qreal xMax ( xMin + rectangle.width() );
  const qreal yMin ( rectangle.y() );
  const qreal yMax ( yMin + rectangle.height() );

  x = qMax ( x, xMin );
  x = qMin ( x, xMax );

  y = qMax ( y, yMin );
  y = qMin ( y, yMax );

  this->setPos ( x, y );
}
