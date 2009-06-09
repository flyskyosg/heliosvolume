
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __FLASH_MODEL_TYPE_WRAPPER_H__
#define __FLASH_MODEL_TYPE_WRAPPER_H__

#include "Usul/Math/Vector4.h"
#include "Usul/Factory/RegisterCreator.h"

#include "Serialize/Xml/DataMemberMap.h"
#include "Serialize/Xml/ValueMapMember.h"

#include "OsgVolume/TransferFunction1D.h"

using OsgVolume::TransferFunction1D;

USUL_FACTORY_REGISTER_CREATOR ( TransferFunction1D );

#include <vector>

///////////////////////////////////////////////////////////////////////////////
//
//  Specialization for std::vector<Usul::Math::Vec4uc>.
//
///////////////////////////////////////////////////////////////////////////////

namespace Serialize {
namespace XML {

  
template < > struct TypeWrapper <TransferFunction1D::RefPtr>
{
  typedef TypeWrapper < std::vector<TransferFunction1D::RefPtr> > ThisType;
  typedef TransferFunction1D::ColorMap ColorMap;
  typedef TransferFunction1D::OpacityMap OpacityMap;
  
  static void addAtribute ( const std::string &name, const std::string &value, XmlTree::Node &node )
  {
    node.attributes()[name] = value;
  }
  static const char *className ( const TransferFunction1D::RefPtr &value )
  {
    return "TransferFunction1D";
  }
  static TransferFunction1D::RefPtr create ( const std::string &typeName )
  {
    return new TransferFunction1D;
  }
  static void deserialize ( const XmlTree::Node &node, TransferFunction1D::RefPtr  &value )
  {
    ColorMap colorMap;
    OpacityMap opacityMap;
    unsigned int mode;
    
    Serialize::XML::DataMemberMap data;
    
    data.addMember ( "color_map", colorMap );
    data.addMember ( "opacities_map", opacityMap );
    data.addMember ( "color_mode", mode );
    
    data.deserialize ( node );
    
    value->colorMap ( colorMap );
    value->opacityMap ( opacityMap );
    value->colorMode ( static_cast<TransferFunction1D::ColorMode> ( mode ) );
  }
  static void getAttribute ( const std::string &name, const XmlTree::Node &node, std::string &value )
  {
    XmlTree::Node::Attributes::const_iterator i = node.attributes().find ( name );
    value.assign ( ( node.attributes().end() == i ) ? "" : i->second );
  }
  static bool isValid ( const TransferFunction1D::RefPtr &value )
  {
    return value.valid();
  }
  static void set ( const std::string &s, TransferFunction1D::RefPtr &value )
  {
  }
  static void serialize ( const TransferFunction1D::RefPtr &value, XmlTree::Node &node )
  {
    ColorMap colorMap ( value->colorMap() );
    OpacityMap opacityMap ( value->opacityMap() );
    unsigned int mode ( value->colorMode() );
    
    Serialize::XML::DataMemberMap data;
    
    data.addMember ( "color_map", colorMap );
    data.addMember ( "opacities_map", opacityMap );
    data.addMember ( "color_mode", mode );
    
    data.serialize ( node );
  }
};
  
  
}
}

#endif // __FLASH_MODEL_TYPE_WRAPPER_H__
