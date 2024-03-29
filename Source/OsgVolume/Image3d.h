
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_IMAGES_IMAGE3D_H__
#define __OSG_TOOLS_IMAGES_IMAGE3D_H__

#include "OsgVolume/Export.h"

namespace Usul { namespace Interfaces { struct IUnknown; } }
namespace osg { template < class T > class ref_ptr; class Image; }

#include <vector>

namespace OsgVolume {

  typedef osg::ref_ptr< osg::Image > ImagePtr;
  typedef std::vector< ImagePtr >    ImageList;

  OSG_VOLUME_EXPORT osg::Image* image3d ( ImageList&, bool ensureProperTextureSize = false, double updateTime = 1000, Usul::Interfaces::IUnknown *caller = 0x0  );

}

#endif // __OSG_TOOLS_IMAGES_THRESHOLD_H__
