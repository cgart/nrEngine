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

#include "nr3D.h"

namespace nrEngine {
	namespace nr3D{

		SharedPtr<nr3D> nr3D::sSingleton;

		//------------------------------------------------------------------------
		nr3D* nr3D::instance()
		{
			if (!valid()){
				sSingleton.reset(new nr3D());
			}
			return sSingleton.get();
		}

		//------------------------------------------------------------------------
		void nr3D::release()
		{
			if (valid())
			{
				sSingleton.reset();
			}
		}

		//--------------------------------------------------------------------------
		bool nr3D::valid()
		{
			return sSingleton != NULL;
		}

		//----------------------------------------------------------------------
		nr3D::nr3D()
		{
			// initialize default variables
			mName = "nr3D-Package";
			mFullName = "3D-Engine package for nrEngine";
			NR_Log(Log::LOG_PLUGIN, "%s: %s",mName.c_str(), mFullName.c_str());

			// check whenever engine is valid
			if (!Engine::valid())
			{
				NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "%s: Engine must be initialized before!", mName.c_str());
				NR_EXCEPT(ENGINE_ERROR, "Engine must be initialized before creating a nr3D", "nr3D::nr3D()");
			}

			// initialize glfw task
		}

		//----------------------------------------------------------------------
		nr3D::~nr3D()
		{
		
		}

		//----------------------------------------------------------------------
		const std::string& nr3D::getName()
		{
			return mName;
		}

		//----------------------------------------------------------------------
		const std::string& nr3D::getFullName()
		{
			return mFullName;
		}
	};
};
