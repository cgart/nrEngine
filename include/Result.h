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

#ifndef __NR_ENGINE_RESULT_H_
#define __NR_ENGINE_RESULT_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"

namespace nrEngine{

	/*!
	* \defgroup error Exception & Errors
	*
	* In every application as in our engine there will be some exceptions
	* and errors that should be managed. Our engine does use return codes
	* to define if any exception occurs. This is used for performance and
	* coding style reasons. So here you can find the definition adn description
	* of the error codes that could be given back by a function.<br>
	* But sometimes engine can not return an error code back (i.e. if error
	* occurs in a constructor). In this case an exception will be thrown.
	* Each exception is of the Exception - class.
	**/

#define NR_ERR_GROUP(a) 0x01000000 * a

	/**
	* nrEngine's error codes returned back by some functions.<br>
	* You can combine error codes of different groups
	* in binary manner (e.g. BAD_PARAMETERS | OUT_OF_MEMORY).
	* \ingroup error
	**/
	enum  ResultCode {

		//------------------------------------------------------------------------------

		//! Don't worry, no error occurs. That is fine :-)
		OK			 		= 0,

		//! Will be given back if the engine can not detect what kind of error does occurs
		UNKNOWN_ERROR 		= 0xFFFFFFFF,

		//! The parameters given to the function are not valid
		BAD_PARAMETERS		= 1 << 0,

		//! Engine couldn't allocate a block of memory because (prob.) no memory available
		OUT_OF_MEMORY		= 1 << 1,

		//! The dator does not contain valid data
		NOT_VALID_DATOR		= 1 << 2,

		//! Generic time out error 
		TIME_OUT		= 1 << 3,
		
		//------------------------------------------------------------------------------

		//! This is a general "group error" produced by work on the files
		FILE_ERROR 			= NR_ERR_GROUP(2),

		//! If file was not found, so this error will be given back
		FILE_NOT_FOUND 		= FILE_ERROR | (1 << 0),

		//! If file has got errors by reading out of a line
		FILE_ERROR_IN_LINE	= FILE_ERROR | (1 << 1),


		//------------------------------------------------------------------------------

		//! General error of the profile error group
		PROFILE_ERROR		= NR_ERR_GROUP(3),

		//! If the specified profile was not found
		PROFILE_NOT_FOUND	= PROFILE_ERROR | (1 << 0),

		//! If the specified profile is currently not loaded
		PROFILE_NOT_LOADED	= PROFILE_ERROR | (1 << 1),

		//! If the specified profile does not exists
		PROFILE_NOT_EXISTS	= PROFILE_ERROR | (1 << 2),

		//! If the profile you requesting already exists
		PROFILE_ALREADY_EXISTS = PROFILE_ERROR | (1 << 3),


		//------------------------------------------------------------------------------

		//! This is an error defining the virtual file system error group
		VFS_ERROR				= NR_ERR_GROUP(4),

		//! If you try to open the file system and it is already opened
		VFS_ALREADY_OPEN		= VFS_ERROR | (1 << 0),

		//! If the engine couldn't open the virtual file system
		VFS_CANNOT_OPEN			= VFS_ERROR | (1 << 1),

		//! If the vfs subsystem couldn't be closed
		VFS_CANNOT_CLOSE		= VFS_ERROR | (1 << 2),

		//! Do you tried to request a file, but forgott to open the file system?
		VFS_IS_NOT_OPEN			= VFS_ERROR | (1 << 3),

		//! The requested file was not found in the vfs
		VFS_FILE_NOT_FOUND		= VFS_ERROR | (1 << 4),

		//! You can not retrieve data from a file, if it was not opened before
		VFS_FILE_NOT_OPEN		= VFS_ERROR | (1 << 5),

		//! The reading of the file found an EOF signal
		VFS_FILE_END_REACHED	= VFS_ERROR | (1 << 6),

		//! File couldn't be readed for many of reasons
		VFS_FILE_READ_ERROR		= VFS_ERROR | (1 << 7),

		//! The line of a file couldn't be readed
		VFS_LINE_READ_ERROR		= VFS_ERROR | (1 << 8),

		//! Scan methof fails because of wrong readed data or so
		VFS_SCAN_READ_ERROR		= VFS_ERROR | (1 << 9),

		//! Seeking in the file failed, probably the seek position is to wide
		VFS_SEEK_ERROR			= VFS_ERROR | (1 << 10),

		//! No such parameter exists
	 	VFS_NO_PARAMETER		= VFS_ERROR | (1 << 11),


