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


#ifndef _NR_FILE_STREAM_H_
#define _NR_FILE_STREAM_H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "IStream.h"
#include "Resource.h"

namespace nrEngine{

	//! Derived class from IStream to provide streaming from files
	/**
	* FileStream is derived class from IStream and IResource. It provides
	* the functionality of streaming the files from the disk. We use the
	* std::ifstream class to do this work.
	*
	* This class is a simple file class which provide the user only
	* with nessary methods. You should use plugins to extend the functionality
	* for working with compressed, encrypted etc. file archives.
	*
	* Because each file is also a resource, so you can open the files through
	* the resource manager. This will call the appropriate resource loader
	* (in our case file loader) to provide you with the file you requested.
	* The type of such a resource is "File"
	* Plugins can add new types like "ZipFile", "ZipArchive", ...
	*
	* \ingroup vfs
	**/
	class _NRExport FileStream : public IStream, public IResource {
	public:

		/**
		* Constructor of the file stream. The loader should
		* specify all neccessary properties of this object to allow
		* opening of the file.
		**/
		FileStream();

		/**
		* Virtual destructor allows to derive new classes from this interface
		**/
		virtual ~FileStream();


		/**
		 * Open a file as a stream without using of underlying resource system.
		 **/
		virtual Result open (const std::string& fileName);

		//------------------------------------------------------------
		//		IStream Interface
		//------------------------------------------------------------

		/**
		* Return the name of the stream. The name of the stream is the same
		* as the name of the underlying resource used for the file storing.
		**/
		const std::string& getName()	const { return getResourceName(); }

		/**
		* @see IStream::read()
		* @param nmemb Number of element to read in
		* @param buf Buffer to read from
		* @param size Size of one element
		**/
		virtual size_t read(void *buf, size_t size, size_t nmemb);

		/**
		* @copydoc IStream::readDelim()
		**/
		virtual size_t readDelim(void* buf, size_t count, const std::string& delim = std::string("\n"));

		/**
		* @copydoc IStream::tell()
		**/
		virtual size_t tell() const;

		/**
		* @copydoc IStream::eof()
		**/
		virtual bool eof() const;

		/**
		* @copydoc IStream::getData()
		**/
		virtual byte* getData(size_t& count) const;

		/**
		* @copydoc IStream::seek()
		**/
		virtual bool seek(int32 offset, int32 whence = IStream::CURRENT);

		/**
		*  @copydoc IStream::close()
		**/
		virtual void close ();

	private:

		//! The file stream loader should be able to change the data of this object
		friend class FileStreamLoader;

		//! Stream used to read the file
		SharedPtr< std::ifstream > mStream;

		Result unloadResource();
		Result reloadResource(PropertyList* params);
	};


	//! Empty file stream to represent empty files
	/**
	 * This is  file stream which loads an empty file. You will
	 * need this to get ResourceManagment working corectly. Any
	 * access to a resource instantiated from this class have
	 * no effect.
	 *
	 * \ingroup vfs
	 **/
	class _NRExport EmptyFileStream : public FileStream {

	public:
		//! Initialize the empty file
		EmptyFileStream();

		//! Deinitilize the empty file object
		~EmptyFileStream();

		//! Read data from the empty file returns nothing
		size_t read(void *buf, size_t size, size_t nmemb);

		//! Read until we fund a delimeter
		size_t readDelim(void* buf, size_t count, const std::string& delim = "\n");

		//! What is the position of the empty file reading cursor
		size_t tell() const;

		//! Are we on the end of file. For empty files it is always true
		bool eof() const;

		//! Returns no data from the empty file
		byte* getData(size_t& count) const;

		//! Seek or not to seek? That is the question!
		bool seek(int32 offset, int32 whence = IStream::CURRENT);

		//! Close the empty file
		void close ();

	};


}; // end namespace

#endif
