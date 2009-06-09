
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

#include "TransferFunctionWidget.h"
#include "Point.h"

#include "Usul/Documents/Manager.h"

#include "QtGui/QMouseEvent"
#include "QtGui/QPaintEngine"
#include "QtOpenGL/QGLFormat"
#include "QtOpenGL/QGLContext"

#include <iostream>


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef OsgVolume::TransferFunction1D::OpacityMap OpacityMap;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TransferFunctionWidget::TransferFunctionWidget ( QWidget* parent ) : BaseClass ( parent ),
  _scene ( 0x0 ),
  _first ( 0x0 ),
  _last  ( 0x0 ),
  _transferFunction ( 0x0 )
{
  const int xMin ( 0 );
  const int yMin ( 0 );
  const int xMax ( 300 );
  const int yMax ( 100 );

  this->setViewport ( new QGLWidget ( QGLFormat ( QGL::SampleBuffers ) ) );
  this->setViewportUpdateMode ( QGraphicsView::FullViewportUpdate );

  _scene = new QGraphicsScene ( this );
  _scene->setItemIndexMethod(QGraphicsScene::NoIndex);
  _scene->setSceneRect ( xMin, yMin, xMax, yMax );
  this->setScene ( _scene );

  this->setCacheMode(CacheNone);
  this->setRenderHint(QPainter::Antialiasing);
  this->setTransformationAnchor(AnchorUnderMouse);
  this->setResizeAnchor(AnchorViewCenter);

  Point* first ( new Point ( this ) );
  Point* last ( new Point ( this ) );

  // Hook up linked list.
  first->right ( last );
  last->left ( first );

  _scene->addItem ( first );
  _scene->addItem ( last );

  first->setPos ( xMin, yMax / 2 );
  last->setPos ( xMax, yMax / 2 );

  _first = first;
  _last = last;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TransferFunctionWidget::~TransferFunctionWidget()
{
  Point *current ( _first );
  while ( 0x0 != current )
  {
    std::cout << "[" << current->pos().x() << "," << current->pos().y() << "]" << std::endl;
    current = current->right();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw the background.
//
///////////////////////////////////////////////////////////////////////////////

void TransferFunctionWidget::drawBackground ( QPainter *painter, const QRectF &rect )
{
  QRectF sceneRect ( this->sceneRect() );

  if ( QPaintEngine::OpenGL == painter->paintEngine()->type() )
  {
    const qreal xMin ( sceneRect.x() );
    const qreal xMax ( xMin + sceneRect.width() );
    const qreal yMin ( sceneRect.y() );
    const qreal yMax ( yMin + sceneRect.height() );

    ::glDisable( GL_LIGHTING );
    ::glBegin( GL_QUADS );

    ::glNormal3f ( 0.0, 0.0, 1.0 );

    // TODO: Instead of color, use texture from transfer function.
    ::glColor4f( 0.5, 0.5, 0.5, 1.0 );

    ::glTexCoord2f( 0.0, 0.0 );
    ::glVertex3f ( xMin, yMin, 0.0 );

    ::glTexCoord2f( 0.0, 1.0 );
    ::glVertex3f ( xMin, yMax, 0.0 );

    ::glTexCoord2f( 1.0, 1.0 );
    ::glVertex3f ( xMax, yMax, 0.0 );

    ::glTexCoord2f( 1.0, 0.0 );
    ::glVertex3f ( xMax, yMin, 0.0 );

    ::glEnd();
  }

  // Draw the border.
  painter->setBrush ( Qt::NoBrush );
  painter->drawRect ( sceneRect );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw the line first, and then draw the items..
//
///////////////////////////////////////////////////////////////////////////////

void TransferFunctionWidget::drawItems ( QPainter *painter, int numItems, QGraphicsItem *items[], const QStyleOptionGraphicsItem options[] )
{
  // Draw the line strip between points.
  this->_drawLineStrip ( painter );

  BaseClass::drawItems ( painter, numItems, items, options );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw the line.
//
///////////////////////////////////////////////////////////////////////////////

void TransferFunctionWidget::_drawLineStrip ( QPainter *painter )
{
  painter->setPen ( QPen ( Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );

  Point *current ( _first );
  while ( 0x0 != current && 0x0 != current->right() )
  {
    painter->drawLine ( current->pos(), current->right()->pos() );
    current = current->right();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Force a redraw.
//
///////////////////////////////////////////////////////////////////////////////

void TransferFunctionWidget::forceRedraw()
{
  QRectF viewport ( _scene->sceneRect() );
  _scene->invalidate ( viewport );
  this->update();
  
  this->_updateTransferFunction();
}


///////////////////////////////////////////////////////////////////////////////
//
//  A mouse button has been pressed.
//
///////////////////////////////////////////////////////////////////////////////

void TransferFunctionWidget::mousePressEvent ( QMouseEvent * event )
{
  if ( true == event->buttons().testFlag ( Qt::RightButton ) )
  {
    Point *point ( new Point ( this ) );
    _scene->addItem( point );

    QPointF position ( this->mapToScene ( event->pos() ) );
    point->setPos ( position );

    // Find the point to the left of the position.
    Point *left ( this->_findPointOnLeft( position ) );
    if ( left )
    {
      Point* temp ( left->right() );

      // Set the new right point.
      left->right( point );

      // Set the pointers for our new point.
      point->left( left );
      point->right( temp );
    }

    this->forceRedraw();
  }

  BaseClass::mousePressEvent ( event );
}


///////////////////////////////////////////////////////////////////////////////
//
//  A mouse button has been released.
//
///////////////////////////////////////////////////////////////////////////////

void TransferFunctionWidget::mouseReleaseEvent ( QMouseEvent *event )
{
  Usul::Interfaces::IDocument::RefPtr activeDocument ( Usul::Documents::Manager::instance().activeDocument() );
  if ( activeDocument.valid() )
    activeDocument->requestRedraw();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the point to the left of position.
//
///////////////////////////////////////////////////////////////////////////////

Point* TransferFunctionWidget::_findPointOnLeft ( QPointF position )
{
  Point *current ( _first );
  while ( 0x0 != current && 0x0 != current->right() )
  {
    Point* right ( current->right() );
    if ( right->pos().x() > position.x() )
      return current;

    current = right;
  }

  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the transfer function.
//
///////////////////////////////////////////////////////////////////////////////

void TransferFunctionWidget::setTransferFunction ( OsgVolume::TransferFunction1D* tf )
{
  // Scene is created in the constructor.  Should never be null.
  USUL_ASSERT ( 0x0 != _scene );

  // Clear what we have.
  _scene->clear();
  _first = 0x0;
  _last = 0x0;
  
  _transferFunction = tf;
  
  if ( 0x0 != tf )
  {
    OpacityMap opacities ( tf->opacityMap() );
    
    if ( opacities.size() < 2 )
      return;
    
    OpacityMap::const_iterator firstIterator ( opacities.begin() );
    OpacityMap::const_iterator lastIterator;
    
    for ( OpacityMap::const_iterator iter = firstIterator; iter != opacities.end(); ++iter )
    {
      lastIterator = iter;
    }
    
    QRectF sceneRectangle ( _scene->sceneRect() );

    const qreal xMin ( sceneRectangle.x() );
    const qreal xMax ( xMin + sceneRectangle.width() );
    const qreal yMin ( sceneRectangle.y() );
    const qreal yMax ( yMin + sceneRectangle.height() );
    
    Point* firstItem ( new Point ( this ) );
    Point* lastItem ( new Point ( this ) );
    
    // Hook up linked list.
    firstItem->right ( lastItem );
    lastItem->left ( firstItem );
    
    _scene->addItem ( firstItem );
    _scene->addItem ( lastItem );
    
    firstItem->setPos ( xMin, yMax - ( yMax * firstIterator->second ) );
    lastItem->setPos ( xMax, yMax - ( yMax * lastIterator->second ) );
    
    Point* previousItem ( firstItem );
    
    OpacityMap::const_iterator currentIterator ( ++opacities.begin() );
    while ( currentIterator != lastIterator )
    {
      const double u ( static_cast<double> ( currentIterator->first - firstIterator->first ) / ( lastIterator->first - firstIterator->first ) );
      const double x ( xMin + ( sceneRectangle.width() * u ) );
      const double y ( yMax - ( yMax * currentIterator->second ) );
      
      Point* point ( new Point ( this ) );
      point->setPos ( x, y );
      _scene->addItem ( point );
      
      previousItem->right ( point );
      point->left ( previousItem );
      previousItem = point;
      
      ++currentIterator;
    }
    
    lastItem->left ( previousItem );
    
    _first = firstItem;
    _last = lastItem;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the transfer function.
//
///////////////////////////////////////////////////////////////////////////////

void TransferFunctionWidget::_updateTransferFunction()
{
  if ( _transferFunction.valid() )
  {
    OpacityMap opacities;
    
    QRectF sceneRectangle ( _scene->sceneRect() );
    
    const qreal xMin ( sceneRectangle.x() );
    const qreal yMin ( sceneRectangle.y() );
    const qreal yMax ( yMin + sceneRectangle.height() );
    
    Point *current ( _first );
    while ( 0x0 != current )
    {
      const double x ( current->pos().x() );
      const double y ( current->pos().y() );
      
      const unsigned int index ( ( ( x - xMin ) / sceneRectangle.width() ) * 255 );
      const double opacity ( ( yMax - y ) / yMax );
      
      opacities[index] = opacity;
      
      current = current->right();
    }
    
    _transferFunction->opacityMap ( opacities );
    _transferFunction->calculateColors();
  }
}