		//------------------------------------------------------------------------------

		//! Any error produced by the settings manager is in this group
		SETTINGS_ERROR			= NR_ERR_GROUP(5),

		//! If you try to register a variable that already registered
		SETTINGS_VAR_ALREADY_REGISTERED = SETTINGS_ERROR | (1 << 1),

		//! The variable you requesting is not registered
		SETTINGS_VAR_NOT_REGISTERED 	= SETTINGS_ERROR | (1 << 2),


		//------------------------------------------------------------------------------
		//! Any error that is thrown in the kernel subsystem is in this group
		KERNEL_ERROR			= NR_ERR_GROUP(6),

		//! There were no task found
		KERNEL_NO_TASK_FOUND 	= KERNEL_ERROR | (1 << 1),

		//! You do not have rights for this operation
		KERNEL_NO_RIGHTS		= KERNEL_ERROR | (1 << 2),

		//! The appropriate task is not ready
		KERNEL_TASK_NOT_READY	= KERNEL_ERROR | (1 << 3),

		//! This error comes if there is a circuit dependencies in the kernel task list
		KERNEL_CIRCULAR_DEPENDENCY = KERNEL_ERROR | (1 << 4),

		//! If tasks are missing, that should be in the pipeline
		KERNEL_TASK_MISSING		= KERNEL_ERROR | (1 << 5),

		//! This error comes if an end of itarating is reached
		KERNEL_END_REACHED		= KERNEL_ERROR | (1 << 6),

		//! This error comes if the given task was already updated in this cycle
		KERNEL_ALREADY_VISITED	= KERNEL_ERROR | (1 << 7),

		//! We are on the leaf in the task dependency tree
		KERNEL_LEAF_TASK	= KERNEL_ERROR | (1 << 8),

		//------------------------------------------------------------------------------
		//! Our clock subsystem has got also it's own error group
		CLOCK_ERROR				= NR_ERR_GROUP(7),

		//! There is no time observer
		CLOCK_OBSERVER_NOT_FOUND = CLOCK_ERROR | (1 << 1),

		//! This observer is already observing the time
		CLOCK_OBSERVER_ALREADY_ADDED = CLOCK_ERROR | (1 << 2),

		//! No time source is currently bounded
		CLOCK_NO_TIME_SOURCE		= CLOCK_ERROR | (1 << 3),


		//------------------------------------------------------------------------------
		//! There are errors produced by the engine's framework subsystem
		FW_ERROR				= NR_ERR_GROUP (8),

		//! THe framework couldn't be initialized
		FW_CANNOT_INITIALIZE	= FW_ERROR | (1 << 1),

		//! The framework is failed to resize
		FW_FAILED_TO_RESIZE		= FW_ERROR | (1 << 2),

		//! The rendering context id you give is not valid
		FW_INVALID_RC_ID		= FW_ERROR | (1 << 3),

		//! The framework is already initialized
		FW_ALREADY_INIT			= FW_ERROR | (1 << 4),

		//! The framework subsystem is not initialized at the moment
		FW_NOT_INITIALIZED		= FW_ERROR | (1 << 5),

		//! The rendering context couldn't setup the pixel format
		FW_RC_CANNOT_SETUP_PIXEL_FORMAT = FW_ERROR | (1 << 6),



		//------------------------------------------------------------------------------
		//! Each error produced by the resource manager is in this group
		RES_ERROR				= NR_ERR_GROUP(9),

		//! For this type of resources there is already a loader
		RES_LOADER_ALREADY_EXISTS 	= RES_ERROR | (1 << 1),

		//! The loader is not registered
		RES_LOADER_NOT_REGISTERED 	= RES_ERROR | (1 << 2),

		//! This resource alreaedy exists in the resource database
		RES_ALREADY_EXISTS		 	= RES_ERROR | (1 << 3),

		//! The file type of the resource is not valid
		RES_BAD_FILETYPE			= RES_ERROR | (1 << 4),

		//! Empty resource couldn't be loaded
		RES_CAN_NOT_LOAD_EMPTY	 	= RES_ERROR | (1 << 5),

		//! The requested resource was not found
		RES_NOT_FOUND				= RES_ERROR | (1 << 6),

		//! There is no loader for such kind of resources
		RES_LOADER_NOT_EXISTS		= RES_ERROR | (1 << 7),

		//! This resource is empty
		RES_IS_EMPTY				= RES_ERROR | (1 << 8),

		//! Such resource group was not found
		RES_GROUP_NOT_FOUND			= RES_ERROR | (1 << 9),

