// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "KinanimData.h"

/**
 * 
 */
class KINANIM_API IKinanimCompression
{
public:
	virtual ~IKinanimCompression()
	{};

	/// <summary>
	/// Maximum frame where we are certain everything is uncompressed
	/// </summary>
	virtual int32 GetMaxUncompressedFrame() = 0;
	/// <summary>
	/// Maximum frame where we are certain every transform are uncompressed
	/// </summary>
	virtual int32 GetMaxUncompressedTransforms() = 0;
	/// <summary>
	/// Maximum frame where we are certain every blendshapes are uncompressed
	/// </summary>
	virtual int32 GetMaxUncompressedBlendshapes() = 0;

	/// <summary>
	/// Target animation to compress
	/// </summary>
	/// <remarks>
	/// Compression will be apply to the target directly.<br/>
	/// Clone your datas before if you need to keep your uncompressed frame<br/>
	/// (ex: for reading the animation)
	/// </remarks>
	virtual void InitTarget(FKinanimData* data) = 0;

	/// <summary>
	/// Called once, compress every frame of the <see cref="KinanimData::content"/>
	/// </summary>
	virtual void Compress() = 0;
	/// <summary>
	/// Called when a new batch of frame have been imported
	/// </summary>
	/// <param name="loadedFrameCount">Number of frame imported</param>
	virtual void DecompressFrame(uint16 loadedFrameCount) = 0;
};
