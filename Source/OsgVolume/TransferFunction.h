
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

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include "osg/Image"
#include "osg/Texture"

namespace OsgVolume {


class OSG_VOLUME_EXPORT TransferFunction : public Usul::Base::Referenced
{
public:
  typedef Usul::Base::Referenced BaseClass;

  USUL_DECLARE_REF_POINTERS ( TransferFunction );

  TransferFunction ();

  /// Get the image
  osg::Image*            image() const;

  /// Get the texture.
  virtual osg::Texture*  texture() = 0;

  /// Get/Set the texture unit.
  void                   textureUnit ( unsigned int );
  unsigned int           textureUnit() const;

  /// Mark as dirty.
  void                   dirty();

  /// Get the number of dimensions.
  virtual unsigned int   dimensions() const = 0;

protected:

  virtual ~TransferFunction();

  osg::ref_ptr<osg::Image> _image;
  unsigned int _textureUnit;
};


}

#endif // __OSG_TOOLS_VOLUME_TRANSFER_FUNCTION_H__
