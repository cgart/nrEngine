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

#ifndef __NR_PLUGIN_METHODS_H_
#define __NR_PLUGIN_METHODS_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include <nrEngine.h>

using namespace nrEngine;

/**
 * This class provides the subroutines used externaly by the user application.
 * The methods will be called by its names and given parameters.
 **/
class Methods{

	public:

		Methods();
		~Methods();

		/**
		 * Call a certain method by its name and pass the given parameters through.
		 **/
		void call(const std::string& name, std::vector<Plugin::PlgParam>& params);

		/**
		 * Get all availab
};


#endif

