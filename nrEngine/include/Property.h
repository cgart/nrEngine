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


#ifndef _NR_PROPERTY_H_
#define _NR_PROPERTY_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include <boost/any.hpp>


namespace nrEngine{

	class PropertyManager;
	
	//! Properties are name value pair with a value of any type
	/**
	 * Properties represents name-value pairs with a name as a string and a
	 * value of any type. For the type of value we use boost::any which is
	 * capable to represent any type of data.
	 * 
	 * \ingroup gp
	**/
	class _NRExport Property{
		public:

			/**
			 * Create an empty property object. No name is specified
			 * and no value
			 **/
			Property();

			/**
			 * Create a new property object.
			 *
			 * @param name Name of the property. Name should be unique
			 * within one property map
			 **/
			Property(const std::string& name);

			/**
			 * Construct a property object with a certain name, but also define
			 * a value directly
			 **/
			Property (const std::string& name, const boost::any& value);

			/**
			 * Construct the property from another one
			 **/
			Property(const Property&);
			
			/**
			 * Get the name of the property
			 **/
			NR_FORCEINLINE const std::string& getName() const { return mName; }
			
			/**
			 * Get the fullname of the property. Fullname contains
			 * whole group path and the name (i.e. "group.name" ).
			 * Only PropertyManager can change full name, because he does
			 * only know to which group this property belongs. If property
			 * is not handled by manager, so fullname is equal name
			 **/
			NR_FORCEINLINE const std::string& getFullName() const { return mFullName; }

			/**
			 * Get value of the property
			 **/
			NR_FORCEINLINE const boost::any& getValue() const { return mValue; }
			NR_FORCEINLINE boost::any& getValue() { return mValue; }


			/**
			 * Check if the stored value has this type
			 **/
			//NR_FORCEINLINE bool is
			
			/**
			 * Assign new value to the property
			 **/
			Property& operator=(const boost::any& );

			/**
			 * Compare two properties. Two properties are the same
			 * if their names and type of the values are the same.
			 *
			 * NOTE: We can not compare the values itself because boost::any
			 * library can not be casted here to any type. If you wish to have
			 * more usefull comparing function, that refer to compare<T>()
			 **/
			bool operator==(const Property& );

			bool operator!=(const Property& p) { return !(operator==(p)); }

			
			/**
			 * Specific compare of the values with a certain type.
			 * This will compare the properties like operator==()
			 * do and also compare the values by converting them
			 * to the certain type and looking if they are the same.
			 **/
			template<class T> bool compare(const Property& p)
			{
				if (*this != p) return false;
				return boost::any_cast<T>(mValue) == boost::any_cast<T>(p.getValue());
			}

			/**
			 * Get the value by casting to a certain type
			 **/
			template<class T> T get(){
				return boost::any_cast<T>(mValue);
			}

			/**
			 * Set user data. We use extra user data information if casting
			 * will fail for the value types.
			 **/
			NR_FORCEINLINE void setUserData(void* data) { mUserData = data; }

			/**
			 * Get user data from the property
			 **/
			NR_FORCEINLINE void* getUserData() { return mUserData; }

			/**
			 * Does our property contains any user data. Just check if the user
			 * data pointer equals to NULL. If yes, so we do not get any user data.
			 **/
			NR_FORCEINLINE bool hasUserData() { return mUserData != NULL; }
			
		private:

			//! Name of the property
			std::string mName;

			//! Full name of property (group.name)
			std::string mFullName;
			
			//! Value holding by the property
			boost::any	mValue;

			//! Userdata to be stored
			void*	mUserData;

			//! Manager is allowed to change internal structure
			friend class PropertyManager;

			//! Copy only the data from given property
			NR_FORCEINLINE void copyDataOnly(const Property& p)
			{
				mValue = p.mValue;
				mUserData = p.mUserData;
			}
	};

	//! Property list to hold property objects
	/**
	 * PropertyList is a derived class from std::list holding
	 * objects of type Property. We expand the functionality by
	 * adding some nicer operators and functions, so the list
	 * can be constructed in concatenated way.
	 * \ingroup gp
	 **/
	class _NRExport PropertyList : public std::list<Property> {
		public:

			/**
			 * Add a new element into the property list
			 **/
			PropertyList& operator, (const Property& p);

			/**
			 * Pop an element from the front and return it back
			 **/
			PropertyList& operator, (Property& p);

			/**
			 * Check if a certain property exists
			 **/
			bool exists(const std::string& name) const;

			/**
			 * Get a property value by the given name.
			 * The behaviour is the same as in std::map. If no
			 * such property exists, so new one with such a name will be created
			 **/
			Property& operator[](const std::string& name);
			const Property& operator[](const std::string& name) const ;

			/**
			 * Get property from the list
			 **/
			//Property& get(const std::string& name);
			
	};
	
	
}; // end namespace

#endif
