// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#ifdef __cplusplus
extern "C"
{
#endif

	namespace Kinanim
	{

		/// <summary>
		/// Open a binary file in read mode
		/// </summary>
		/// <remarks>
		/// Creates a pointer internally, please use <see cref="CloseStream"/> to dispose the stream
		/// </remarks>
		/// <param name="filepath">Path to the file</param>
		/// <returns>Returns a reference to a pointer of the stream</returns>
		KINANIM_API void* /* std::ifstream* */ OpenReadFile(const TCHAR* filepath);
		/// <summary>
		/// Open a binary file in read mode
		/// </summary>
		/// <remarks>
		/// Creates a pointer internally, please use <see cref="CloseStream"/> to dispose the stream
		/// </remarks>
		/// <param name="filepath">Path to the file</param>
		/// <param name="isAppend">If true, append to the end of the file</param>
		/// <returns>Returns a reference to a pointer of the stream</returns>
		KINANIM_API void* /* std::ofstream* */ OpenWriteFile(const TCHAR* filepath, bool isAppend);
		/// <summary>
		/// Create a binary stream in read/write mode
		/// </summary>
		/// <remarks>
		/// Creates a pointer internally, please use <see cref="CloseStream"/> to dispose the stream
		/// </remarks>
		/// <param name="size">Start size allocation (in bytes)</param>
		/// <returns>Returns a reference to a pointer of the stream</returns>
		KINANIM_API void* /* ioMemStream* */ CreateBinaryStream(const SIZE_T size = 1024);
		/// <summary>
		/// Create a binary stream in read/write mode from an array
		/// </summary>
		/// <remarks>
		/// Creates a pointer internally, please use <see cref="CloseStream"/> to dispose the stream
		/// </remarks>
		/// <param name="binary">Byte array to give to </param>
		/// <param name="size">Start size allocation (in bytes)</param>
		/// <returns>Returns a reference to a pointer of the stream</returns>
		KINANIM_API void* /* ioMemStream* */ CreateBinaryStreamFromArray(const char* binary, const SIZE_T size);

		/// <summary>
		/// Dispose the input file stream.
		/// </summary>
		/// <param name="stream">Stream to dispose</param>
		KINANIM_API void ifstream_CloseStream(void* /* std::ifstream */ stream);

		/// <summary>
		/// Dispose the output file stream.
		/// </summary>
		/// <param name="stream">Stream to dispose</param>
		KINANIM_API void ofstream_CloseStream(void* /* std::ofstream */ stream);

		/// <summary>
		/// Dispose the memory stream.
		/// </summary>
		/// <param name="stream">Stream to dispose</param>
		KINANIM_API void ioMemStream_CloseStream(void* /* ioMemStream */ stream);
	}
#ifdef __cplusplus
}
#endif

/**
 * 
 */
class IKinanimInterface
{
public:
	IKinanimInterface();
	~IKinanimInterface();
};
