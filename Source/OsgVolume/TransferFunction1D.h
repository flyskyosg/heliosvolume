
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_VOLUME_TRANSFER_FUNCTION_1D_H__
#define __OSG_TOOLS_VOLUME_TRANSFER_FUNCTION_1D_H__

#include "OsgVolume/Export.h"
#include "OsgVolume/TransferFunction.h"

#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"

#include "osg/Image"
#include "osg/Texture"

#include <vector>

namespace OsgVolume {


class OSG_VOLUME_EXPORT TransferFunction1D : public OsgVolume::TransferFunction
{
public:
  typedef Usul::Math::Vec3f RGB;
  typedef std::map<unsigned int,RGB> ColorMap;
  typedef std::map<unsigned int,RGB::value_type> OpacityMap;
  
  USUL_DECLARE_REF_POINTERS ( TransferFunction1D );

  enum ColorMode
  {
    COLOR_MODE_RGB = 0,
    COLOR_MODE_HSV
  };

  /// Construction.
  TransferFunction1D();

  /// Get the number of dimensions for this transfer function.
  virtual unsigned int   dimensions() const;
  
  /// Get the texture.
  virtual osg::Texture*  texture();

  /// Get/Set the mininium value.
  double                 minimium() const;
  void                   minimium ( double );

  /// Get/Set the maximium value.
  double                 maximium() const;
  void                   maximium ( double );
  
  void                   calculateColors();
  
  /// Set the color.
  void                   color ( unsigned int index, const RGB& color );
  
  /// Set/get the color map.
  void                   colorMap ( const ColorMap& colorMap );
  ColorMap               colorMap() const;
  
  /// Set/get the color mode.
  void                   colorMode ( ColorMode mode );
  ColorMode              colorMode() const;
  
  /// Set the opacity.
  void                   opacity ( unsigned int index, RGB::value_type opacity );
  
  /// Set/get the opacity map.
  void                   opacityMap ( const OpacityMap& opacityMap );
  OpacityMap             opacityMap() const;

protected:
  
  virtual ~TransferFunction1D();

  void                   _init();
  
  RGB                    _interpolate ( double u, const RGB& color0, const RGB& color1 ) const;
  
private:
  
  typedef OsgVolume::TransferFunction BaseClass;
  typedef Usul::Math::Vec4f   Color;
  typedef std::vector<Color>  Colors;
  
  double _minimium;
  double _maximium;
  Colors _colors;
  ColorMap _colorMap;
  OpacityMap _opacityMap;
  ColorMode _colorMode;
};


}

#endif // __OSG_TOOLS_VOLUME_TRANSFER_FUNCTION_1D_H__
