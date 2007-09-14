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


#ifndef _NR_FILESYSTEM_H_
#define _NR_FILESYSTEM_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "IFileSystem.h"

/**
* @defgroup vfs Engine's filesystem
*
* NOT IMPLEMENTED AT NOW !!!!
*
* Files are storing the data needed for apropriate working of the engine and the
* underlying application. Files could be found on the disk but also on external media
* like ftp servers or similar things. You can either implement your own file
* loading routines to load such external files or you use the engine's file system
* which has the ability to be fully transparent to the user.
*
* Our filesystem contains a manger which does manage all available virtual file systems.
* Each such virtual file system (vfs) can be either a local fs working on local files
* or an http implementation of this one working on files in the internet. Each such
* vfs is registerd by the manager, so it knows how to handle file requests.
*
* Files are requested through it's name. So just ask a manger to open a file with name A.ext
* and it will ask each vfs if it does know such a file and ask them to open it. So for you
* as user there is no difference if the file is found on the local disk or on a ftp server.
* However transfering file data from external fs can be slow.
*
* If you know where the file is, you could also specify the file position directly, by
* specifying the file location similar to "file:/A.ext" for local files and "ftp:/A.ext"
* for external files. So the manager will ask that vfs thats type is specified for
* appropriate file. This mechanism is very similar to unix one, so you should be familar with it.
*
* There can also be some parameters to the certain vfs specified in the file path. For
* example you want to open a file on a certain ftp server and not the default one.
* So calling something like this "ftp:server.domain:user:password:/A.ext" will force the
* ftp-vfs to login on that server and download the file.
*
* We definy that all parameters given to the certain vfs systems should be separated by :
* Which parameters can be given you can find in the documentation of each fs module.
* There is no matter how much parameters you set, the filename must be at last.
* i.e. "filesystem:param1:param2:..:paramn:/path/to/file/file.name"
* The given filename is always in context to the filesystem. For example for local
* files you specify relative path, for ftp-servers it is always absolute.
* Filename-conventions are always according to the module you use. Our standard module
* to load local files, does use simple convention that all file names are relative to the
* application and the path is separated by slash. Conversion to windows filenames is
* done by the modules automaticaly, so "file:d:/A.ext" refers to a file on disk d in windows.
*
*
* In our engine you have a defualt filesystem which you can you
* for your own puprose. The filesystem we are using is based on std::ifstream
* so it is fully compatible to all system where std::ifstream works.
* The filesystem is built in the way that it can easy be replaced through
* plugins which can expand the functionality of the system.
*
* Each file in the system is also a resource which is managed by resource
* manager. We do not want to separate the filesystem from resoure system
* (as in many engine's does) because so we get the ability of simple user interface
* and the possibility of controlling the files as resources (e.g. if they are
* used in streams and can be un/loaded at runtime).
*
* If files are handled as resources you can also still able to manage your
* memory without forgetting of counting of the size which each file has.
* For example unstreamed file which is loaded completely in the memory does
* use a lot of free memory. The resource according to the file for example an image
* is working on that file. In a naive way you will get only the size of the image
* counted and the file size wuold be not mentioned in the resource system. So
* you didn't realy realize how much memory does your resource need. With the system
* we are using you know that the complete size of the resource is the image object
* and the file opened by it.
*
* Also the possibility of using files like resource does centralize the managment
* of all opperations according to loading/unloading/removing. So you can also use
* scripts and just load resource "File" which is handled like normal resource but
* provides reading to the file.
*
* Our filesystem has also the possibility to add an archive support to the filesystem.
* This could be done by appropriate plugin.
*
* Actually our current implementation of the engine allows us to add the filesystem
* functionality to the engine by plugins. But because we must be able to read simple
* files like config file which is loaded before all plugins, we must find the way
* how to do this. So using of the filesystem as plugin is not possible.
* But you are still able to add more supported file types (filesystems) through
* plugins after this system is running.
*
* @see ResourceManager, IResource, IStream
**/

namespace nrEngine{

 //! File system manager class handles all filesystem transparent to the user
 /**
  * Engine's does use a file system manger to manage all available file systems.
  * This fs is called virtual file system or VFS. You as user of the engine does
  * use this vfs to get the data from files. The system is working transparent, so
  * you does not notice where the files are readed from.
  *
  * Each certain module should register by this manager so the user get access to the
  * file sstem provided by the module.
  *
  * \ingroup vfs
  **/
 class _NRExport FileSystemManager {
		public:

			//! Initilize the virtual file system so it can be now accessed
			FileSystemManager();

			//! Release used memory and force all modules to unload
			~FileSystemManager();

			/**
			 * Register a new file system by the manager.
			 * The file systems are used to access the files
			 * 
			 * @param fs Smart pointer on the file system object
			 **/
			Result addFilesystem(SharedPtr<IFileSystem> fs);

			
			/**
			 * Remove a certain filesystem from the list. This will unload
			 * the filesystem smart pointer, so if it not used anymore the filesystem
			 * will be removed from the memory. This should also close all opened files.
			 *
			 * @param name Name of the file system
			 **/
			Result removeFilesystem(const std::string& name);

			
			/**
			 * Get the file system by it's type name.
			 *
			 * @param name Name of the file system
			 **/
			SharedPtr<IFileSystem> getFilesystem(const std::string& name);

			
			/**
			 **/


		private:
			//! Map of registered file systems with their type names
			typedef std::map< std::string, SharedPtr<IFileSystem> > FilesystemMap;

			FilesystemMap mFilesystems;

			
			
	};
								 
};

#endif
