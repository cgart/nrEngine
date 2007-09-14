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
#include "IStream.h"

namespace nrEngine {

	//----------------------------------------------------------------------------------
	IStream::IStream(uint32 streamBufSize) : mSize(0), mStreamBufSize(streamBufSize)
	{
		// check for the size. If size is smaller than 1 we want to have unbuffered data
		if (streamBufSize < 1)
			mStreamBufSize = 1;

		// allocate memory for the buffer
		mBuffer = new byte[mStreamBufSize];
	}


	//----------------------------------------------------------------------------------
	IStream::IStream(const std::string& name, uint32 streamBufSize) : mName(name), mSize(0), mStreamBufSize(streamBufSize)
	{
		// check for the size. If size is smaller than 1 we want to have unbuffered data
		if (streamBufSize < 1)
			mStreamBufSize = 1;

		// allocate memory for the buffer
		mBuffer = new byte[mStreamBufSize];
	}

	//----------------------------------------------------------------------------------
	IStream::~IStream()
	{
		// release the buffer
		NR_SAFE_DELETE_ARRAY(mBuffer);
	}


	//----------------------------------------------------------------------------------
  template <typename T> IStream& IStream::operator >>(T& val)
  {
		read(static_cast<void*>(&val), sizeof(T));
		return *this;
	}


	//----------------------------------------------------------------------------------
	std::string IStream::getLine()
	{

		// some variables to store the data
		size_t c = mStreamBufSize;
		std::string str;

		// read until we reached end of file or got
		// on got the new line character
		while (c == mStreamBufSize - 1){

			c = readDelim(mBuffer, mStreamBufSize - 1);

			// copy the content of the bufer to the string
			str += (char*)mBuffer;
		}

		// return the string
		return str;
	}

	//----------------------------------------------------------------------------------
	std::string IStream::getAsString(){

		// create buffer to hold the whole data
		char* pBuf = new char[mSize+1];

		// read the whole data
		size_t c = read(pBuf, mSize);

		// setup the end characeter and create the string
		pBuf[c] = '\0';
		std::string str(pBuf);

		// free data and return
		delete [] pBuf;

		return str;
	}

	//----------------------------------------------------------------------------------
	size_t IStream::read(void *buf, size_t count)
	{
		return read(buf, count, 1);
	}


};

