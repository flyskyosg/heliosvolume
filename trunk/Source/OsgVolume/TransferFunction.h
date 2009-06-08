
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_VOLUME_TRANSFER_FUNCTION_H__
#define __OSG_TOOLS_VOLUME_TRANSFER_FUNCTION_H__

#include "OsgVolume/Export.h"

#include "osg/Image"
#include "osg/Texture"

namespace OsgVolume {


class OSG_VOLUME_EXPORT TransferFunction : public osg::Referenced
{
public:
  typedef osg::Referenced BaseClass;
  typedef osg::ref_ptr<TransferFunction> RefPtr;

  TransferFunction ();

  /// Get the image
  osg::Image*            image ();
  const osg::Image*      image () const;

  /// Get the texture.
  osg::Texture*          texture ();
  const osg::Texture*    texture () const;

  /// Get/Set the texture unit.
  void                   textureUnit ( unsigned int );
  unsigned int           textureUnit () const;

  /// Mark as dirty.
  void                   dirty ();

  /// Get the number of dimensions.
  virtual unsigned int   dimensions () const = 0;

protected:
  virtual ~TransferFunction ();

  osg::ref_ptr < osg::Image > _image;
  osg::ref_ptr < osg::Texture > _texture;
  unsigned int _textureUnit;
};


}

#endif // __OSG_TOOLS_VOLUME_TRANSFER_FUNCTION_H__
