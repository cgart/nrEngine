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
 ***************************************************************************/

#ifndef __NR_PREREQUISITIES_H_
#define __NR_PREREQUISITIES_H_


//------------------------------------------------------------------------------
//	First get all information about the used platform
//------------------------------------------------------------------------------
#include "Platform.h"

//------------------------------------------------------------------------------
//	Standard library includings and definitions
//------------------------------------------------------------------------------
#include "StdHeaders.h"

//------------------------------------------------------------------------------
//	Now follows all forward declarations and type definitions
//------------------------------------------------------------------------------
namespace nrEngine{

	//------------------------------------------------------------------------------
	//	Version Information
	//  MAJOR.MINOR.PATH  - for example  0.3.1
	//------------------------------------------------------------------------------
	#define NR_VERSION_MAJOR 0
	#define NR_VERSION_MINOR 0
	#define NR_VERSION_PATCH 8
	#define NR_VERSION_NAME "Alores"
	#define NR_DEFAULT_EVENT_CHANNEL "nrEngine_default_channel"
	
	//------------------------------------------------------------------------------
	//	Version Information unsigned integer
	//  We have 8 bit for each component of the version part.
	//  The integer is build in the way to be ably to test it against each self.
	//  Lesser number means we have got one of the version parts smaller than
	//  another one. 
	//------------------------------------------------------------------------------
	const unsigned int nrEngineVersion = 	NR_VERSION_MAJOR * 256 * 256 +
											NR_VERSION_MINOR * 256 +
											NR_VERSION_PATCH;
	
	//------------------------------------------------------------------------------
	//	Some macros that makes the life easier
	//------------------------------------------------------------------------------	
	#define NR_SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
	#define NR_SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
	
	#define NR_ENGINE_PROFILING
	
	//------------------------------------------------------------------------------
	//	Basic-type definitions used in the engine
	//------------------------------------------------------------------------------
	typedef unsigned char 	BYTE;
	typedef unsigned char 	byte;
	typedef unsigned char 	uchar;
	
	typedef char			int8;
	typedef unsigned char	uint8;
	
	typedef signed short	int16;
	typedef	unsigned short	uint16;
	
	typedef signed int		int32;
	typedef unsigned int	uint32;
	
	typedef float			float32;
	typedef double			float64;
	
	#if NR_PLATFORM == NR_PLATFORM_WIN32
		typedef   signed __int64  int64;
		typedef unsigned __int64 uint64;
	#else
		typedef   signed long long  int64;
		typedef unsigned long long uint64;
	#endif
	
	//----------------------------------------------------------------------------------
	// Forward declarations of used classes.
	//----------------------------------------------------------------------------------
	/*class 										CPriority;
	template<class T, class value_parser> class CDator;
	template<class T, class value_parser> class CDatorList;
	class 										CStringDator;
	template<class T> class 					CValueParser;
	*/
	
	class										Clock;
	class 										Engine;
	class 										ExceptionManager;
	class 										Exception;
	class 										ITask;
	class										ITimeObserver;
	class										Profiler;
	class										Profile;
	class										TimeSource;
		
	class										EmptyPlugin;
	class										PluginLoader;
	class										Plugin;
	
	class 										IResourcePtr;
	class 										IResource;
	class										ResourceManager;
	class 										ResourceHolder;
	template<class ResType> class 				ResourcePtr;
	class 										IResourceLoader;
	
	class 										Kernel;
	class 										Log;
	//class 										NameValuePairs;
	class										Timer;	

	class										IStream;

	class										IFileSystem;
	class										FileSystem;

	class										ScriptEngine;
	class										IScript;
	class										Script;
	class 										ScriptLoader;

	class										EventManager;
	class										EventChannel;
	class										EventActor;
	class										Event;
	class										EventFactory;
	
	class										VarArg;
	class										Property;
	class 										PropertyManager;
	
}; // end namespace


//----------------------------------------------------------------------------------
// Some includes that are essential for engine's work
//----------------------------------------------------------------------------------
#include "Result.h"
#include "KeySym.h"
#include "StdHelpers.h"
#include "SmartPtr.h"
#include "GetTime.h"
#include "VariadicArgument.h"

#endif
