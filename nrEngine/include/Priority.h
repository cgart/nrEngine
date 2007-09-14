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


#ifndef _NR_C_PRIORITY__H_
#define _NR_C_PRIORITY__H_

	
//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"


namespace nrEngine{
	
		
	//! Priorities class definition
	/**
	* Priorities definition.
	* Priorities are used very often to determine for example which object should be removed
	* first or which one has to get access to some resources.
	*
	* In our engine we understand under priority numbers some numbers that handles like
	* integers. You have two operations defined on the priorities: addidition and substraction.
	* You can add integers to priority numbers to increase or decrease the meaning of such a priority.
	*
	* The class which use such priorities should know how to handle with them. That means it
	* has to know if the priority numbers are in increasing or decreasing order. Increasing order
	* means that the element with the lowest priority is coming at last and with the highest as first.
	* So to say colloquially, lower priority is better than higher. There can also be another
	* way: decreasing order. Which is converse to increasing order.
	*
	* All default priority numbers are stored in decreasing order. That means the best priority is
	* the priority with the smallest number. So you are welcome to use this definition or just
	* write you classes according to increasing definition.
	*
	* You can also create priority numbers from strings. The given string will first be scanned
	* for known names like "PRIORITY_FIRST" or "NR_PRIORITY_FIRST" and so on. If such known string
	* was found, so the value according to such a string will be assigned. If no such name found,
	* so standard boost's lexical cast will be used to cast for the value. So this can also fail
	* if the given string could not been casted. (e.g. "5bla" -> not castable)
	* \ingroup gp
	**/
	class _NRExport CPriority{
	public:
	
		/**
		* Couple of pririoty default definitions. Can be used to specify a priority of
		* any elements with CPriority-Class
		**/
		typedef enum _PriorityType{

			//! This number represents an immediate priority (better than first)
			IMMEDIATE		= 0,
			
			//! Smallest possible priority number
			FIRST			= 1,
			
			//! Ultry high priority
			ULTRA_HIGH		= 10000,
			
			//! Next smaller priority to the ultra high value
			VERY_HIGH		= 20000,
			
			//! High priority
			HIGH			= 30000,
			
			//! Default priority for all definitions (Center point)
			NORMAL			= 40000,
			
			//! Lower priority according to the normal
			LOW				= 50000,
			
			//! Very low priority in relation to normal value
			VERY_LOW		= 60000,
			
			//! Ultra low priority by comparison to the normal value
			ULTRA_LOW		= 70000,
			
			//! Greatest possible priority number
			LAST			= 0x8FFFFFFE
			
		}PriorityType;
			
		//! Normal constructor
		CPriority():	mPriority(NORMAL){}
		
		//! non-virtual destructor to prevent deriving of classes
		~CPriority()				{}
		
		//! Copy constructor for priorities
		CPriority(const CPriority& p){
			mPriority = p.get();	
		}
		
		//! Create priority object from a number
		CPriority(const int32& n): mPriority(n) {}
			
		/**
		* Create the priority from a string. String can contain either a number or
		* a character array with priority names e.g. "PRIORITY_LAST"
		**/
		CPriority(const std::string& str){
			
			if (str.length() == 0){ mPriority = NORMAL;}
			#define _PRIORITY_(name)	else if (str == #name){ mPriority = name;}
			_PRIORITY_(FIRST)
			_PRIORITY_(ULTRA_HIGH)
			_PRIORITY_(VERY_HIGH)
			_PRIORITY_(HIGH)
			_PRIORITY_(NORMAL)
			_PRIORITY_(LOW)
			_PRIORITY_(VERY_LOW)
			_PRIORITY_(ULTRA_LOW)
			_PRIORITY_(LAST)
			#undef _PRIORITY_
			else{
				mPriority = boost::lexical_cast<uint32>(str);
			}
		}
		
		//! Get the priority as an integer number, Number will be saturated
		operator	int32() const { return u2i(mPriority);}
		
		//! Get the priority number
		operator	uint32() const { return mPriority;}
		
