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

#ifndef __NR3D_PACKAGE_H_
#define __NR3D_PACKAGE_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include <nrEngine/nrEngine.h>

namespace nrEngine {

	namespace nr3D {
		
		//! Main class which is a root class for 3D-Engine part of nrEngine
		/**
		* This is the main class of nr3D-Package for nrEngine. nr3D is a 3D engine
		* based on the nrEngine core package providing the user with a powerfull
		* 3d engine.
		*
		* This is a main class for the nr3D package. It declared as singleton. You have
		* to create this root part, to get functionality of 3D-package for nrEngine.
		**/
		class _NRPackageExport nr3D : public nrEngine::Package{

			public:

				/**
				* Default constructor used, to create instance and
				* to initialize all underlying systems.
				*
				* NOTE: You have to instantiate a class after initialization
				* 		of the engine, otherwise exception will be thrown
				**/
				nr3D();

				/**
				* Release used memory.
				**/
				~nr3D();

				/**
				 * @see nrEngine::Binding::getName()
				 **/
				const std::string& getName();

				/**
				 * @see nrEngine::Binding::getFullName()
				 **/
				const std::string& getFullName();

				/**
				 * Access to the singleton object
				 **/
				static nr3D* instance();
	
				/**
				 * Release the singleton object
				 **/
				static void release();
				 
				 /**
				 * Check whenever the engine'S singleton was created before
				 */
				 static bool valid();

			private:

				//! Singleton to hold the binding
				static SharedPtr<nr3D> sSingleton;

				//! Name of the binding
				std::string mName;

				//! Full name
				std::string mFullName;

		};

	};
};

#endif

