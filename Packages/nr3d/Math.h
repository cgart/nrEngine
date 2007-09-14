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
#ifndef _NR__MATH_H
#define _NR__MATH_H

/*!
 * \defgroup math Mathematics
 *
 * Very simple and very useful math library. Has support for a lot of operations
 * on vectors and matricies. Vector names are choosen so to be equivalent to names used
 * by GLSL (OpenGL shading language)
 */

namespace nrEngine {
 
	namespace nr3D {
		/*--------------------------------------------------------------------------
		* Provided classes
		*------------------------------------------------------------------------*/
		struct vec2;
		struct vec3;
		struct vec4;
		struct mat3;
		struct mat4;
		struct quat;
		
		/*--------------------------------------------------------------------------
		* Rename classes, so you can choose names you like
		*------------------------------------------------------------------------*/
		typedef vec2 float2;
		typedef vec3 float3;
		typedef vec4 float4;
		typedef mat3 float3x3;
		typedef mat4 float4x4;
		
		
		/*--------------------------------------------------------------------------
		* Provided definitions
		*------------------------------------------------------------------------*/
		#ifndef DEG2RAD
		#define DEG2RAD (PI / 180.0f)
		#endif
		
		#ifndef RAD2DEG
		#define RAD2DEG (180.0f / PI)
		#endif
		
		#ifndef TODEG
		#define TODEG(x)    ((x * 180.0) / M_PI)
		#endif
		
		#ifndef TORAD
		#define TORAD(x)    ((x / 180.0) * M_PI)
		#endif
		
		/*--------------------------------------------------------------------------
		* Provided constants
		*------------------------------------------------------------------------*/
		static const float EPSILON =  1e-6f;
		static const float PI  = 3.14159265358979323846f;
	};
};

/*--------------------------------------------------------------------------
* Includes
*------------------------------------------------------------------------*/
#include <math.h>
#include "Mathlib.h"


#endif
