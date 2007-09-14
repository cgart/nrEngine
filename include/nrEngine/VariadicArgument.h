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

#ifndef _NR_VARIADIC_ARGUMENT_H 
#define _NR_VARIADIC_ARGUMENT_H

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"

namespace nrEngine {

	//! VariadicArgument - Class helping you to pass variable number of parameters to a scripting function
	/**
	 * VariadicArgument - This class is usefull if you want to pass variable parameters to
	 * a function of a script. You can surely easy use the calling function,
	 * by providing a vector containing the data, but you have to create the vector
	 * before. Herewith you just write (Class << data1 << data2 ... ) to create
	 * temporary a list of contained data and pass it as an argument
	 *
	 * The variadic argument object could also be used to retrieve a resulting
	 * value from a function. Such a result value would contain more than one
	 * result which can then be casted to any type.
	 * 
	 * \ingroup gp
	**/
	class _NRExport VarArg {
	
		public:
			
			//! Default constructor
			VarArg();

			//! Constructor getting any element
			VarArg(const boost::any& p);

			//! Copyconstructor
			VarArg(const VarArg& v);
			
			//! Fill the list with new element
			VarArg& operator, (const boost::any&);
	
			//! Get access to certain element
			boost::any& operator[](int index);

			//! Get number of elements stored here
			uint32 size() const { return count; }

			//! Stack based function to retrieve the elements form the list
			boost::any pop_front();
			
			//! Stack based function to retrieve the elements form the list
			boost::any pop_back();

			//! Empty the resulting list
			void empty();
			
			//! Push new elements into the argument list
			void push_back(const boost::any&);
			
			//! Get an element of casted to a certain type
			template<class T> T get(int index){
				return cast<T>(_get(index));
			}
			
			//! Cast the element to a certain type
			template<class T> T cast(const boost::any& p){
				// we want to copy to the same type, so do nothing
				//if (p.type() == typeid(T) || typeid(T) == typeid(boost::any))
				//	return (T)p;
					
				// in other cases, do any_cast and hope it works
				return boost::any_cast<T>(p);
			}

			//! Convert the variable argument list into a vector of certain type
			template<class T> void convert(std::vector<T>& v) const {
				v.clear();
				args::const_iterator it = mArgs.begin();
				for (int32 i=0; i < count; it++,i++){
					
					// we want to copy to the same type, so do nothing
					if (it->type() == typeid(T)){
						v.push_back(*it);

					// we want to copy to type "any", so simple conversion
					}else if (typeid(T) == typeid(boost::any)){
						boost::any t = *it;
						v.push_back(t);

					// in other cases, do any_cast and hope it works
					}else{
						v.push_back (boost::any_cast<T>(*it));
					}
					
				}
				//return v;
			}
			 
		private:

			//! Type of the list we store the data in
			typedef std::list<boost::any> args;

			//! here we store our data
			args mArgs;

			//! here we store the number of elements
			int32 count;			
	
			//! Get access to certain element
			boost::any& _get(int index);

	};

};

#endif
