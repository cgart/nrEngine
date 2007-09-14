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


#ifndef _NR_I_STREAM_H_
#define _NR_I_STREAM_H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"

namespace nrEngine{

	//! Stream is an interface for stream objects (files, urls, ...)
	/**
	* IStream represents an interface for every kind of stream objects.
	* Stream objects should be able to pass the data from somewhere else
	* to the requester (e.g. engine). The interface is similar to the
	* std::istream object so you can think it is like a wrapper.
	*
	* Good example for such stream objects could be a file stream. In the
	* standard c++ library you already have got the ifstream which can be
	* reimplemented for this interface.
	*
	*
	* \ingroup gp
	**/
	class _NRExport IStream {
	public:

		//! Start position for the seeking
		enum {

			//! Compute new cursor position from the beginning
			START,

			//! Seek from the current position
			CURRENT,

			//! NEw position is computed from the end of file
			END
		};

		/**
		* Empty default constructor.
		* @param streamBufSize Size of the buffer for this stream. Only buffered streams.
		**/
		IStream(uint32 streamBufSize = STREAM_BUFFER_SIZE);

		/**
		* Constructor allows specify a name for the stream
		* @param name Name of the stream
		* @param streamBufSize Size of the buffer for this stream. Only buffered streams.
		**/
		IStream(const std::string& name, uint32 streamBufSize = STREAM_BUFFER_SIZE);

		/**
		* Virtual destructor allows to derive new classes from this interface
		**/
		virtual ~IStream();

		/**
		* Return the name of the stream
		**/
		virtual const std::string& getName()	{ return mName; }

		/**
		* Streaming operator to stream the data to the variable
		**/
		template<typename T> IStream& operator>>(T& val);

		/**
		* Get the size of the buffer (only for buffered streams)
		**/
		NR_FORCEINLINE int32 getBufferSize() const { return mStreamBufSize; }

		/**
		* Read data from the stream (like read from stdio.h).
		*
		* @param buf Here the readed data will be stored
		* @param count Number of bytes to be readed
		*
		* @return Count fo readed bytes.
		* @note <i>buf</i> should have at least size <i>count</i>
		**/
		size_t read(void *buf, size_t count);

		/**
		* Same as read but allows reading of specified count of
		* elements instead of bytes.
		*
		* @param buf Buffer where to store the readed data
		* @param size Size of each element to be readed
		* @param nmemb Count of elements
		*
		* @return Number of successfully readed elements (not bytes)
		**/
		virtual size_t read(void *buf, size_t size, size_t nmemb) = 0;

		/**
		* Same as read, but reads the line until it found
		* the delimiter string in the data
		**/
		virtual size_t readDelim(void* buf, size_t count, const std::string& delim = std::string("\n")) = 0;

		/**
		* Returns the current byte offset from the beginning
		* in other words position of the stream cursor
		**/
		virtual size_t tell() const = 0;

		/**
		* Returns true if end of the stream is reached
		**/
		virtual bool eof() const = 0;

		/**
		* Return the full data containing in the stream only if we were
		* able to retrieve the data. If the stream is buffered, so it
		* should return the content of the whole buffer and retrieve new data.
		* @param count Return count of bytes returned by the function
		**/
		virtual byte* getData(size_t& count) const = 0;

		/**
		* Only for text-only streams. You have the abbility to return the whole
		* stream content as a string
		**/
		virtual std::string getAsString();

		/**
		* Read a line from the stream. Only usefull for text-only-streams
		**/
		virtual std::string getLine();

		/**
		* Seek the read pointer to specified position.
		*
		* @param offset Number of bytes to jump
		* @param whence From where we should compute the new position
		*				CURRENT, START, END
		* @return false if error occurs (e.g. eof or wrong whence position)
		**/
		virtual bool seek(int32 offset, int32 whence = CURRENT) = 0;

		/**
		* Returns the size of the stream (ength of the data in the stream)
		* If size could not be determined, so return 0
		**/
		size_t size() const { return mSize; }

		/**
		* Close the stream. After you close it no operations like seek, or read
		* are valid anymore. The stream is also automaticly closed if
		* you call the destructor.
		**/
		virtual void close () = 0;

		//! Default size of the buffer
		static const uint32 STREAM_BUFFER_SIZE = 256;

	protected:

		//! Name of the stream
		std::string	mName;

		//! Size of the stream in bytes. If size could not be determined so 0
		size_t	mSize;

		//! Our stream buffer should be of this size
		uint32 mStreamBufSize;

		//! Buffer to hold the data.
		byte*	mBuffer;

	};

}; // end namespace

#endif
