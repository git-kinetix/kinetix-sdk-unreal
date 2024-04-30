// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "KinanimTypes.h"
#include "KinanimData.h"
#include "IKinanimCompression.h"
#include "KinanimID.h"
#include "KinanimPointerCollector.h"
#include <iostream>

class KINANIM_API KinanimImporter final : IDisposable
{
	private: 
		long _highestImportedFrame = -1;
	public:
		long GetHighestImportedFrame() const;

	private: 
		FKinanimData* result;
	public:
		/// <summary>
		/// Imported data
		/// </summary>
		FKinanimData* GetResult() const;

	private:
		FKinanimHeader* _uncompressedHeader = nullptr;
	public:
		FKinanimHeader* GetUncompressedHeader();

		/// <summary>
		/// Lossy compression algorythm to use to decompress (interpolate) the kinanim
		/// </summary>
		IKinanimCompression* compression = nullptr;

		/// <summary>
		/// Import a <see cref="KinanimData"/> from a file.kinanim
		/// </summary>
		/// <param name="compression">Lossy compression algorythm to use to decompress (interpolate) the kinanim</param>
		KinanimImporter(IKinanimCompression* compression = nullptr);
		~KinanimImporter();

		/// <summary>
		/// Set uncompressed header to nullptr to avoid getting it deleted by the destructor
		/// </summary>
		FKinanimHeader* ReleaseUncompressedHeader();

		/// <summary>
		/// Set result to nullptr to avoid getting it deleted by the destructor
		/// </summary>
		/// <returns>Returns the pointer before release</returns>
		FKinanimData* ReleaseResult();

		/// <summary>
		/// Read the header of the kinanim file
		/// </summary>
		/// <param name="stream">Stream (not disposed)</param>
		void ReadHeader(std::istream* reader);

		/// <summary>
		/// Read all frames contained inside the reader
		/// </summary>
		/// <param name="reader">Binary reader (not disposed)</param>
		void ReadFrames(std::istream* reader);
		
		/// <summary>
		/// Read 1 frame contained inside the reader
		/// </summary>
		/// <param name="reader">Binary reader (not disposed)</param>
		void ReadFrame(std::istream* reader);

		void ReadTransform(FFrameData* frameData, std::istream* reader);
		void ReadBlendshape(FFrameData* frameData, std::istream* reader);
private:
		//====================================//
		// Header inner functions             //
		//====================================//

		/// <summary>
		/// Get version from the reader
		/// </summary>
		void ReadVersion(std::istream* reader);
		
		/// <summary>
		/// Get frameRate from the reader
		/// </summary>
		void ReadFrameRate(std::istream* reader);
		
		/// <summary>
		/// Get frameCount and binary size of frames from the reader
		/// </summary>
		void ReadFrameCountAndSizes(std::istream* reader);
		
		/// <summary>
		/// Get type of key of each transform from the reader
		/// </summary>
		void ReadKeyTypes(std::istream* reader);
		
		/// <summary>
		/// Get if the animation contains blendshapes or not
		/// </summary>
		void ReadHasBlendshapes(std::istream* reader);
		
public:
		//====================================//
		// Other                              //
		//====================================//

		/// <summary>
		/// <see cref="UncompressedHeader"/>
		/// </summary>
		/// <param name="minFrame"></param>
		/// <param name="maxFrame"></param>
		void ComputeUncompressedFrameSize(int minFrame, int maxFrame);

		/// <summary>
		/// Read a full file from a stream
		/// </summary>
		/// <remarks>
		/// This method cannot read partial files.<br/>
		/// See also <seealso cref="ReadHeader(Stream)"/> or <seealso cref="ReadFrames(Stream)"/>
		/// </remarks>
		/// <param name="stream">Stream (auto disposed)</param>
		void ReadFile(std::istream* stream);
};