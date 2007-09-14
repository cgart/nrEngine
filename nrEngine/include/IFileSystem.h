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


#ifndef _NR_I_FILESYSTEM_H_
#define _NR_I_FILESYSTEM_H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "Property.h"
#include "FileStream.h"

namespace nrEngine{

	//! File system module interface that are managed by file system manager
	/**
	 * Each file system has it's own handling routines and access controls.
	 * So this class provide an interface for abstract using of such file systems.
	 * Modules derived from this interface can be registered by the file system manager
	 * so they are used to access files.
	 *
	 * Each file system does have a type. A type is unique for each kind of fs.
	 * The name of the type is used to allow user to specify directly the fs he
	 * want to use to access the files. So calling "file:/.." force a local file
	 * system to be used and "http:/..." will access the files through htpp-interface.
	 * Accessing the files without any fs specified force the manager to look
	 * for the file under each registered file system.
	 *
	 * User can also specify parameters to the file system by accessing the files.
	 * The parameters are separated through ':' and are given in the file name.
	 * You can also specify parameters by setting them directly. The parameters
	 * are always states. i.e. if parameter 1 is set to the value A, so this
	 * value will be used until it is changed. Parameters specified in the filename
	 * are local parameters used only for the current access and they overload
	 * the global parameters of the file system.
	 * 
	 * \ingroup vfs
	**/
	class _NRExport IFileSystem{
		public:
	
			//! Initalize the file system
			IFileSystem();
	
			//! Release used memory and force all files to close
			virtual ~IFileSystem();

			/**
			 * Each file has more info about it than only a name.
			 * So we store file info in a structure.
			 **/
			struct FileInfo{
				//! The file system to which one this file belongs
				IFileSystem* fsParent;

				//! full name of the file
				std::string name;

				//! virtual path of the file separated by '/' and ending with '/'
				std::string path;

				//! real file path, in the file system specific manner
				std::string realPath;
				
				//! size of the file
				size_t size;
				
			};
			
			//! Vector containing file info information
			typedef std::vector<FileInfo> FileInfoList;
			typedef SharedPtr<FileInfoList> FileInfoListPtr;

			
			/**
			 * Check whenever such a file exists in that file system.
			 * @param fileName
			 **/
			virtual bool exists(const std::string& fileName) = 0;

			/**
			 * Returns true if this filesystem is case sensitive by matching the filenames
			 **/
			virtual bool isCaseSensitive() const = 0;

			/**
			 * List all files in the archive. You get the file info list containing
			 * information about each file in the archive
			 *
			 * @param recursive if true so search for the files recursively
			 **/
			virtual FileInfoListPtr listFiles(bool recursive = true) = 0;
			
			/**
			 * Find a file by the given pattern. Wildcard '*' is allowed.
			 *
			 * @param pattern filename pattern including wildcards
			 * @param recursive if true so search recursively
			 **/
			virtual FileInfoListPtr findFiles(const std::string& pattern, bool recursive = true) = 0;

			/**
			 * Initialize the file system.
			 **/
			virtual Result initialize() = 0;

			/**
			 * Release the file system and close all opened files.
			 **/
			virtual Result deinitialize() = 0;

			/**
			 * Open a file by the given filename. The method returns a smart pointer
			 * on the file stream. So as soon as the pointer is not used anymore the file will
			 * be closed automaticaly behaving to the file stream.
			 *
			 * @param filename Name of the file (without parameters)
			 * @param params Local parameters for this access
			 **/
			virtual SharedPtr<FileStream> open(const std::string& filename, PropertyList* params = NULL) = 0;

			/**
			 * File system type. This type is used to allow users to access directly
			 * to the files on this filesystem.
			 **/
			virtual const std::string& getType() { return mType; }

			/**
			 * Set global parameters for this file system
			 * @param param Parameter number
			 * @param value Value of the parameter
			 **/
			virtual Result set(uint32 param, const std::string& value);

			/**
			 * This method allows to setup the parameters by their names.
			 * The file system modules should specifiy somehow the parameter
			 * names which can be used to setup them
			 *
			 * @param name Name of the paramter (i.e. by ftp access "username")
			 * @param value Value of the parameter
			 **/
			virtual Result set(const std::string& name, const std::string& value);
			
		protected:

			//! Unique type name for this file system
			std::string mType;

			//! Map containing the parameters and their values
			PropertyList	mParameter;

	};

};

#endif
