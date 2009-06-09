
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface to get and set transfer functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_VOLUME_TRANSFER_FUNCTION_1D_LIST_H_
#define _OSG_VOLUME_TRANSFER_FUNCTION_1D_LIST_H_

#include "Usul/Interfaces/IUnknown.h"

#include "OsgVolume/TransferFunction1D.h"

namespace OsgVolume {


struct ITransferFunction1DList : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITransferFunction1DList );

  /// Id for this interface.
  enum { IID = 2297209053u };
  
  /// Typedef.
  typedef OsgVolume::TransferFunction1D::RefPtr TransferFunction1DPtr;

  /// Add a transfer function.
  virtual void addTransferFunction1D ( TransferFunction1DPtr ) = 0;
  
  /// Get the i'th transfer function.
  virtual TransferFunction1DPtr getTransferFunction1D ( unsigned int i ) const = 0;
  
  /// Get the number of 1D transfer functions.
  virtual unsigned int getNumberOfTransferFunctions1D() const = 0;
  
  /// Set/get the active transfer function.
  virtual void setActiveTransferFunction ( unsigned int index ) = 0;
  virtual unsigned int getActiveTransferFunction() const = 0;
};


} // namespace OsgVolume


#endif // _OSG_VOLUME_TRANSFER_FUNCTION_1D_LIST_H_
