
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for Volume models.
//
///////////////////////////////////////////////////////////////////////////////

#include "VolumeModel/VolumeDocument.h"
#include "VolumeModel/ImageReaderWriter.h"
#include "VolumeModel/RawReaderWriter.h"

#include "OsgVolume/Image3d.h"
#include "OsgVolume/Texture3DVolume.h"
#include "OsgVolume/GPURayCasting.h"

#include "OsgTools/Box.h"
#include "OsgTools/State/StateSet.h"

#include "Usul/File/Path.h"
#include "Usul/Strings/Case.h"
#include "Usul/Interfaces/IViewport.h"
#include "Usul/Interfaces/IViewMatrix.h"

#include "osg/MatrixTransform"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( VolumeDocument, VolumeDocument::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

VolumeDocument::VolumeDocument() : BaseClass ( "Volume Document" ),
  _root ( new osg::Group ),
  _projection ( new osg::Projection ),
  _image3D ( 0x0 ),
  _bb ( -1.0, -1.0, -1.0, 1.0, 1.0, 1.0 ),
  _readerWriter ( 0x0 ),
  _dirty ( false ),
  _transferFunctions(),
  _activeTransferFunction()
{
  OsgVolume::TransferFunction1D::RefPtr tf ( new OsgVolume::TransferFunction1D );
  tf->color ( 0, Usul::Math::Vec3f ( 0.0f, 0.0f, 1.0f ) );
  tf->opacity ( 0, 0.01f );
  
  tf->color ( 255, Usul::Math::Vec3f ( 1.0f, 0.0f, 0.0f ) );
  tf->opacity ( 255, 1.0f );
  
  tf->colorMode ( OsgVolume::TransferFunction1D::COLOR_MODE_HSV );
  
  this->addTransferFunction ( tf.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

VolumeDocument::~VolumeDocument()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *VolumeDocument::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast < Usul::Interfaces::IUpdateListener * > ( this );
  case OsgVolume::ITransferFunction1DList::IID:
    return static_cast<OsgVolume::ITransferFunction1DList*> ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool VolumeDocument::canExport ( const std::string &file ) const
{
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool VolumeDocument::canInsert ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "jpeg" || ext == "jpg" || ext == "tiff" || ext == "tif" || ext == "gif" || ext == "png" || ext == "bmp" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool VolumeDocument::canOpen ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "rawvol" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool VolumeDocument::canSave ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file and add it to existing data.
//
///////////////////////////////////////////////////////////////////////////////

void VolumeDocument::read ( const std::string &name, Unknown *caller, Unknown *progress )
{
  if ( false == _readerWriter.valid () )
  {
    // Image reader writer is the default.
    _readerWriter = new ImageReaderWriter;

    std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( name ) ) );
    if ( "rawvol" == ext )
      _readerWriter = new RawReaderWriter;
  }

  if ( _readerWriter.valid () )
    _readerWriter->read ( name, *this, caller );

  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void VolumeDocument::write ( const std::string &filename, Unknown *caller, Unknown *progress ) const
{
  if ( _readerWriter.valid () )
    _readerWriter->write ( filename, *this, caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void VolumeDocument::clear ( Usul::Interfaces::IUnknown *caller )
{
  if ( _readerWriter.valid () )
    _readerWriter->clear ( caller );
  _readerWriter = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

VolumeDocument::Filters VolumeDocument::filtersExport() const
{
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

VolumeDocument::Filters VolumeDocument::filtersInsert() const
{
  Filters filters;
  filters.push_back ( Filter ( "All Images (*.jpeg *.jpg *.tiff *.tif *.png *.gif *.bmp)", "*.jpeg;*.jpg;*.tiff;*.tif;*.png;*.gif;*.bmp" ) );
  filters.push_back ( Filter ( "JPEG (*.jpeg *.jpg)", "*.jpeg;*.jpg" ) );
  filters.push_back ( Filter ( "TIFF (*.tiff *.tif)", "*.tiff;*.tif" ) );
  filters.push_back ( Filter ( "PNG  (*.png)",        "*.png"        ) );
  filters.push_back ( Filter ( "GIF  (*.gif)",        "*.gif"        ) );
  filters.push_back ( Filter ( "BMP  (*.bmp)",        "*.bmp"        ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

VolumeDocument::Filters VolumeDocument::filtersOpen() const
{
  Filters filters;
  filters.push_back ( Filter ( "Raw  (*.rawvol)",        "*.rawvol"        ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

VolumeDocument::Filters VolumeDocument::filtersSave() const
{
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *VolumeDocument::buildScene ( const BaseClass::Options &opt, Unknown *caller )
{
  this->setStatusBar ( "Building scene..." );

  this->_buildScene ();

  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void VolumeDocument::dirty ( bool b )
{
  Guard guard ( this->mutex() );
  _dirty = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

bool VolumeDocument::dirty () const
{
  Guard guard ( this->mutex() );
  return _dirty;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void VolumeDocument::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  if ( this->dirty() )
    this->_buildScene();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void VolumeDocument::_buildScene ()
{
  _root->removeChildren ( 0, _root->getNumChildren() );

  if ( _readerWriter.valid () )
  {
#if 1
    osg::ref_ptr<OsgVolume::Texture3DVolume> volume ( new OsgVolume::Texture3DVolume );
    volume->numPlanes ( 256 );
    volume->resizePowerTwo ( true );
    volume->image ( this->image3D() );
    volume->boundingBox ( this->boundingBox() );
    
#else
    osg::ref_ptr < OsgVolume::GPURayCasting > volume ( new OsgVolume::GPURayCasting );
    volume->samplingRate ( 0.05 );
    //volume->resizePowerTwo ( true );
    volume->image ( this->image3D() );
    volume->boundingBox ( this->boundingBox() );
    
#endif

    if ( _transferFunctions.size() > 0 )
      volume->transferFunction ( _transferFunctions.at ( _activeTransferFunction ) );
    
    OsgTools::ColorBox box ( volume->boundingBox() );
    box.color_policy().color ( osg::Vec4 ( 0, 0, 1, 1 ) );
    
    // Position it.
    osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
    mt->setMatrix ( osg::Matrix::translate ( volume->boundingBox().center() ) );
    mt->addChild ( box() );
    
    // Wire-frame.
    OsgTools::State::StateSet::setPolygonsLines ( mt.get(), true );
    OsgTools::State::StateSet::setLighting ( mt.get(), false );
    
    _root->addChild ( mt.get() );
    
    _root->addChild ( volume.get() );
  }

  this->dirty ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the 3D image.
//
///////////////////////////////////////////////////////////////////////////////

void VolumeDocument::image3D ( osg::Image* image )
{
  {
    Guard guard ( this->mutex() );
    _image3D = image;
  }
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the 3D image.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* VolumeDocument::image3D () const
{
  Guard guard ( this->mutex() );
  return _image3D.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

void VolumeDocument::boundingBox ( const osg::BoundingBox& bb )
{
  {
    Guard guard ( this->mutex() );
    _bb = bb;
  }
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

osg::BoundingBox VolumeDocument::boundingBox() const
{
  Guard guard ( this->mutex() );
  return _bb;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a transfer function.
//
///////////////////////////////////////////////////////////////////////////////

void VolumeDocument::addTransferFunction ( TransferFunction* tf )
{
  Guard guard ( this->mutex () );
  _activeTransferFunction = _transferFunctions.size();
  _transferFunctions.push_back ( tf );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a transfer function (ITransferFunction1DList).
//
///////////////////////////////////////////////////////////////////////////////

void VolumeDocument::addTransferFunction1D ( TransferFunction1DPtr tf )
{
  this->addTransferFunction ( tf );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the i'th transfer function (ITransferFunction1DList).
//
///////////////////////////////////////////////////////////////////////////////

VolumeDocument::TransferFunction1DPtr VolumeDocument::getTransferFunction1D ( unsigned int i ) const
{
  Guard guard ( this->mutex() );
  return ( i < _transferFunctions.size() ? _transferFunctions.at ( i ) : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of 1D transfer functions (ITransferFunction1DList).
//
///////////////////////////////////////////////////////////////////////////////

unsigned int VolumeDocument::getNumberOfTransferFunctions1D() const
{
  Guard guard ( this->mutex() );
  return _transferFunctions.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the active transfer function (ITransferFunction1DList).
//
///////////////////////////////////////////////////////////////////////////////

void VolumeDocument::setActiveTransferFunction ( unsigned int index )
{
  Guard guard ( this->mutex() );
  _activeTransferFunction = index;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the active transfer function (ITransferFunction1DList).
//
///////////////////////////////////////////////////////////////////////////////

unsigned int VolumeDocument::getActiveTransferFunction() const
{
  Guard guard ( this->mutex() );
  return _activeTransferFunction;
}
