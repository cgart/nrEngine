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

#ifndef __NR3D_GEOMETRY_H_
#define __NR3D_GEOMETRY_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include <nrEngine/nrEngine.h>
#include "nr3D.h"

namespace nrEngine {

	namespace nr3D {
		
		//! Geometry class represents the geometry in our 3d world
		/**
		 * A geometry object does contain mesh information. This are information
		 * like vertices, normals, tangents, colours, texture coordinates and
		 * other things which can be passed to graphics pipeline.
		 *
		 * Actually geometry should also be a resource object controlled by resource
		 * management system. However we decided to use Meshes as resources. So that
		 * each mesh containing the geometry is controlled by resource management.
		 * This could prevent strange artifacts if a mesh is depending somehow on geometry
		 * data (i.e. for animation or collision detection). So we do not get a part
		 * of a mesh unloaded while another is still in the memory.
		 *
		 * \ingroup nr3d
		**/
		class _NRPackageExport Geometry{

			public:

				/**
				 * Create an empty geometry object.
				 **/
				Geometry();
				 
				/**
				* Create geometry by copiing it from another one
				**/
				Geometry(const Geometry& g);

				/**
				* Remove used memory, so geometry data is not used anymore
				**/
				virtual ~Geometry();

				/**
				 * Check whenever the geometry object does contain any data.
				 * @return false if no data available
				 **/
				//bool isEmpty() const;

				/**
				 * Add new primitive array containing primitives to this geometry
				 * object. The data will be copied, so you have then to release the old one.
				 *
				 * @param arr Smart array containing primitives
				 **/
				 //void addPrimitives();

				 /**
				  * Set vertex array containing all verticies for this geometry mesh
				  * The array will be copied into internal data structures. So you
				  * have to care about what will happen with original data.
				  *
				  * @param array Array containing the vertices data
				  **/
				void setVertexArray(const std::vector<vec3>& data);

				/**
				 * Same as setVertexArray() but for color information on vertices
				 **/
				void setColorArray(const std::vector<vec4>& data);

				/**
				 * Set normals for verticies
				 **/
				void setNormalArray(const std::vector<vec3>& data);

				/**
				 * Set texture coordinates array. For each texture, we have 3 coordinates
				 **/
				void setTexCoordArray(uint32 unit, const std::vector<vec3>& data);

				/**
				 * Vertex attributes
				**/
				void setVertexAttribArray(uint32 unit, const std::vector<vec3>& data);
					
			protected:

				//! Store verticies of the geometry here
				std::vector<vec3>	mVertexArray;

				//! Store normals here
				std::vector<vec3>	mNormalArray;

				//! Store color information here
				std::vector<vec3>	mColorArray;

				//! Store texture coordinates here
				std::map<uint32, std::vector<vec3> >	mTexCoordArray;

				//! Store here vertex attributes
				std::map<uint32, std::vector<vec3> >	mVertexAttribArray;
						
				/**
				* Cause the geometry data to empty all buffers.
				* After applying this method you have no more access
				* to any data.
				**/
				//void emptyBuffers();
				
	};
};

#endif

