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


#ifndef _NR_PROPERTY_MANAGERH_
#define _NR_PROPERTY_MANAGERH_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "Property.h"
#include "Log.h"
#include "Exception.h"

namespace nrEngine{
	
	//! Property manager does hold properties for global access
	/**
	 * Property manager class is used to access global properties. In this
	 * way your application can store some values of any type in the manager
	 * by a certain name. The value can then be accessed from another place.
	 * Propertties are grouped in groups, so group names must be unique. If no
	 * group name is specified, so default group is used. Property must have
	 * unique names in one group.
	 *
	 * \ingroup gp
	**/
	class _NRExport PropertyManager{
		public:

			/**
			 * Create Property manager object and initialiye default data
			 **/
			PropertyManager();

			/**
			 * Release used memory and remove all properties from the memory.
			 **/
			~PropertyManager();

			/**
			 * Create new property group.
			 * If such group already exists, so nothing happens.
			 *
			 * @param name Unique name of a property group
			 **/
			void createGroup(const std::string& name);

			/**
			 * Get number of properties in a certain group
			 *
			 * @param [group] Unique name of a property group
			 **/
			//int32 getPropertyCount(const std::string& group = std::string());

			/**
			 * Get property list of properties of certain group.
			 * The list is const, so no changes are allowed.
			 * If no such group exists, so empty list will be returned
			 * @param [group] Name of the group
			 **/
			const PropertyList& getPropertyList(const std::string& group = std::string());

			/**
			 * Get property.
			 * @param name Name of the property
			 * @param [group] Unique name of the group
			 **/
			Property& getProperty(const std::string& name, const std::string& group = std::string());
			Property& getPropertyByFullName(const std::string& fullname);
			
			/**
			* Get the property directly. @see PropertyManager::getProperty()
			**/
			NR_FORCEINLINE Property& operator()(const std::string& name, const std::string& group = std::string()) {return getProperty(name, group);}
			
			/**
			 * Add/add new property to a group.
			 * If no such group exists, so group will be created.
			 * If such property already exists, so it will be overwritten
			 *
			 * @param property Property which to add
			 * @param name Name of the property
			 * @param group Unique group name.
			 **/
			void set (const Property& property, const std::string& name, const std::string& group = std::string());

			/**
			 * Directly setup new property with new value
			 * @see PropertyManager::set()
			 * @param value Any value to set to the property (value must be convertible)
			 * @param name Name of the property
			 * @param group Unique goup name
			 **/
			void set(const boost::any& value, const std::string& name, const std::string& group = std::string());

			/**
			 * Set new value based on fullname
			 * @see IPropertyManager::set()
			 * @param value Valeu to be set for the property
			 * @param fullname Fullname of the property (i.e. "group.name")
			 **/
			void setByFullName(const boost::any& value, const std::string& fullname);
			void setByFullName(const Property& property,const std::string& name);

			/**
			 * Get the value directly from the database.
			 * The method is templated, so you get the value converted to given type.
			 * If conversion is not possible, so error occurs. To prevent errors, check
			 * the type before access.
			 *
			 * @param name Name of the property
			 * @param [group] Unique group name of the property
			 **/
			template<typename T>
			NR_FORCEINLINE T get(const std::string& name, const std::string& group = std::string());
			
			/**
			 * @see get()
			 **/
			template<typename T>
			NR_FORCEINLINE T getByFullName(const std::string& name);

		private:
						
			//! This describey the type of our property map
			typedef std::map<std::string, PropertyList> PropertyMap;

			//! Here we store our properties
			PropertyMap mPropertyMap;
	};
	

	//----------------------------------------------------------------------------------
	template<typename T>
	NR_FORCEINLINE T PropertyManager::get(const std::string& name, const std::string& group)
	{	
		try{
			// get property value
			boost::any& v = getProperty(name, group).getValue();
			
			// check if property is empty
			if (v.empty())
			{
				// give warning, that now a undefined value will be returned
				NR_Log(Log::LOG_ENGINE | Log::LOG_CONSOLE, Log::LL_WARNING, "PropertyManager::get() - You are retrieving undefined value from '%s.%s' property", group.c_str(), name.c_str());
				return T();
			}

			// cast the property value and return it back
			return boost::any_cast<T>(v);
			
		}catch(const boost::bad_any_cast &){
			NR_EXCEPT(PROPERTY_WRONG_TYPE, "Cannot cast property to the given type", "PropertyManager::get()");
		}
	}

	//----------------------------------------------------------------------------------
	template<typename T>
	NR_FORCEINLINE T PropertyManager::getByFullName(const std::string& fullname)
	{	
		try{
			// get property value
			boost::any& v = getPropertyByFullName(fullname).getValue();
			
			// check if property is empty
			if (v.empty())
			{
				// give warning, that now a undefined value will be returned
				NR_Log(Log::LOG_ENGINE | Log::LOG_CONSOLE, Log::LL_WARNING, "PropertyManager::get() - You are retrieving undefined value from '%s' property", fullname.c_str());
				return T();
			}

			// cast the property value and return it back
			return boost::any_cast<T>(v);
			
		}catch(const boost::bad_any_cast &){
			NR_EXCEPT(PROPERTY_WRONG_TYPE, "Cannot cast property to the given type", "PropertyManager::get()");
		}
	}

}; // end namespace

#endif
