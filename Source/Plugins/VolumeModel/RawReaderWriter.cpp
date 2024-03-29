
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "VolumeModel/RawReaderWriter.h"
#include "VolumeModel/VolumeDocument.h"

#include "Usul/Convert/Vector3.h"
#include "Usul/Convert/Vector4.h"
#include "Usul/File/Path.h"
#include "Usul/Strings/Case.h"
#include "Usul/Scope/CurrentDirectory.h"
#include "Usul/Registry/Convert.h"

#include "XmlTree/Document.h"
#include "XmlTree/XercesLife.h"

#include "OsgVolume/TransferFunction1D.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( RawReaderWriter, RawReaderWriter::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RawReaderWriter::RawReaderWriter() : BaseClass (),
_filename(),
_size()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RawReaderWriter::~RawReaderWriter()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file and add it to existing data.
//
///////////////////////////////////////////////////////////////////////////////

void RawReaderWriter::read ( const std::string &name, VolumeDocument &doc, Unknown *caller )
{
  typedef XmlTree::Node::Children Children;

  // Initialize and finalize use of xerces.
  XmlTree::XercesLife life;

  XmlTree::Document::RefPtr document ( new XmlTree::Document );
  document->load ( name );

  // Get the nodes from the document.
  Children file ( document->find ( "file", true ) );
  Children size ( document->find ( "size", true ) );
  Children transferFunctions ( document->find ( "transfer_function", true ) );

  // Make sure we have a filename and size.
  if ( file.size() > 0 && size.size() > 0 )
  {
    _filename = file.front()->value ();
    Usul::Convert::Type < std::string, Usul::Math::Vec3ui >::convert ( size.front()->value(), _size );

    //std::string directory ( Usul::File::directory ( _filename, false ) );
    //Usul::System::CurrentDirectory cwd ( directory );

    // Read the file.
    FILE *fp ( fopen( _filename.c_str(), "rb" ) );

    if ( 0x0 != fp )
    {
	    unsigned int size ( _size[0] * _size[1] * _size[2] );

      osg::ref_ptr < osg::Image > image ( new osg::Image );
      image->allocateImage( _size[0], _size[1], _size[2], GL_LUMINANCE, GL_UNSIGNED_BYTE );

      fread( image->data(), sizeof ( unsigned char ), size, fp );
      fclose( fp );

      doc.image3D ( image.get() );

      double xHalf ( _size[0] / 2.0 );
      double yHalf ( _size[1] / 2.0 );
      double zHalf ( _size[2] / 2.0 );

      doc.boundingBox ( osg::BoundingBox ( -xHalf, -yHalf, -zHalf, xHalf, yHalf, zHalf ) );
    }
  }

  for ( Children::iterator iter = transferFunctions.begin(); iter != transferFunctions.end(); ++iter )
    this->_addTransferFunction ( doc, *(*iter) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a transfer function.
//
///////////////////////////////////////////////////////////////////////////////

void RawReaderWriter::_addTransferFunction ( VolumeDocument& doc, XmlTree::Node& node )
{
  unsigned int size ( 0 );
  std::string name;

  {
    XmlTree::Node::Attributes a ( node.attributes () );
    Usul::Convert::Type < std::string, unsigned int >::convert ( a["size"], size );
    Usul::Convert::Type < std::string, std::string  >::convert ( a["name"], name );
  }

  // Typedefs.
  typedef XmlTree::Node::Children Children;
  typedef OsgVolume::TransferFunction1D::RefPtr TransferFunctionPtr;

  TransferFunctionPtr tf ( new OsgVolume::TransferFunction1D );

  Children children ( node.find ( "element", true ) );
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    unsigned int value ( 0 );
    Usul::Math::Vec4f color;

    XmlTree::Node::Attributes a ( (*iter)->attributes () );
    Usul::Convert::Type < std::string, unsigned int       >::convert ( a["value"], value );
    Usul::Convert::Type < std::string, Usul::Math::Vec4f >::convert ( a["color"], color );

    tf->color ( value, Usul::Math::Vec3f ( color[0] / 255.0, color[1] / 255.0, color[2] / 255.0 ) );
    tf->opacity ( value, color[3] / 255.0 );
  }

  tf->textureUnit ( 1 );

  doc.addTransferFunction ( tf.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void RawReaderWriter::write ( const std::string &filename, const VolumeDocument &doc, Unknown *caller ) const
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void RawReaderWriter::clear ( Usul::Interfaces::IUnknown *caller )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* RawReaderWriter::queryInterface( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case IReaderWriter::IID:
    return static_cast < IReaderWriter * > ( this );
  default:
    return 0x0;
  }
}
