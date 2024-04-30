// Copyright Kinetix. All Rights Reserved.


#include "IKinanimInterface.h"

#include <fstream>

#include "iomemstream.h"

void* Kinanim::OpenReadFile(const TCHAR* filepath)
{
	// IPlatformFile& PlatformFileManager = FPlatformFileManager::Get().GetPlatformFile();
	// IFileHandle* File = PlatformFileManager.OpenReadNoBuffering(filepath);
	// return File;
	std::ifstream* stream = new std::ifstream(filepath, std::ios_base::binary | std::ios_base::in);
	return stream;
}

void* Kinanim::OpenWriteFile(const TCHAR* filepath, bool isAppend)
{
	// IPlatformFile& PlatformFileManager = FPlatformFileManager::Get().GetPlatformFile();
	// IFileHandle* File = PlatformFileManager.OpenWrite(filepath, isAppend);
	// return File;
	std::ios_base::openmode openMode = std::ios_base::binary | std::ios_base::out;
	if (isAppend)
		openMode |= std::ios_base::app;
	
	std::ofstream* stream = new std::ofstream(filepath, openMode);
	return stream;
}

void* Kinanim::CreateBinaryStream(const SIZE_T size)
{
	// ioMemStream* ostr = new ioMemStream(size);
	// return ostr;
	return nullptr;
}

void* Kinanim::CreateBinaryStreamFromArray(const char* binary, const SIZE_T size)
{
	ioMemStream* ostr = new ioMemStream(binary, size);
	return ostr;
}

void Kinanim::ifstream_CloseStream(void* stream)
{
	// std::ifstream* ifStr = (std::ifstream*)stream;
	// ifStr->close();
	// delete ifStr;
}

void Kinanim::ofstream_CloseStream(void* stream)
{
	// std::ofstream* ofStr = (std::ofstream*)stream;
	// ofStr->close();
	// delete ofStr;
}

void Kinanim::ioMemStream_CloseStream(void* stream)
{
	// delete ((ioMemStream*)stream);
}
