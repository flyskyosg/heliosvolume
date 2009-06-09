
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSGTOOLS_VOLUME_GPU_RAY_CASTING_H__
#define __OSGTOOLS_VOLUME_GPU_RAY_CASTING_H__

#include "OsgVolume/Export.h"
#include "OsgVolume/TransferFunction.h"

#include "OsgTools/Configure/OSG.h"

#include "osg/Image"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Shader"
#include "osg/Uniform"

namespace OsgVolume {

class OSG_VOLUME_EXPORT GPURayCasting : public osg::Geode
{
public:
  /// Typedefs.
  typedef osg::Geode                             BaseClass;
  typedef osg::Geometry                          Geometry;
  typedef osg::ref_ptr < osg::Image >            ImagePtr;
  typedef unsigned int                           TextureUnit;
  typedef std::pair < ImagePtr, TextureUnit >    TexutreInfo;
  typedef OsgVolume::TransferFunction     TransferFunction;

  /// Construction.
  GPURayCasting();
  GPURayCasting( osg::Program * );
  
  static osg::Program*             createProgram();

  /// Get/Set the image.
  osg::Image*                      image ();
  const osg::Image*                image () const;
  void                             image ( osg::Image* image, TextureUnit unit = 0 );

  /// Get/Set the sampling rate.
  float                            samplingRate () const;
  void                             samplingRate ( float rate );

  /// Get/Set the bounding box.
  void                             boundingBox ( const osg::BoundingBox& bb );
  const osg::BoundingBox&          boundingBox () const;

  /// Traverse this node.
  void                             traverse ( osg::NodeVisitor &nv );
  
  /// Get/Set the transfer function as an image.
  void                             transferFunction ( TransferFunction* tf, TextureUnit unit = 1 );
  TransferFunction*                transferFunction () const;

protected:
  virtual ~GPURayCasting();

  void                             _construct();
  static osg::Shader*              _buildVertexShader ();
  static osg::Shader*              _buildFragmentShader ();

private:

  osg::ref_ptr<osg::Program>    _program;
  TexutreInfo                   _volume;
  osg::ref_ptr < Geometry >     _geometry;
  TransferFunction::RefPtr      _transferFunction;
  unsigned int                  _tfTextureUnit;
  osg::BoundingBox              _bb;
  float                         _samplingRate;
  osg::ref_ptr < osg::Uniform > _cameraUniform;
  osg::ref_ptr < osg::Uniform > _minUniform;
  osg::ref_ptr < osg::Uniform > _maxUniform;
  osg::ref_ptr < osg::Uniform > _volumeUniform;
  osg::ref_ptr < osg::Uniform > _tfUniform;
  osg::ref_ptr < osg::Uniform > _rateUniform;
};


}

#endif // __OSGTOOLS_VOLUME_GPU_RAY_CASTING_H__
