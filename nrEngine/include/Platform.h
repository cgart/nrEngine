/***************************************************************************
 *                                                                         *
 *   (c) Art Tevs, MPI Informatik Saarbruecken                             *
 *       mailto: <tevs@mpi-sb.mpg.de>                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 * Parts of this file was get from OGRE (Object-oriented Graphics          * 
 * Rendering Engine). see http://www.ogre3d.org                            *
 * Copyright (c) 2000-2005 The OGRE Team                                   *
 **************************************************************************/
#ifndef __NR_PLATFORM_H_
#define __NR_PLATFORM_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* Initial platform/compiler-related stuff to set.
*/
#define NR_PLATFORM_WIN32 1
#define NR_PLATFORM_LINUX 2
#define NR_PLATFORM_APPLE 3
#define NR_PLATFORM_NETWARE 4
#define NR_PLATFORM_OS2		5

#define NR_COMPILER_MSVC 1
#define NR_COMPILER_GNUC 2
#define NR_COMPILER_BORL 3

#define NR_ENDIAN_LITTLE 1
#define NR_ENDIAN_BIG 2

#define NR_ARCHITECTURE_32 1
#define NR_ARCHITECTURE_64 2

/* Finds the compiler type and version.
*/
#if defined( _MSC_VER )
#   define NR_COMPILER NR_COMPILER_MSVC
#   define NR_COMP_VER _MSC_VER

#elif defined( __GNUC__ )
#   define NR_COMPILER NR_COMPILER_GNUC
#   define NR_COMP_VER (((__GNUC__)*100)+__GNUC_MINOR__)

#elif defined( __BORLANDC__ )
#   define NR_COMPILER NR_COMPILER_BORL
#   define NR_COMP_VER __BCPLUSPLUS__

#else
#   pragma warning "No known compiler. Behaivor is not defined :-)"

#endif

/* See if we can use __forceinline or if we need to use __inline instead */
#if NR_COMPILER == NR_COMPILER_MSVC 
#   if NR_COMP_VER >= 1200
#       define NR_FORCEINLINE __forceinline
#   endif
#else
#   define NR_FORCEINLINE __inline
#endif


/* Finds the current platform */
#if defined( __WIN32__ ) || defined( _WIN32 )
#   define NR_PLATFORM NR_PLATFORM_WIN32
#elif defined( __APPLE_CC__)
#   define NR_PLATFORM NR_PLATFORM_APPLE
#else
#   define NR_PLATFORM NR_PLATFORM_LINUX
#endif


/* Find the arch type */
#if defined(__x86_64__)
#   define NR_ARCH_TYPE NR_ARCHITECTURE_64
#else
#   define NR_ARCH_TYPE NR_ARCHITECTURE_32
#endif


//----------------------------------------------------------------------------
// Windows Settings
#if NR_PLATFORM == NR_PLATFORM_WIN32
#include <windows.h>
// If we're not including this from a client build, specify that the stuff
// should get exported. Otherwise, import it.
#if defined( NONCLIENT_BUILD ) || defined ( BUILDING_DLL ) || defined (BUILD_DLL)
#  	define _NRExport __declspec( dllexport )
#elif defined (BUILDING_PLUGIN) || defined (BUILD_PLUGIN) || defined (CLIENT_BUILD)
#  	define _NRExport __declspec( dllimport )
#else
#   define _NRExport
#endif

// Win32 compilers use _DEBUG for specifying debug builds.
#   ifdef _DEBUG
#       define NR_DEBUG_MODE 1
#   else
#       define NR_DEBUG_MODE 0
#   endif

// Check for mingw compilers
#if defined( __MINGW32__ )
    #define GCC_3_1
    #define EXT_HASH
#else
    #define snprintf _snprintf
    #define vsnprintf _vsnprintf
#endif

#endif
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// Linux/Apple Settings
#if NR_PLATFORM == NR_PLATFORM_LINUX || NR_PLATFORM == NR_PLATFORM_APPLE

// Linux compilers don't have symbol import/export directives.
#   define _NRExport

// A quick define to overcome different names for the same function
#   define stricmp strcasecmp

// Unlike the Win32 compilers, Linux compilers seem to use DEBUG for when
// specifying a debug build.
#   ifdef DEBUG
#       define NR_DEBUG_MODE 1
#   else
#       define NR_DEBUG_MODE 0
#   endif

#endif

//For apple, we always have a custom config.h file
#if NR_PLATFORM == NR_PLATFORM_APPLE
#    include "config.h"
//SDL_main must be included in the file that contains
//the application's main() function.
// #ifndef NONCLIENT_BUILD
// #   include <SDL/SDL_main.h>
// #endif

#endif
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// Endian Settings
// check for BIG_ENDIAN config flag, set NR_ENDIAN correctly
#ifdef CONFIG_BIG_ENDIAN
#    define NR_ENDIAN NR_ENDIAN_BIG
#else
#    define NR_ENDIAN NR_ENDIAN_LITTLE
#endif


#endif
