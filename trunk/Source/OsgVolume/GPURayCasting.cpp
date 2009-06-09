
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgVolume/GPURayCasting.h"

#include "Usul/Bits/Bits.h"

#include "osg/CullFace"
#include "osg/Texture1D"
#include "osg/Texture3D"

#include "osgUtil/CullVisitor"

#include <sstream>
#include <iostream>

#define BACK_TO_FRONT_COMPOSITE 1

using namespace OsgVolume;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

GPURayCasting::GPURayCasting() : BaseClass (),
  _program ( GPURayCasting::createProgram() ),
  _volume ( 0x0, 0 ),
  _geometry ( new Geometry ),
  _transferFunction ( 0x0 ),
  _bb ( ),
  _samplingRate ( 0.1f ),
  _cameraUniform ( new osg::Uniform ( osg::Uniform::FLOAT_VEC3, "Camera" ) ),
  _minUniform ( new osg::Uniform ( osg::Uniform::FLOAT_VEC3, "bbMin" ) ),
  _maxUniform ( new osg::Uniform ( osg::Uniform::FLOAT_VEC3, "bbMax" ) ),
  _volumeUniform ( new osg::Uniform ( osg::Uniform::INT, "Volume" ) ),
  _tfUniform ( new osg::Uniform ( osg::Uniform::INT, "TransferFunction" ) ),
  _rateUniform ( new osg::Uniform ( osg::Uniform::FLOAT, "SampleRate" ) )
{
  this->_construct();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

GPURayCasting::GPURayCasting( osg::Program * program ) : BaseClass (),
  _program ( program ),
  _volume ( 0x0, 0 ),
  _geometry ( new Geometry ),
  _transferFunction ( 0x0 ),
  _bb(),
  _samplingRate ( 0.1f ),
  _cameraUniform ( new osg::Uniform ( osg::Uniform::FLOAT_VEC3, "Camera" ) ),
  _minUniform ( new osg::Uniform ( osg::Uniform::FLOAT_VEC3, "bbMin" ) ),
  _maxUniform ( new osg::Uniform ( osg::Uniform::FLOAT_VEC3, "bbMax" ) ),
  _volumeUniform ( new osg::Uniform ( osg::Uniform::INT, "Volume" ) ),
  _tfUniform ( new osg::Uniform ( osg::Uniform::INT, "TransferFunction" ) ),
  _rateUniform ( new osg::Uniform ( osg::Uniform::FLOAT, "SampleRate" ) )
{
  this->_construct();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

void GPURayCasting::_construct()
{
  // Get the state set.
  osg::ref_ptr < osg::StateSet > ss  ( this->getOrCreateStateSet() );
  
  // Turn on front face culling
#if BACK_TO_FRONT_COMPOSITE
  ss->setAttributeAndModes ( new osg::CullFace ( osg::CullFace::FRONT ), osg::StateAttribute::ON );
#endif
  ss->setMode ( GL_CULL_FACE, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );
  ss->setMode ( GL_BLEND, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
  
  ss->setRenderBinDetails ( 1000, "RenderBin" );
  
  // Add the planes.
  this->addDrawable ( _geometry.get() );
  
  // Set the internal bounding box.
  this->boundingBox ( osg::BoundingBox ( -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f ) );
  
  ss->setAttributeAndModes( _program.get(), osg::StateAttribute::ON );
  
  // Add the uniforms.
  ss->addUniform ( _cameraUniform.get() );
  ss->addUniform ( _minUniform.get() );
  ss->addUniform ( _maxUniform.get() );
  ss->addUniform ( _volumeUniform.get() );
  ss->addUniform ( _tfUniform.get() );
  ss->addUniform ( _rateUniform.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

GPURayCasting::~GPURayCasting()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the image.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* GPURayCasting::image ()
{
  return _volume.first.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the image.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Image* GPURayCasting::image () const
{
  return _volume.first.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the image.
//
///////////////////////////////////////////////////////////////////////////////

void GPURayCasting::image ( osg::Image* image, TextureUnit unit )
{
  _volume.first = image;
  _volume.second = unit;

  // Set the uniform for the volume.
  _volumeUniform->set ( static_cast < int > ( unit ) );

  // Create the 3D texture.
  osg::ref_ptr < osg::Texture3D > texture3D ( new osg::Texture3D );
  texture3D->setImage( image );    

  texture3D->setFilter( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR );
  texture3D->setFilter( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR );

#if 0
  texture3D->setWrap( osg::Texture3D::WRAP_R, osg::Texture3D::CLAMP );
  texture3D->setWrap( osg::Texture3D::WRAP_S, osg::Texture3D::CLAMP );
  texture3D->setWrap( osg::Texture3D::WRAP_T, osg::Texture3D::CLAMP );
#else
  texture3D->setWrap( osg::Texture3D::WRAP_R, osg::Texture3D::CLAMP_TO_EDGE );
  texture3D->setWrap( osg::Texture3D::WRAP_S, osg::Texture3D::CLAMP_TO_EDGE );
  texture3D->setWrap( osg::Texture3D::WRAP_T, osg::Texture3D::CLAMP_TO_EDGE );
#endif
  
  if ( 0x0 != image )
  {
    if ( GL_ALPHA == image->getPixelFormat() || GL_LUMINANCE == image->getPixelFormat() )
    {
      texture3D->setInternalFormatMode ( osg::Texture3D::USE_USER_DEFINED_FORMAT );
      texture3D->setInternalFormat ( GL_INTENSITY );
    }
    else
    {
      texture3D->setInternalFormatMode ( osg::Texture3D::USE_IMAGE_DATA_FORMAT );
    }
  }

  // Don't resize.
  texture3D->setResizeNonPowerOfTwoHint( false );

  // Get the state set.
  osg::ref_ptr< osg::StateSet > ss ( this->getOrCreateStateSet() );
  ss->setTextureAttributeAndModes ( unit, texture3D.get(), osg::StateAttribute::ON );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of planes.
//
///////////////////////////////////////////////////////////////////////////////

float GPURayCasting::samplingRate () const
{
  return _samplingRate;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of planes.
//
///////////////////////////////////////////////////////////////////////////////

void GPURayCasting::samplingRate ( float rate )
{
  _rateUniform->set ( rate );
  _samplingRate = rate;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Vertex shaders.
//
///////////////////////////////////////////////////////////////////////////////

osg::Shader* GPURayCasting::_buildVertexShader()
{
  std::ostringstream os;

  os << "uniform vec3 Camera;\n";
  os << "uniform vec3 bbMax;\n";
  os << "uniform vec3 bbMin;\n";
  os << "varying vec3 vertexPos;\n";
  os << "varying vec3 cameraPos;\n";
  os << "void main(void)\n";
  os << "{\n";
  os << "   gl_TexCoord[0].x =  ( ( gl_Vertex.x - bbMin.x ) ) / ( bbMax.x - bbMin.x );\n";
  os << "   gl_TexCoord[0].y =  ( ( gl_Vertex.y - bbMin.y ) ) / ( bbMax.y - bbMin.y );\n";
  os << "   gl_TexCoord[0].z =  ( ( gl_Vertex.z - bbMin.z ) ) / ( bbMax.z - bbMin.z );\n";
  os << "   gl_TexCoord[0].w =  ( gl_Vertex.w + 1.0 ) / 2.0;\n";
  
#if 1
  os << "   vec4 camera = gl_ModelViewMatrixInverse*vec4(0,0,0,1);\n";
  //os << "   vec4 camera = vec4 ( Camera.xyz, 1.0 );\n";
  os << "   cameraPos.x =  ( ( camera.x - bbMin.x ) ) / ( bbMax.x - bbMin.x );\n";
  os << "   cameraPos.y =  ( ( camera.y - bbMin.y ) ) / ( bbMax.y - bbMin.y );\n";
  os << "   cameraPos.z =  ( ( camera.z - bbMin.z ) ) / ( bbMax.z - bbMin.z );\n";
  //os << "   cameraPos.w =  ( cameraPos.w + 1.0 ) / 2.0;\n";
  
  os << "   vertexPos = gl_TexCoord[0].xyz;\n";
#else
  //os << "   vec4 camera = gl_ModelViewMatrixInverse*vec4(0,0,0,1);\n";
  os << "   cameraPos = Camera.xyz;\n";
  os << "   vertexPos = gl_Vertex.xyz;\n";
#endif
  
  os << "   gl_Position = ftransform();\n";
  //os << "   gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;\n";
  os << "}\n";

  osg::ref_ptr< osg::Shader > vertexShader ( new osg::Shader( osg::Shader::VERTEX ) );
  vertexShader->setShaderSource ( os.str() );
  return vertexShader.release();
}


namespace Detail
{
  inline std::string buildFragmentShader() 
  {
    std::ostringstream os;

    os << " uniform sampler3D Volume;\n"
       <<  "uniform sampler1D TransferFunction;\n"
       <<  "uniform vec3 bbMin;\n"
       <<  "uniform vec3 bbMax;\n"
       <<  "uniform float SampleRate;\n"
       << " varying vec3 vertexPos;\n"
       << " varying vec3 cameraPos;\n"
       <<  "void main(void)\n"
       <<  "{\n"
       <<  "  vec4 value;\n"
       <<  "  float scalar;\n"

       // Initialize answer fragment.
       <<  "   vec4 dst = vec4 ( 0.0, 0.0, 0.0, 0.0 );\n"

      // Find the entry position in the volume.
      <<  " vec3 position = gl_TexCoord[0].xyz;\n"

      // Compute the ray direction.
#if BACK_TO_FRONT_COMPOSITE
    <<  "   vec3 direction = normalize ( cameraPos.xyz - gl_TexCoord[0].xyz );\n"
#else
      <<  "   vec3 direction = normalize ( gl_TexCoord[0].xyz - cameraPos.xyz );\n"
#endif

    // Ray traversal loop.  Should probably make the 200 an input.
       <<  "   for ( int i = 0; i < 2048; i++ )\n"
       <<  "   {\n"

    //  Look up the scalar value.
       <<  "   value = texture3D( Volume, position );\n"
       <<  "   scalar = value.a;\n"
    
    // Apply the transfer function.
       <<  "   vec4 src = vec4( texture1D( TransferFunction, scalar ) );\n"
   
#if BACK_TO_FRONT_COMPOSITE
    <<  "   dst.xyz = dst.xyz * ( 1.0 - src.a ) + src.xyz * src.a;\n"
    <<  "   dst.a += src.a;\n"
#else
    // Front to back compositing.
    <<  "   dst = ( ( 1.0 - dst.a ) * src ) + dst;\n"
   // <<  "  if ( dst.a > .95 ) break;\n "
#endif
       // Advance the ray position
       <<  "   position = position + direction * SampleRate;\n"

       // Ray termination.
       //<<  "   vec3 temp1 = sign ( position - bbMin );\n"
       //<<  "   vec3 temp2 = sign ( bbMax - position );\n"
#if 1
    <<  "   vec3 temp1 = sign ( position - vec3 ( 0.0, 0.0, 0.0 ) );\n"
    <<  "   vec3 temp2 = sign ( vec3 ( 1.0, 1.0, 1.0 ) - position );\n"
    
       <<  "   float inside = dot ( temp1, temp2 );\n"
    
    // If inside, break.
       <<  "   if ( inside < 3.0 )\n"
       <<  "     break;\n"

#else
       
       <<  "   if ( position.x < 0.0 || position.y < 0.0 || position.z < 0.0 || position.x > 1.0 || position.y > 1.0 || position.z > 1.0 )\n"
       <<  "     break;\n"
#endif
    // End of the for loop.
       <<  "   }\n"

      <<   "  if ( dst.a > 1.0 ) dst.a = 1.0;\n"
    // Return the result.
       <<  "   gl_FragColor = vec4( dst );\n"
       <<  "}\n";

    return os.str();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Fragment shader.
//
///////////////////////////////////////////////////////////////////////////////

osg::Shader* GPURayCasting::_buildFragmentShader()
{
  osg::ref_ptr< osg::Shader > fragmentShader ( new osg::Shader( osg::Shader::FRAGMENT ) );
  fragmentShader->setShaderSource ( Detail::buildFragmentShader() );
  return fragmentShader.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the shaders.
//
///////////////////////////////////////////////////////////////////////////////

osg::Program* GPURayCasting::createProgram()
{
  osg::ref_ptr< osg::Program > program ( new osg::Program ); 

	program->addShader ( GPURayCasting::_buildVertexShader() );
	program->addShader ( GPURayCasting::_buildFragmentShader() );
 
  return program.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

void GPURayCasting::boundingBox ( const osg::BoundingBox& bb )
{
  _bb = bb;
  
  const osg::Vec3 min ( bb._min );
  const osg::Vec3 max ( bb._max );

  // Half lengths
  const float hw ( 0.5f * ( max.x() - min.x() ) );
  const float hh ( 0.5f * ( max.y() - min.y() ) );
  const float hd ( 0.5f * ( max.z() - min.z() ) );
  
  // Vertices and normals.
  osg::ref_ptr<osg::Vec3Array> vertices ( new osg::Vec3Array );
  //osg::ref_ptr<osg::Vec3Array> normals  ( new osg::Vec3Array );
  
  // Front face
  vertices->push_back ( bb.center() + osg::Vec3 ( -hw, -hh,  hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 (  hw, -hh,  hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 (  hw,  hh,  hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 ( -hw,  hh,  hd ) );
  //normals->push_back  ( osg::Vec3 ( 0, 0, 1 ) );
  
  // Back face
  vertices->push_back ( bb.center() + osg::Vec3 (  hw, -hh, -hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 ( -hw, -hh, -hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 ( -hw,  hh, -hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 (  hw,  hh, -hd ) );
  //normals->push_back  ( osg::Vec3 ( 0, 0, -1 ) );
  
  // Top face
  vertices->push_back ( bb.center() + osg::Vec3 (  hw,  hh,  hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 (  hw,  hh, -hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 ( -hw,  hh, -hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 ( -hw,  hh,  hd ) );
  //normals->push_back  ( osg::Vec3 ( 0, 1, 0 ) );
  
  // Bottom face
  vertices->push_back ( bb.center() + osg::Vec3 (  hw, -hh,  hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 ( -hw, -hh,  hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 ( -hw, -hh, -hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 (  hw, -hh, -hd ) );
  //normals->push_back  ( osg::Vec3 ( 0, -1, 0 ) );
  
  // Left face
  vertices->push_back ( bb.center() + osg::Vec3 ( -hw,  hh,  hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 ( -hw,  hh, -hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 ( -hw, -hh, -hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 ( -hw, -hh,  hd ) );
  //normals->push_back  ( osg::Vec3 ( -1, 0, 0 ) );
  
  // Right face
  vertices->push_back ( bb.center() + osg::Vec3 (  hw,  hh, -hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 (  hw,  hh,  hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 (  hw, -hh,  hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 (  hw, -hh, -hd ) );
  //normals->push_back  ( osg::Vec3 ( 1, 0, 0 ) );
  
  // Make a new geometry and add the vertices and normals.
  osg::ref_ptr<osg::Geometry> geometry ( new osg::Geometry );
  geometry->setVertexArray ( vertices.get() );
  //geometry->setNormalArray ( normals.get() );
  //geometry->setNormalBinding ( osg::Geometry::BIND_PER_PRIMITIVE );
  geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::QUADS, 0, vertices->size() ) );

  _geometry = geometry;
  
  this->removeDrawables ( 0, this->getNumDrawables() );
  this->addDrawable ( _geometry.get() );

  // Set the min and max values of the bounding box.
  _minUniform->set ( min );
  _maxUniform->set ( max );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

const osg::BoundingBox& GPURayCasting::boundingBox () const
{
  return _bb;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the transfer function image.
//
///////////////////////////////////////////////////////////////////////////////

void GPURayCasting::transferFunction ( TransferFunction* tf, TextureUnit unit )
{
  if ( 0x0 != tf )
  {
    _transferFunction = tf;
    _transferFunction->textureUnit ( unit );

    // Get the state set.
    osg::ref_ptr< osg::StateSet > ss ( this->getOrCreateStateSet() );
    ss->setTextureAttributeAndModes ( unit, _transferFunction->texture(), osg::StateAttribute::ON );

    // Set the uniform value.
    _tfUniform->set ( static_cast < int > ( unit ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the transfer function image.
//
///////////////////////////////////////////////////////////////////////////////

OsgVolume::TransferFunction* GPURayCasting::transferFunction() const
{
  return _transferFunction.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse this node.
//
///////////////////////////////////////////////////////////////////////////////

void GPURayCasting::traverse ( osg::NodeVisitor &nv )
{
  if ( osg::NodeVisitor::CULL_VISITOR == nv.getVisitorType() )
  {
    const osg::Vec3 camera ( nv.getEyePoint() );
    _cameraUniform->set ( camera );
  }

  // Call the base class' one.
  BaseClass::traverse ( nv );
}
