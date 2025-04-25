/**
  \ingroup GLRenderExternalInterfaces
  \file    GLRenderDecl.h
  \brief   This GLRenderDecl.h file controls DLL export/import under windows
  \author  guether
  \date    2023-01-26

  Copyright:
  2023 Fraunhofer Institute for Telecommunications, Heinrich-Hertz-Institut (HHI)
  The copyright of this software source code is the property of HHI.
  This software may be used and/or copied only with the written permission
  of HHI and in accordance with the terms and conditions stipulated
  in the agreement/contract under which the software has been supplied.
  The software distributed under this license is distributed on an "AS IS" basis,
  WITHOUT WARRANTY OF ANY KIND, either expressed or implied.


  This file shall not be used for any implementation stuff.
  This is the file included by users of the lib GLRender
 */

/// \cond NEVER_DOC
#pragma once
/// \endcond

#if defined( _WIN32 )
#  if defined( GLRENDER_DYN_LINK )
#    if defined( GLRENDER_SOURCE )
#      define GLRENDER_DECL __declspec( dllexport )
#    else
#      define GLRENDER_DECL __declspec( dllimport )
#    endif  // GLRENDER_SOURCE
#  endif    // GLRENDER_DYN_LINK
#endif      // _WIN32

#if ! defined( GLRENDER_DECL )
#  define GLRENDER_DECL
#endif
