// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "KinanimPointerCollector.h"
#include "KinanimTypes.h"

class KINANIM_API FKinanimHeader final : public IDisposable
{
	/// <summary>
	/// Number of frames
	/// </summary>
	uint16 frameCount;

public:
	/// <summary>
	/// Get the number of frames
	/// </summary>
	uint16 GetFrameCount() const;

	/// <summary>
	/// Set the number of frames
	/// </summary>
	void SetFrameCount(uint16 count);

	/// <summary>
	/// Version of the file formate
	/// </summary>
	uint16 version;

	/// <summary>
	/// Play rate of frames
	/// </summary>
	float frameRate;

	/// <summary>
	/// Binary size of frames in the file
	/// </summary>
	uint16* frameSizes;

	/// <summary>
	/// Size in byte of the header.<br/>
	/// Retrieved during <see cref="KinanimImporter.ReadHeader(System.IO.BinaryReader)"/>.
	/// </summary>
	int32 binarySize;

	/// <summary>
	/// The type of keys used
	/// </summary>
	TArray<EKeyType> KeyTypes;

	/// <summary>
	/// If the animation has blendshapes or not
	/// </summary>
	bool hasBlendshapes;

	FKinanimHeader();
	~FKinanimHeader();


	FKinanimHeader* Clone() const;
};

class KINANIM_API FKinanimContent final : public IDisposable
{
	uint16 _frameCount;
public:
	FFrameData* frames;
	FKinanimContent();
	~FKinanimContent();
	uint32 GetFrameCount() const;
		
	void InitFrameCount(uint16 frameCount);

	FKinanimContent* Clone() const;
};
/**
 * 
 */
class KINANIM_API FKinanimData final : public IDisposable
{
public:
	static const char* FILE_EXTENSION;
	static const uint16 VERSION = 0;

	FKinanimHeader*  Header;
	FKinanimContent* Content;

	FKinanimData();
	~FKinanimData();
private: 
	FKinanimData(FKinanimHeader* InHeader, FKinanimContent* InContent);
		
public:
	void CalculateEveryFrameSize() const;

	uint16 CalculateFrameSize(FFrameData data) const;
		
	FKinanimData* Clone() const;
};