		//! The pointer to the resource is equal to null
		RES_PTR_IS_NULL				= RES_ERROR | (1 << 10),

		//! The resource type of the given resource is not supported
		RES_TYPE_NOT_SUPPORTED		= RES_ERROR | (1 << 11),

		//! We can not lock anymore, because the lock state stack is full
		RES_LOCK_STATE_STACK_IS_FULL= RES_ERROR | (1 << 12),

		//! No empty resource was created before
		RES_NO_EMPTY_RES_FOUND 		= RES_ERROR | (1 << 13),


		//------------------------------------------------------------------------------
		//! This are plugin managment errors
		PLG_ERROR			= NR_ERR_GROUP(10),

		//! Plugin could not be loaded
		PLG_COULD_NOT_LOAD		= PLG_ERROR | (1 << 0),

		//! Plugin library could not been initialized
		PLG_CANNOT_INITIALIZE	= PLG_ERROR | (1 << 1),

		//! Plugin throws an error
		PLG_EXTERNAL_ERROR		= PLG_ERROR | (1 << 2),

		//! Plugin symbol is not defined
		PLG_SYMBOL_NOT_FOUND	= PLG_ERROR | (1 << 3),

		//! Plugin version does not accords to the engine's one
		PLG_WRONG_VERSION		= PLG_ERROR | (1 << 4),

		//! Plugin could not been unloaded properly
		PLG_UNLOAD_ERROR		= PLG_ERROR | (1 << 5),


		//------------------------------------------------------------------------------
		//! Generic script error
		SCRIPT_ERROR = NR_ERR_GROUP(11),

		//! The given script could not be parsed
	 	SCRIPT_PARSE_ERROR = SCRIPT_ERROR | (1 << 0),

		//! The function you want register is already in the database
		SCRIPT_FUNCTION_REGISTERED = SCRIPT_ERROR | (1 << 1),

		//! The function you want access is is not in the database
		SCRIPT_FUNCTION_NOT_REGISTERED = SCRIPT_ERROR | (1 << 2),

		//! This function was not found in the script
		SCRIPT_FUNCTION_NOT_FOUND = SCRIPT_ERROR | (1 << 3),

		//! Wrong parameter value given
		SCRIPT_WRONG_PARAMETER_VALUE = SCRIPT_ERROR | (1 << 4),

		//! Script is already running
		SCRIPT_ALREADY_RUNNING = SCRIPT_ERROR | (1 << 5),

		//------------------------------------------------------------------------------
		//! Generic event management error
		EVENT_ERROR = NR_ERR_GROUP(12),

		//! We already does have this event channel in our database
		EVENT_CHANNEL_EXISTS = EVENT_ERROR | (1 << 0),

		//! There is no channel whith the given name
		EVENT_NO_CHANNEL_FOUND = EVENT_ERROR | (1 << 1),

		//! The actor is already connected to a certain communication channel
		EVENT_ALREADY_CONNECTED = EVENT_ERROR | (1 << 2),

		//! The actor is not connected to a certain communication channel
		EVENT_NOT_CONNECTED = EVENT_ERROR | (1 << 3),

		//! We do not have this event channel in our database
		EVENT_CHANNEL_NOT_EXISTS = EVENT_ERROR | (1 << 4),

		//! Event of the given type could not be casted to another type
		EVENT_COULD_NOT_CAST = EVENT_ERROR | (1 << 5),

		//! A given factory name is already in the database
		EVENT_FACTORY_FOUND = EVENT_ERROR | (1 << 6),

		//! There is no such factory with the given name
		EVENT_FACTORY_NOT_FOUND = EVENT_ERROR | (1 << 7),

		//! Given actor is not valid
		EVENT_NO_VALID_ACTOR = EVENT_ERROR | (1 << 8),

		//------------------------------------------------------------------------------
		//! Generic group for properties errors
		PROPERTY_ERROR = NR_ERR_GROUP(13),

		//! No such property exists
		PROPERTY_NOT_EXISTS = PROPERTY_ERROR | (1 << 0),

		//! Wrong property type, so cannot cast
		PROPERTY_WRONG_TYPE = PROPERTY_ERROR | (1 << 1),

		//------------------------------------------------------------------------------
		//! This are general engine layer errors
		ENGINE_ERROR			= NR_ERR_GROUP(127)


	};


	//! The result code is only an integer in real \ingroup error
	typedef uint32		Result;


#undef NR_ERR_GROUP

}; // end namespace


#endif

