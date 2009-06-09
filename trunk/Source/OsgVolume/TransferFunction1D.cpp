
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgVolume/TransferFunction1D.h"

#include "Usul/Functions/Color.h"
#include "Usul/Math/Interpolate.h"

#include "osg/Texture1D"

#include <iostream>

using namespace OsgVolume;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TransferFunction1D::TransferFunction1D () : 
  BaseClass(),
  _minimium ( 0 ),
  _maximium ( 0 ),
  _colors(),
  _colorMap(),
  _opacityMap(),
  _colorMode ( COLOR_MODE_RGB )
{
  this->_init();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

void TransferFunction1D::_init()
{
  _image = new osg::Image;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

TransferFunction1D::~TransferFunction1D()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of dimensions.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int TransferFunction1D::dimensions() const
{
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the mininium value.
//
///////////////////////////////////////////////////////////////////////////////

double TransferFunction1D::minimium() const
{
  return _minimium;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the mininium value.
//
///////////////////////////////////////////////////////////////////////////////

void TransferFunction1D::minimium ( double minimium )
{
  _minimium = minimium;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the maximium value.
//
///////////////////////////////////////////////////////////////////////////////

double TransferFunction1D::maximium() const
{
  return _maximium;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the maximium value.
//
///////////////////////////////////////////////////////////////////////////////

void TransferFunction1D::maximium ( double maximium )
{
  _maximium = maximium;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the texture.
//
///////////////////////////////////////////////////////////////////////////////

osg::Texture* TransferFunction1D::texture()
{
  this->calculateColors();
  
  // Create the 1D texture.
  osg::ref_ptr < osg::Texture1D > texture1D ( new osg::Texture1D );
  texture1D->setImage ( _image.get() );
  
  texture1D->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
  texture1D->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
  texture1D->setWrap  ( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE );
  texture1D->setInternalFormatMode ( osg::Texture::USE_IMAGE_DATA_FORMAT );
  
  return texture1D.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color.
//
///////////////////////////////////////////////////////////////////////////////

void TransferFunction1D::color ( unsigned int index, const RGB& color )
{
  _colorMap[index] = color;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the opacity.
//
///////////////////////////////////////////////////////////////////////////////

void TransferFunction1D::opacity ( unsigned int index, RGB::value_type opacity )
{
  _opacityMap[index] = opacity;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate all the colors.
//
///////////////////////////////////////////////////////////////////////////////

void TransferFunction1D::calculateColors()
{
  // Return if we don't have enough colors.
  if ( _colorMap.size() < 2 || _opacityMap.size() < 2 )
    return;
  
  _colors.clear();
  
      // First interpolate the colors.
  {
    ColorMap::const_iterator current ( _colorMap.begin() );
    ColorMap::const_iterator next ( ++_colorMap.begin() );
    
    while ( next != _colorMap.end() )
    {
      unsigned int index0 ( current->first );
      unsigned int index1 ( next->first );
      
      RGB color0 ( current->second );
      RGB color1 ( next->second );
      
      _colors.resize ( index1 + 1 );
      
      for ( unsigned int i = index0; i <= index1; ++i )
      {
        if ( index0 != index1 )
        {
          double u ( static_cast<double> ( i - index0 ) / ( index1 - index0 ) );
          RGB color ( this->_interpolate ( u, color0, color1 ) );
          _colors.at ( i ).set ( color[0], color[1], color[2], 1.0 );
        }
      }
      
      current = next;
      ++next;
    }
  }
  
  // Interpolate the opacities.
  {
    OpacityMap::const_iterator current ( _opacityMap.begin() );
    OpacityMap::const_iterator next ( ++_opacityMap.begin() );
    
    while ( next != _opacityMap.end() )
    {
      unsigned int index0 ( current->first );
      unsigned int index1 ( next->first );
      
      RGB::value_type opacity0 ( current->second );
      RGB::value_type opacity1 ( next->second );
      
      //colors.resize ( index1 + 1 );
      
      for ( unsigned int i = index0; i <= index1; ++i )
      {
        if ( index0 != index1 && i < _colors.size() )
        {
          double u ( static_cast<double> ( i - index0 ) / ( index1 - index0 ) );
          RGB::value_type opacity ( Usul::Math::Interpolate<float>::linear ( u, opacity0, opacity1 ) );
          _colors.at ( i )[3] = opacity;
        }
      }
      
      current = next;
      ++next;
    }
  }
  
  /*for ( unsigned int i = 0; i < _colors.size(); ++i )
  {
    Color color ( _colors[i] );
    std::cout << i 
      << ": " 
      << static_cast<unsigned int> ( color[0] )
      << " " 
      << static_cast<unsigned int> ( color[1] )
      << " " 
      << static_cast<unsigned int> ( color[2] )
      << " " 
      << static_cast<unsigned int> ( color[3] )
      << std::endl;
  }*/
  
  // Set the image data.
  _image->setImage( _colors.size(), 1, 1, GL_RGBA, GL_RGBA, GL_FLOAT, reinterpret_cast < unsigned char * > ( &_colors[0] ), osg::Image::NO_DELETE );
  _image->dirty();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Interpolate the color.
//
///////////////////////////////////////////////////////////////////////////////

TransferFunction1D::RGB TransferFunction1D::_interpolate ( double u, const RGB& color0, const RGB& color1 ) const
{
  RGB answer ( color0 );
  
  if ( COLOR_MODE_RGB == _colorMode )
  {
    answer[0] = Usul::Math::Interpolate<RGB::value_type>::linear ( u, color0[0], color1[0] );
    answer[1] = Usul::Math::Interpolate<RGB::value_type>::linear ( u, color0[1], color1[1] );
    answer[2] = Usul::Math::Interpolate<RGB::value_type>::linear ( u, color0[2], color1[2] );
  }
  else if ( COLOR_MODE_HSV == _colorMode )
  {
    typedef Usul::Math::Vec3f HSV;
    HSV hsv0, hsv1;
    Usul::Functions::Color::rgbToHsv ( hsv0[0], hsv0[1], hsv0[2], color0[0], color0[1], color0[2] );
    Usul::Functions::Color::rgbToHsv ( hsv1[0], hsv1[1], hsv1[2], color1[0], color1[1], color1[2] );
    
    HSV hsv;
    hsv[0] = Usul::Math::Interpolate<RGB::value_type>::linear ( u, hsv0[0], hsv1[0] );
    hsv[1] = Usul::Math::Interpolate<RGB::value_type>::linear ( u, hsv0[1], hsv1[1] );
    hsv[2] = Usul::Math::Interpolate<RGB::value_type>::linear ( u, hsv0[2], hsv1[2] );
    
    Usul::Math::Vec3f temp;
    Usul::Functions::Color::hsvToRgb ( temp[0], temp[1], temp[2], hsv[0], hsv[1], hsv[2] );
    answer[0] = temp[0];
    answer[1] = temp[1];
    answer[2] = temp[2];
  }
  
  return answer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color map.
//
///////////////////////////////////////////////////////////////////////////////

void TransferFunction1D::colorMap ( const ColorMap& colorMap )
{
  _colorMap = colorMap;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color map.
//
///////////////////////////////////////////////////////////////////////////////

TransferFunction1D::ColorMap TransferFunction1D::colorMap() const
{
  return _colorMap;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the opacity map.
//
///////////////////////////////////////////////////////////////////////////////

void TransferFunction1D::opacityMap ( const OpacityMap& opacityMap )
{
  _opacityMap = opacityMap;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the opacity map.
//
///////////////////////////////////////////////////////////////////////////////

TransferFunction1D::OpacityMap TransferFunction1D::opacityMap() const
{
  return _opacityMap;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color mode.
//
///////////////////////////////////////////////////////////////////////////////

void TransferFunction1D::colorMode ( ColorMode mode )
{
  _colorMode = mode;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color mode.
//
///////////////////////////////////////////////////////////////////////////////

TransferFunction1D::ColorMode TransferFunction1D::colorMode() const
{
  return _colorMode;
}
