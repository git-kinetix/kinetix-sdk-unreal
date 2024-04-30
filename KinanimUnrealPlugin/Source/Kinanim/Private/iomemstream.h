// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <iostream>
#include <ostream>
#include <streambuf>
#include <vector>

class KINANIM_API memoryBuffer:
	public std::streambuf
{
	private:
	std::vector<char> buffer;
	std::ios_base::openmode mode;

	public:
	memoryBuffer(const char* buffer, const size_t width, std::ios_base::openmode _Mode = std::ios_base::in | std::ios_base::out | std::ios_base::binary);
	memoryBuffer(size_t startSize = 1024, std::ios_base::openmode _Mode = std::ios_base::in | std::ios_base::out | std::ios_base::binary);
	~memoryBuffer() override;

	protected:
	virtual std::streamsize showmanyc() override;

	virtual std::streamsize xsgetn(char* buffer, std::streamsize size) override;

	virtual std::streamsize xsputn(const char* buffer, std::streamsize size) override;

	virtual int_type overflow(int_type c = EOF) override;

	pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which) override;

	private:
	void updatePointerPos(std::streamsize arraySize, std::streampos poseG, std::streampos poseP);
};

class KINANIM_API ioMemStream:
	public std::iostream
{
	public:
	ioMemStream(const char* data, size_t size, ios_base::openmode _Mode = ios_base::in | ios_base::out | ios_base::binary);
	ioMemStream(size_t size, ios_base::openmode _Mode = ios_base::in | ios_base::out | ios_base::binary);
	
	virtual ~ioMemStream() override;
	
};
//-------------------//