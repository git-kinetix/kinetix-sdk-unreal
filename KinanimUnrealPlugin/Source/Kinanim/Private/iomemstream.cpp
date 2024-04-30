// Copyright Kinetix. All Rights Reserved.

#include "iomemstream.h"

#include <iostream>
#include <ostream>
#include <streambuf>
#include <vector>
#include <cstring>

memoryBuffer::memoryBuffer(const char* buffer, size_t arraySize, std::ios_base::openmode _Mode)
{
	mode = _Mode;
	this->buffer.reserve(arraySize);
	this->buffer.resize(arraySize);
	std::memcpy(this->buffer.data(), buffer, arraySize);
	updatePointerPos(arraySize, 0, 0);
}

memoryBuffer::memoryBuffer(size_t startSize, std::ios_base::openmode _Mode)
{
	mode = _Mode;
	buffer.reserve(startSize);
	updatePointerPos(startSize, 0, 0);
}

memoryBuffer::~memoryBuffer()
{
	buffer.clear();
}

void memoryBuffer::updatePointerPos(std::streamsize arraySize, std::streampos poseG, std::streampos poseP)
{
	setg(buffer.data(), poseG + buffer.data(), buffer.data() + arraySize);
	setp(buffer.data(), buffer.data() + arraySize);
	pbump(poseP);
}

std::streamsize memoryBuffer::showmanyc()
{ 
	return buffer.size();
}

std::streamsize memoryBuffer::xsgetn(char* s, std::streamsize n)
{
	//s = new char[n];

	if (egptr() - gptr() < n)
	{
		return underflow();
	}

	std::memcpy(s, gptr(), n);
#pragma warning( push )
#pragma warning(disable : 4244)
	gbump(n);
#pragma warning( pop )
	return n;
}

std::streamsize memoryBuffer::xsputn(const char* s, std::streamsize n)
{
	while (epptr() - pptr() < n)
	{
		overflow(EOF);
	}

	uint64 newPosition = pptr() - pbase() + n;
	if (newPosition > buffer.size())
		buffer.resize(newPosition);

	std::memcpy(pptr(), s, n);

#pragma warning( push )
#pragma warning(disable : 4244)
	pbump(n);
#pragma warning( pop )
	return n;
};

memoryBuffer::int_type memoryBuffer::overflow(int_type ch)
{
	if (buffer.capacity() > buffer.size())
	{
		std::streampos gPosition = gptr() - eback();
		std::streampos pPosition = pptr() - pbase();

		buffer.resize(buffer.size() + 1);
		updatePointerPos(buffer.size(), gPosition, pPosition);
		return traits_type::not_eof(ch);
	}

	buffer.reserve(buffer.capacity() * 2); //double the capacity
	return overflow(ch);
}

memoryBuffer::pos_type memoryBuffer::seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which)
{
	if ((which & std::ios_base::out) != 0)
	{
#pragma warning( push )
#pragma warning(disable : 4244)
		if (dir == std::ios_base::cur)
			pbump(off);
		else if (dir == std::ios_base::end) 
		{
			setp(pbase(), epptr());
			pbump(static_cast<int>(epptr() - pbase()) + off);
		}
		else if (dir == std::ios_base::beg) 
		{
			setp(pbase(), epptr());
			pbump(off);
		}
		return pptr() - pbase();
#pragma warning( pop )
	}

	if ((which & std::ios_base::in) != 0)
	{
#pragma warning( push )
#pragma warning(disable : 4244)
		if (dir == std::ios_base::cur)
			gbump(off);
		else if (dir == std::ios_base::end)
			setg(eback(), egptr() + off, egptr());
		else if (dir == std::ios_base::beg)
			setg(eback(), eback() + off, egptr());
		return gptr() - eback();
#pragma warning( pop )
	}

	return -1;
}

ioMemStream::ioMemStream(const char* data, size_t size, ios_base::openmode _Mode):
	std::iostream(new memoryBuffer(data, size, _Mode))
{
}

ioMemStream::ioMemStream(size_t size, ios_base::openmode _Mode):
	std::iostream(new memoryBuffer(size, _Mode))
{
}

ioMemStream::~ioMemStream()
{
	delete rdbuf();
}