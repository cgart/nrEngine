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

#ifndef __NR_STD_HEADERS_H_
#define __NR_STD_HEADERS_H_

#ifdef __BORLANDC__
    #define __STD_ALGORITHM
#endif

// default system headers
#include <ctime>
#include <cstdarg>

// STL Library
#include <vector>
#include <map>
#include <string>
#include <set>
#include <list>
#include <deque>
#include <queue>

#if 0
// Hash Map if it is exists
#ifdef EXT_HASH
#	include <ext/hash_map>
#	include <ext/hash_set>
#else
#	include <hash_set>
#	include <hash_map>
#endif

#ifndef HashMap
	#ifdef GCC_3_1
	#   define HashMap __gnu_cxx::hash_map
	#else
	#   if NR_COMPILER == COMPILER_MSVC
	#       if NR_COMP_VER > 1300 && !defined(_STLP_MSVC)
	#           define HashMap stdext::hash_map
	#       else
	#           define HashMap std::hash_map
	#       endif
	#   else
	#       define HashMap std::hash_map
	#   endif
	#endif
#endif
#endif	

// Streaming
#include <fstream>
#include <iostream>
#include <sstream>

// boost stuff
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_array.hpp>
#include <boost/function.hpp>
#include <boost/any.hpp>


// load default libraries for linux using
#if NR_PLATFORM == NR_PLATFORM_LINUX
extern "C" {
#   include <unistd.h>
#   include <dlfcn.h>
};
#endif

// some default headers for mac systems
#if NR_PLATFORM == NR_PLATFORM_APPLE
extern "C" {
#   include <unistd.h>
#   include <sys/param.h>
#   include <CoreFoundation/CoreFoundation.h>
};
#endif

#endif
