
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for flash files
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FLASH_MODEL_DOCUMENT_H_
#define _FLASH_MODEL_DOCUMENT_H_

#include "Timestep.h"
#include "IFlashDocument.h"

#include "Usul/Documents/Document.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Jobs/Manager.h"

#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/ITimeVaryingData.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Interfaces/IMenuAdd.h"

#include "Serialize/XML/Macros.h"

#include "OsgVolume/GPURayCasting.h"
#include "OsgVolume/Texture3DVolume.h"
#include "OsgVolume/TransferFunction1D.h"

#include "osg/BoundingBox"
#include "osg/Group"

#include <string>
#include <vector>
#include <list>

// Uncomment this line to use GPU ray casting.  There are still some issues to work out.
#define USE_RAY_CASTING 0

class FlashDocument : public Usul::Documents::Document,
                      public Usul::Interfaces::IBuildScene,
                      public Usul::Interfaces::ITimeVaryingData,
                      public Usul::Interfaces::IUpdateListener,
                      public Usul::Interfaces::IMenuAdd,
                      public Flash::IFlashDocument
{
public:

  /// Useful typedefs.
  typedef Usul::Documents::Document            BaseClass;
  typedef OsgVolume::TransferFunction   TransferFunction;
  typedef OsgVolume::TransferFunction1D TransferFunction1D;
  typedef TransferFunction1D::RefPtr    TransferFunctionPtr;
  typedef std::vector<TransferFunctionPtr>     TransferFunctions;
  
#if USE_RAY_CASTING
  typedef OsgVolume::GPURayCasting Volume;
#else
  typedef OsgVolume::Texture3DVolume Volume;
#endif

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( FlashDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  FlashDocument();
  
  /// Build the scene.
  virtual osg::Node *         buildScene ( const BaseClass::Options &options, Unknown *caller = 0x0 );

  /// Return true if this document can do it.
  virtual bool                canExport ( const std::string &file ) const;
  virtual bool                canInsert ( const std::string &file ) const;
  virtual bool                canOpen   ( const std::string &file ) const;
  virtual bool                canSave  ( const std::string &file ) const;

  /// Clear any existing data.
  virtual void                clear ( Unknown *caller = 0x0 );
  
  /// Clear the cache.
  void                        clearCache();
  
  /// Set/get the dataset name.
  void                        dataSet ( const std::string& );
  std::string                 dataSet() const;

  /// Set/get the value multiply dataset name.
  void                        vDataSet ( const std::string& );
  std::string                 vDataSet() const;
  
  // Deserialize.
  virtual void                deserialize ( const XmlTree::Node &node );
  
  /// Get/Set the dirty flag.
  void                        dirty ( bool b );
  bool                        dirty () const;

  /// Set/get the draw bounding box flag.
  void                        drawBBox ( bool b );
  bool                        isDrawBBox() const;
  
  /// Set/get the draw points flag.
  void                        drawPoints ( bool b );
  bool                        isDrawPoints() const;
  
  /// Set/get the draw volume flag.
  void                        drawVolume ( bool b );
  bool                        isDrawVolume() const;

  /// Usul::Interfaces::ITimeVaryingData
  virtual void                setCurrentTimeStep ( unsigned int current );
  virtual unsigned int        getCurrentTimeStep () const;
  
  virtual unsigned int        getNumberOfTimeSteps () const;
  
  /// Get the filters that correspond to what this document can read and write.
  virtual Filters             filtersOpen()   const;
  virtual Filters             filtersSave()   const;
  virtual Filters             filtersInsert() const;
  virtual Filters             filtersExport() const;
  
  /// Set/get the minimum value to show.
  virtual void                minimumSet ( double );
  virtual double              minimumGet() const;

  /// Set/get the maximum value to show.
  virtual void                maximumSet ( double );
  virtual double              maximumGet() const;
  
  /// Read the document.
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );
  
  /// Is the i'th timestep loaded?
  bool                        hasTimestep ( unsigned int i ) const;
  
  /// Load the i'th timestep.
  Timestep::RefPtr            loadTimestep ( unsigned int i, bool cache );

  /// Set the transfer function.
  void                        transferFunction ( unsigned int i );
  bool                        isTransferFunction ( unsigned int i ) const;
  
  /// Write the document to given file name.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0  ) const;
  
  /// Usul::Interfaces::IUpdateListener
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );

  /// Function Code ///

  /// get/set the function type
  virtual void                functionType( int type );
  virtual int                 functionType();
  bool                        isFunctionType( int type );

  /// get/set the scalar value
  virtual void                scalar( double scalar );
  virtual double              scalar();



protected:

  /// Do not copy.
  FlashDocument ( const FlashDocument & );
  FlashDocument &operator = ( const FlashDocument & );

  /// Use reference counting.
  virtual ~FlashDocument();
  
  /// Build the scene.
  void                        _buildScene ( Usul::Interfaces::IUnknown* caller );
  osg::Node*                  _buildVolume ( const Timestep& timestep, osg::Image* image, unsigned int numPlanes, const osg::BoundingBox& bb, TransferFunction::RefPtr tf );
  osg::Node*                  _buildLegend ( double minimum, double maximum, TransferFunction::RefPtr tf, Usul::Interfaces::IUnknown* caller ) const;

  /// Build the default transfer functions.
  void                        _buildDefaultTransferFunctions();
  
  /// Sort the files before reading.
  virtual bool                _sortFilesBeforeInserting() const;

  /// Add to the menu (IMenuAdd).
  virtual void                menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller = 0x0 );

private:
  
  typedef std::vector<std::string> Filenames;
  typedef std::map <unsigned int, Timestep::RefPtr>             Timesteps;
  
  Filenames _filenames;
  double _scale;
  unsigned int _currentTimestep;
  std::string _dataSet;
  std::string _valueMultiplier;
  double _minimum;
  double _maximum;
  osg::ref_ptr < osg::Group > _root;
  bool _dirty;
  bool _drawBBox;
  bool _drawPoints;
  bool _drawVolume;
  
  unsigned int _currentTransferFunction;
  TransferFunctions _transferFunctions;
  Timesteps _timesteps;

  // timesteps to contain the second value to multiply to the initial
  Timesteps _vTimeSteps;

  osg::ref_ptr<osg::Program> _program;
  
   // Function variables
  double  _scalar;
  int     _functionType;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_CLASS_NAME ( FlashDocument );
  SERIALIZE_XML_SERIALIZE_FUNCTION;
  SERIALIZE_XML_ADD_MEMBER_FUNCTION;
};


#endif // _FLASH_MODEL_DOCUMENT_H_
