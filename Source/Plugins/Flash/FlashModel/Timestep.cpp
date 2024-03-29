
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Timestep.h"
#include "H5File.h"
#include "Dataset.h"
#include "IFlashDocument.h"

#include "Usul/Functions/Color.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Documents/Document.h"
#include "Usul/Documents/Manager.h"

#include "OsgTools/Box.h"
#include "OsgTools/State/StateSet.h"

#include "osg/MatrixTransform"
#include "osg/Point"

#include "hdf5.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Timestep::Timestep( const std::string& filename ) : BaseClass(),
  _filename ( filename ),
  _boundingBoxes(),
  _leafFlags(),
  _levels(),
  _data (),
  _minimum ( 0.0 ),
  _maximum ( 0.0 ),
  _hierarchy(),
  _secondValue(),
  _vMinimum ( 1.0 ),
  _vMaximum ( 1.0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Timestep::~Timestep()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

void Timestep::init()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  
  // Open the file.
  H5File file ( _filename );
  
  // Make sure we got a good file handle.
  if ( false == file.isOpen() )
    throw std::runtime_error ( "Error 1747059583: Could not open file: " + _filename );
  
  // Initialize the bounding boxes.
  this->_initBoundingBoxes ( file );
  
  // Initialize the level numbers.
  this->_initLevels ( file );
  
  // Initialize flags for leaves.
  this->_initLeafFlags ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the bounding boxes.
//
///////////////////////////////////////////////////////////////////////////////

void Timestep::_initBoundingBoxes ( H5File& file )
{
  // Open the data set.
  Dataset::RefPtr boundingBox ( file.openDataset( "bounding box" ) );
  
  // Make sure we got a good data set handle.
  if ( false == boundingBox.valid() || false == boundingBox->isOpen() )
    throw std::runtime_error ( "Error 1220099573: Could not open data set." );
  
  // Get the number of elements.
  const hsize_t elements ( boundingBox->elements() );
  
  // Buffer for the data.
  std::vector<double> buffer ( elements );
  
  // Read the data.
  boundingBox->read ( H5T_NATIVE_DOUBLE, &buffer[0] );
  
  // Number of nodes.
  const unsigned int numNodes ( boundingBox->size ( 0 ) );
  
  // Clear what we have.
  _boundingBoxes.clear();
  
  // Reserve enough room.
  _boundingBoxes.reserve ( numNodes );
  
  // Make bounding boxes.
  for ( unsigned int num = 0; num < numNodes; ++num )
  {
    const unsigned int start ( num * 6 );
      
    osg::BoundingBox bb ( buffer[start    ], buffer[start + 2], buffer[start + 4], 
                          buffer[start + 1], buffer[start + 3], buffer[start + 5] );
    
    _boundingBoxes.push_back ( bb );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the leaf flags.
//
///////////////////////////////////////////////////////////////////////////////

void Timestep::_initLeafFlags ( H5File& file )
{
  // Open the data set.
  Dataset::RefPtr nodeData ( file.openDataset( "node type" ) );
  
  // Make sure we got a good data set handle.
  if ( false == nodeData.valid() || false == nodeData->isOpen() )
    throw std::runtime_error ( "Error 1356818230: Could not open data set." );
  
  // Buffer for node type.
  std::vector<int> nodeType ( nodeData->elements() );
  
  // Read the levels.
  nodeData->read ( H5T_NATIVE_INT, &nodeType[0] );
  
  // Clear what we have.
  _leafFlags.clear();
  
  // Make enough room.
  _leafFlags.reserve ( nodeType.size() );
  
  for ( unsigned int i = 0; i < nodeType.size(); ++i )
    _leafFlags.push_back ( 1 == nodeType.at ( i ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the levels.
//
///////////////////////////////////////////////////////////////////////////////

void Timestep::_initLevels ( H5File& file )
{
  // Open the data set.
  Dataset::RefPtr levelsData ( file.openDataset( "refine level" ) );
  
  // Make sure we got a good data set handle.
  if ( false == levelsData.valid() || false == levelsData->isOpen() )
    throw std::runtime_error ( "Error 3972712559: Could not open data set." );
  
  // Clear what we have.
  _levels.clear();
  
  // Make vector the correct size.
  _levels.resize ( levelsData->elements() );
  
  // Read the levels.
  levelsData->read ( H5T_NATIVE_INT, &_levels[0] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the hierarchy.
//
///////////////////////////////////////////////////////////////////////////////

void Timestep::_initHierarchy ( H5File& file )
{
  // Open the data set.
  Dataset::RefPtr data ( file.openDataset( "gid" ) );
  
  // Make sure we got a good data set handle.
  if ( false == data.valid() || false == data->isOpen() )
    throw std::runtime_error ( "Error 2495376396: Could not open data set." );
  
  // Make vector the correct size.
  _hierarchy.resize ( boost::extents [data->size ( 0 )][data->size ( 1 )] );
  
  // Read the levels.
  data->read ( H5T_NATIVE_INT, _hierarchy.origin() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the data.
//
///////////////////////////////////////////////////////////////////////////////

void Timestep::loadData ( const std::string& name, const std::string& valueName )
{
  Guard guard ( this->mutex() );
  
  // Open the file.
  H5File file ( _filename );
  
  // Make sure we got a good file handle.
  if ( false == file.isOpen() )
    throw std::runtime_error ( "Error 1747059583: Could not open file: " + _filename );
  
  // Read the density.
  Dataset::RefPtr data ( file.openDataset ( name ) );  
  
  // Make sure it's valid.
  if ( false == data.valid() || false == data->isOpen() )
    throw std::runtime_error ( "Error 6348683680: Could not open data set." );
  
  // Get the dimensions in each direction.
  const unsigned int x ( data->size ( 1 ) );
  const unsigned int y ( data->size ( 2 ) );
  const unsigned int z ( data->size ( 3 ) );
  
  // Buffer for density.
  _data.resize ( boost::extents [data->size ( 0 )][x][y][z] );
  
  // Fill the buffer.
  data->read ( H5T_NATIVE_DOUBLE, _data.origin() );
  
  // Get min and max from the data set.
  _minimum = data->attribute ( "minimum" );
  _maximum = data->attribute ( "maximum" );

  //----------------------------------------------------------------------------
  // Second value code
  //----------------------------------------------------------------------------

  // Make sure there is a second value to read
  if( valueName != "" )
  {
	  // Read the density.
	  Dataset::RefPtr vData ( file.openDataset ( valueName ) );  
	  
	  // Make sure it's valid.
	  if ( false == vData.valid() || false == vData->isOpen() )
		throw std::runtime_error ( "Error 1444387917: Could not open second data set." );
	  	  
	  // Buffer for density.
	  _secondValue.resize ( boost::extents [vData->size ( 0 )][x][y][z] );

	  // Make sure it's valid.
	  if ( _secondValue.size() != _data.size() )
		throw std::runtime_error ( "Error 2659461171: Size mismatch between first and second datasets." );
	  
	  // Fill the buffer.
	  vData->read ( H5T_NATIVE_DOUBLE, _secondValue.origin() );
	  
	  // Get min and max from the data set.
	  _vMinimum = vData->attribute ( "minimum" );
	  _vMaximum = vData->attribute ( "maximum" );
  }


}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a bounding box.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Timestep::buildBoundingBox ( const osg::BoundingBox& bb, const osg::Vec4f& color ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  
  OsgTools::ColorBox box ( bb );
  box.color_policy().color ( color );
  
  osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
  mt->setMatrix ( osg::Matrix::translate ( bb.center() ) );
  mt->addChild ( box() );
   
  // Wire-frame.
  OsgTools::State::StateSet::setPolygonsLines ( mt.get(), true );
  OsgTools::State::StateSet::setLighting ( mt.get(), false );
  
  return mt.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the points.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Timestep::buildPoints ( const osg::BoundingBox& bb, unsigned int num ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  
  // Get the dimensions in each direction.
  const unsigned int x ( _data.shape()[3] );
  const unsigned int y ( _data.shape()[2] );
  const unsigned int z ( _data.shape()[1] );
  
  // Get min and max from the data set.
  const double minimum ( _minimum );
  const double maximum ( _maximum );
  
  const osg::Vec3 min ( bb._min );
  const osg::Vec3 max ( bb._max );
  
  const double lengthX ( bb._max.x() - bb._min.x() );
  const double lengthY ( bb._max.y() - bb._min.y() );
  const double lengthZ ( bb._max.z() - bb._min.z() );
  
  const double deltaX ( lengthX / x );
  const double deltaY ( lengthY / y );
  const double deltaZ ( lengthZ / z );
  
  osg::ref_ptr<osg::Vec3Array> vertices ( new osg::Vec3Array );
  osg::ref_ptr<osg::Vec3Array> normals ( new osg::Vec3Array ( 1 ) );
  osg::ref_ptr<osg::Vec4Array> myColors ( new osg::Vec4Array );
  
  // One normal.
  normals->at ( 0 ) = osg::Vec3 ( 0.0, 0.0, 1.0 );
  
  for ( unsigned int i = 0; i < x; ++i )
  {
    for ( unsigned int j = 0; j < y; ++j )
    {
      for ( unsigned int k = 0; k < z; ++k )
      {
        //NOTES: Should be ijk order or will cause a crash.  Dimensions won't match on some files
        const double value ( _data[num][k][j][i] );
        
        float r ( 0.0 ), g ( 0.0 ), b ( 0.0 );
        
        double temp ( ( value - minimum ) / ( maximum - minimum ) );
        
        Usul::Functions::Color::hsvToRgb ( r, g, b, 300 - static_cast<float> ( temp * 300.0f ), 1.0f, 1.0f );
        
        osg::Vec3 position ( ( static_cast<double> ( i ) + 0.5 ) * deltaX, 
                            ( static_cast<double> ( j ) + 0.5 ) * deltaY, 
                            ( static_cast<double> ( k ) + 0.5 ) * deltaZ );
        vertices->push_back ( osg::Vec3 ( bb._min + position ) );
        myColors->push_back ( osg::Vec4 ( r, g, b, temp * ( 35.0f / 255.0 ) ) );
      }
    }
  }
  
  osg::ref_ptr<osg::Geometry> geometry ( new osg::Geometry );
  geometry->setVertexArray ( vertices.get() );
  geometry->setNormalArray ( normals.get() );
  geometry->setNormalBinding ( osg::Geometry::BIND_OVERALL );
  geometry->setColorArray ( myColors.get() );
  geometry->setColorBinding ( osg::Geometry::BIND_PER_VERTEX );
  geometry->setUseDisplayList ( false );
  
  geometry->addPrimitiveSet ( new osg::DrawArrays ( GL_POINTS, 0, vertices->size() ) );
  
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
  geode->addDrawable ( geometry.get() );
  
  osg::ref_ptr<osg::Point> ps ( new osg::Point );
  ps->setSize ( 5.0f );
  geode->getOrCreateStateSet()->setAttributeAndModes ( ps.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
  
  // Turn off lighting
  OsgTools::State::StateSet::setLighting ( geode.get(), false );
  
  geode->getOrCreateStateSet()->setRenderBinDetails ( 1, "DepthSortedBin" );
  
  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a volume.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* Timestep::buildVolume ( unsigned int num, double minimum, double maximum ) const
{
  // Query the active document for IVaporIntrusionGUI
  Flash::IFlashDocument::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

	int type ( Flash::IFlashDocument::NO_FUNCTION );
  // Check for a valid document
  if( true == document.valid() )
  {
	//throw std::runtime_error ( "Error 1932718685: Invalid Document in buildVolume().  Unable to apply a function." );
	  
	type = document->functionType();
  }

  // get the user specified function type
  // int type ( document->functionType() );

  // Get the dimensions in each direction.
  const unsigned int x ( _data.shape()[3] );
  const unsigned int y ( _data.shape()[2] );
  const unsigned int z ( _data.shape()[1] );

  // if there is a function to apply to the values apply them to the min/max as well
  if( minimum == maximum )
  {
	  minimum = this->_applyFunction( type, _minimum, _vMinimum );
	  maximum = this->_applyFunction( type, _maximum, _vMaximum );
  }
// else
// {
//	  minimum = this->_applyFunction( type, minimum, _vMinimum );
//	  maximum = this->_applyFunction( type, maximum, _vMaximum );		
//  }
	
  // TODO: remove before final build
  // some debug statements
  // std::cout << "Min/MAX: { " << minimum << " | " << maximum << " }" << std::endl;

  // Get the 3D image for the volume.
  osg::ref_ptr<osg::Image> image ( new osg::Image );
  
  image->allocateImage ( x, y, z, GL_LUMINANCE, GL_UNSIGNED_BYTE );
  //image->allocateImage ( x, y, z, GL_LUMINANCE, GL_FLOAT );
  
  for ( unsigned int i = 0; i < x; ++i )
  {
    for ( unsigned int j = 0; j < y; ++j )
    {
      for ( unsigned int k = 0; k < z; ++k )
      {
        // get the value
        double value ( _data[num][k][j][i] );
		
		// get the second value
		double value2 ( 1.0 );
		if( _secondValue.size() == _data.size() )
         ( value2 = _secondValue[num][k][j][i] );

        // apply the function
        value = this->_applyFunction( type, value, value2 );
        
        // clamp the value
        value = Usul::Math::clamp ( value, minimum, maximum );
#if 1
        value = ( value - minimum ) / ( maximum - minimum );
        const unsigned char pixel ( static_cast < unsigned char > ( value * 255 ) );
        *image->data( i, j, k ) = pixel;
#else
        float* pixel ( reinterpret_cast<float*> ( image->data( i, j, k ) ) );
        *pixel = static_cast < float > ( ( value - minimum ) / ( maximum - minimum ) );
#endif
      }
    }
  }
  
  return image.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the node a leaf?
//
///////////////////////////////////////////////////////////////////////////////

bool Timestep::isLeaf( unsigned int i ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _leafFlags.at ( i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of nodes.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Timestep::numNodes() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _boundingBoxes.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the level of the node.
//
///////////////////////////////////////////////////////////////////////////////

int Timestep::level ( unsigned int i ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _levels.at ( i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the min value.
//
///////////////////////////////////////////////////////////////////////////////

double Timestep::minimum() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _minimum;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the max value.
//
///////////////////////////////////////////////////////////////////////////////

double Timestep::maximum() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _maximum;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

osg::BoundingBox Timestep::boundingBox( unsigned int i ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _boundingBoxes.at ( i );
}


///////////////////////////////////////////////////////////////////////////////
//
// Apply a function to the given value based on the function code.
//
///////////////////////////////////////////////////////////////////////////////

double Timestep::_applyFunction( int functionCode, double value, double value2 ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  switch ( functionCode )
  {
    // Take the absolute value of the input value
    case Flash::IFlashDocument::ABS_FUNCTION:
    {
      value = abs( value );
      break;
    }
    // Apply a log function to the input value
    case Flash::IFlashDocument::LOG_FUNCTION:
    {
      value = log10( value );
      break;
    }
    // Apply a scalar multiplier to the input value
    case Flash::IFlashDocument::SCALAR_MULT_FUNCTION:
    {
      value *= value2;
      break;
    }
		  // Apply a scalar multiplier to the input value
	case Flash::IFlashDocument::MULT_LOG_FUNCTION:
	{
	  value *= value2;
	  value = log10( value );
	  break;
	}
    // do nothing to the value
    default:
      break;
  }

  // return the modified value
  return value;
}
