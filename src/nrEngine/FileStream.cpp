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
#include <nrEngine/FileStream.h>
#include <nrEngine/Log.h>

namespace nrEngine {

	//----------------------------------------------------------------------------------
	FileStream::FileStream() : IStream(0), IResource("File")
	{
	}

	//----------------------------------------------------------------------------------
	FileStream::~FileStream()
	{
		unloadResource();
	}

	//----------------------------------------------------------------------------------
	Result FileStream::unloadResource()
	{
		close();
		mStream.reset();
		return OK;
	}

	//----------------------------------------------------------------------------------
	Result FileStream::reloadResource(PropertyList* params)
	{
		return open(getResourceFilenameList().front());
	}
	
	//----------------------------------------------------------------------------------
	Result FileStream::open (const std::string& fileName)
	{
		// create a pointer to the stream object and open the file
		mStream.reset (new std::ifstream(fileName.c_str()));

		// check if the file could be found
		if (!mStream->good())
		{
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "The file \"%s\" was not found", fileName.c_str());
			return FILE_NOT_FOUND;
		}

		// calculate the size
		mStream->seekg(0, std::ios_base::end);
		mSize = mStream->tellg();
		mStream->seekg(0, std::ios_base::beg);

		return OK;
	}

	//----------------------------------------------------------------------------------
	void FileStream::close()
	{
		if (!mStream) return;
		mStream->close();
		mStream.reset();
		mSize = 0;
	}

	//----------------------------------------------------------------------------------
	size_t FileStream::read(void *buf, size_t size, size_t nmemb)
	{
		if (!mStream) return 0;
		mStream->read(static_cast<char*>(buf), size * nmemb);
		return mStream->gcount();
	}

	//----------------------------------------------------------------------------------
	size_t FileStream::readDelim(void* buf, size_t count, const std::string& delim)
	{
		if (!mStream) return 0;
		if (delim.empty()){
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "FileStream::readDelim(): No delimiter provided");
			return 0;
		}

		if (delim.size() > 1){
			NR_Log(Log::LOG_ENGINE, Log::LL_WARNING,
				"FileStream::readDelim(): Only first character of delimiter \"%s\" is used", delim.c_str());
		}

		// Deal with both Unix & Windows LFs
		bool trimCR = false;
		if (delim.at(0) == '\n'){
			trimCR = true;
		}

		// maxCount + 1 since count excludes terminator in getline
		mStream->getline(static_cast<char*>(buf), count+1, delim.at(0));
		size_t ret = mStream->gcount();

		if (mStream->fail()){
			// Did we fail because of maxCount hit?
			if (ret == count){
				// clear failbit for next time
				mStream->clear();
			}else{
				NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "FileStream::readDelim(): Error during reading the stream");
			}
		}

		if (!ret){
			(static_cast<char*>(buf))[1] = '\0';
		}else{
			// trim off CR if we found CR/LF
			if (trimCR && (static_cast<char*>(buf))[ret] == '\r'){
				--ret;
				(static_cast<char*>(buf))[ret+1] = '\0';
			}
		}

		return ret;
	}

	//----------------------------------------------------------------------------------
	size_t FileStream::tell() const
	{
		if (!mStream) return 0;
		mStream->clear(); //Clear fail status in case eof was set
		return mStream->tellg();
	}

	//----------------------------------------------------------------------------------
	bool FileStream::eof() const
	{
		if (!mStream) return true;
        return mStream->eof();
	}

	//----------------------------------------------------------------------------------
	byte* FileStream::getData(size_t& count) const
	{
		if (!mStream || mSize <= 0) return NULL;

		byte* data = new byte[mSize];

		// read data
		mStream->read((char*)data, mSize);

		// get the size of readed data
		count = mStream->gcount();
		if (count < mSize){
			byte* ndata = new byte[count];
			memcpy(ndata, data, sizeof(byte) * count);
			delete [] data;
			return ndata;
		}
		return data;
	}

	//----------------------------------------------------------------------------------
	bool FileStream::seek(int32 offset, int32 whence)
	{
		if (!mStream) return false;
		mStream->clear(); //Clear fail status in case eof was set

		std::_Ios_Seekdir sd;
		if (whence == IStream::START)
			sd = std::ios::beg;
		else if (whence == IStream::END)
			sd = std::ios::end;
		else
			sd = std::ios::cur;

		mStream->seekg(static_cast<std::ifstream::pos_type>(offset), sd);
		return true;
	}

	//----------------------------------------------------------------------------------
	EmptyFileStream::EmptyFileStream()
	{

	}

	//----------------------------------------------------------------------------------
	EmptyFileStream::~EmptyFileStream()
	{

	}

	//----------------------------------------------------------------------------------
	size_t EmptyFileStream::read(void *buf, size_t size, size_t nmemb)
	{
		return 0;
	}

	//----------------------------------------------------------------------------------
	size_t EmptyFileStream::readDelim(void* buf, size_t count, const std::string& )
	{
		return 0;
	}

	//----------------------------------------------------------------------------------
	size_t EmptyFileStream::tell() const{
		return 0;
	}

	//----------------------------------------------------------------------------------
	bool EmptyFileStream::eof() const{
		return true;
	}

	//----------------------------------------------------------------------------------
	byte* EmptyFileStream::getData(size_t& count) const{
		count = 0;
		return NULL;
	}

	//----------------------------------------------------------------------------------
	bool EmptyFileStream::seek(int32 offset, int32 whence)
	{
		return true;
	}

	//----------------------------------------------------------------------------------
	void EmptyFileStream::close ()
	{
		return;
	}


};