		//! add new value to the priority number
		CPriority&	operator	+= (const CPriority& p)	{mPriority += p.get(); return *this;}
		CPriority&	operator	+= (const uint32& p)		{mPriority += p; return *this;}
		
		//! Add an integer value can produce negative values
		CPriority&	operator	+= (const int32& p)			{mPriority = i2u(u2i(mPriority) + p); return *this;}
		
		//! Substract a value from the priority by appropriate conversion between these types
		CPriority&	operator	-= (const CPriority& p)	{mPriority = i2u(u2i(mPriority) - u2i(p.get())); return *this;}
		CPriority&	operator	-= (const int32& p)			{mPriority = i2u(u2i(mPriority) - p); return *this;}
		CPriority&	operator	-= (const uint32& p)		{mPriority = i2u(u2i(mPriority) - u2i(p)); return *this;}
	
		//! Check the priority eqiuvalence and order to other values
		bool	operator	< (const CPriority& p)  const{return mPriority < p.get();}
		bool	operator	> (const CPriority& p)	const{return mPriority > p.get();}
		bool	operator	==(const CPriority& p)	const{return mPriority == p.get();}
		bool	operator	!=(const CPriority& p)	const{return mPriority != p.get();}
		bool	operator	<=(const CPriority& p)	const{return mPriority <= p.get();}
		bool	operator	>=(const CPriority& p)	const{return mPriority >= p.get();}
	
		//! Check the priority eqiuvalence and order to other integer values
		bool	operator	< (const int32& p)	const{return mPriority < i2u(p);}
		bool	operator	> (const int32& p)	const{return mPriority > i2u(p);}
		bool	operator	==(const int32& p)	const{return mPriority == i2u(p);}
		bool	operator	!=(const int32& p)	const{return mPriority != i2u(p);}
		bool	operator	<=(const int32& p)	const{return mPriority <= i2u(p);}
		bool	operator	>=(const int32& p)	const{return mPriority >= i2u(p);}

		//! Check to PriorityType
		bool	operator	< (PriorityType p)	const{return mPriority < (uint32)p;}
		bool	operator	> (PriorityType p)	const{return mPriority > (uint32)p;}
		bool	operator	==(PriorityType p)	const{return mPriority == (uint32)p;}
		bool	operator	!=(PriorityType p)	const{return mPriority != (uint32)p;}
		bool	operator	<=(PriorityType p)	const{return mPriority <= (uint32)p;}
		bool	operator	>=(PriorityType p)	const{return mPriority >= (uint32)p;}
		
		//! Convert to a string
		operator std::string(){
			if (false){}
			#define _PRIORITY_(name)	else if (mPriority == name){ return std::string(#name);}
			_PRIORITY_(IMMEDIATE)
			_PRIORITY_(FIRST)
			_PRIORITY_(ULTRA_HIGH)
			_PRIORITY_(VERY_HIGH)
			_PRIORITY_(HIGH)
			_PRIORITY_(NORMAL)
			_PRIORITY_(LOW)
			_PRIORITY_(VERY_LOW)
			_PRIORITY_(ULTRA_LOW)
			_PRIORITY_(LAST)
			#undef _PRIORITY_
			else{
				return boost::lexical_cast<std::string>(mPriority);
			}
		}
		
		private:
			//! priority number
			uint32	mPriority;
		
			//! Get the priority number
			const uint32& get()const	{return mPriority;}
		
			//! Saturate an integer value to unsigned integer
			uint32	i2u(const int32& i) const{
				if (i >= 0) return static_cast<uint32>(i);
				else		return 0;
			}
		
			//! Saturate an unsigned integer signed integer
			int32	u2i(const uint32& i) const {
				if (i < 0x8FFFFFFF) return static_cast<uint32>(i);
				else				return 0x8FFFFFFE;
			}

	};
	
	//! Write the priority without C prefix, to define it like normal type. \ingroup gp
	typedef		CPriority		Priority;
	
}; // end namespace

#endif
 
