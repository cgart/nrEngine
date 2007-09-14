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


		//------------------------------------------------------------------------
		Geometry::Geometry()
		{

		}

		//------------------------------------------------------------------------
		Geometry::Geometry(const Geometry& g)
		{

		}

		//------------------------------------------------------------------------
		Geometry::~Geometry()
		{

		}

		//------------------------------------------------------------------------
		void Geometry::setVertexArray(const std::vector<vec3>& data)
		{
			mVertexArra = data;
		}
		
		//------------------------------------------------------------------------
		void Geometry::setColorArray(const std::vector<vec4>& data)
		{
			mColorArray = data;
		}
		
		//------------------------------------------------------------------------
		void Geometry::setNormalArray(const std::vector<vec3>& data)
		{
			mNormalArray = data;
		}
			
		//------------------------------------------------------------------------
		void Geometry::setTexCoordArray(uint32 unit, const std::vector<vec3>& data)
		{
			mTexCoordArray[unit] = data;
		}

		//------------------------------------------------------------------------
		void Geometry::setVertexAttribArray(uint32 unit, const std::vector<vec3>& data)
		{
			mVertexAttrArray[unit] = data;
		}

		
	};
};
