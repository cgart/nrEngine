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

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include <nrEngine/VariadicArgument.h>
#include <nrEngine/Exception.h>

namespace nrEngine {

	//----------------------------------------------------------------------------------
	VarArg::VarArg() : count(0)
	{

	}

	//----------------------------------------------------------------------------------
	VarArg::VarArg(const boost::any& p) : count(1)
	{
		mArgs.push_back(p);
	}

	//----------------------------------------------------------------------------------
	VarArg::VarArg(const VarArg& v)
	{
		// copy all elemnts form v into this
		this->count = v.count;
		this->mArgs = v.mArgs;
	}

	//----------------------------------------------------------------------------------
	VarArg& VarArg::operator, (const boost::any& p)
	{
		// add new element to the list
		mArgs.push_back(p);
		count ++;
		return *this;
	}

	//----------------------------------------------------------------------------------
	boost::any VarArg::pop_front()
	{
		NR_ASSERT(count > 0);		
		boost::any p = mArgs.front();
		mArgs.pop_front();
		count --;
		return p;
	}

	//----------------------------------------------------------------------------------
	boost::any VarArg::pop_back()
	{
		NR_ASSERT(count > 0);
		boost::any p = mArgs.back();
		mArgs.pop_back();
		count --;
		return p;
	}

	//----------------------------------------------------------------------------------
	void VarArg::push_back(const boost::any& p)
	{
		*this,p;
	}

	//----------------------------------------------------------------------------------
	void VarArg::empty()
	{
		mArgs.empty();
		count = 0;
	}

	//----------------------------------------------------------------------------------
	boost::any& VarArg::_get(int index)
	{
		// check if the index is OK
		NR_ASSERT(index >= 0 && index < count);

		// get the value on this position
		args::iterator it = mArgs.begin();
		for (int k=0; k < index; k++) it++;
		//it = it + index;
		
		return *it;
	}
	
	//----------------------------------------------------------------------------------
	boost::any& VarArg::operator[](int index)
	{
		return _get(index);
	}

};

