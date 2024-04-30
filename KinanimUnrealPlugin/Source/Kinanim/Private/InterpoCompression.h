// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IKinanimCompression.h"

class KINANIM_API InterpoCompression:
	public IKinanimCompression
{
	public:
		/// <summary>
		/// Default value for maxFramePerLerp in constructor. 
		/// </summary>
		static const int DEFAULT_BATCH_SIZE = 20;

	private:
		/// <summary>
		/// A small number
		/// </summary>
		static const float EPSILON;

	public:
		/// <summary>
		/// Maximum frame per interpolation<br/>
		/// <br/>
		/// For example: Interpolation from 2 to 5 has 6 frames
		/// </summary>
		uint16 maxFramePerLerp;
		/// <summary>
		/// A value between 0 and 1 that will result in more or less compression
		/// </summary>
		float threshold;
		/// <summary>
		/// A value between 0 and 1 for checking the distance between 2 blendshape values
		/// </summary>
		float blendshapeThrshold;


	private:
		FKinanimData* target = nullptr;
		
		/// <summary>
		/// Decompression position (= last frame that is decompressed)
		/// </summary>
		uint16 lastNonCompressedFrame[static_cast<uint64>(EKinanimTransform::KT_Count)] = {};
		/// <summary>
		/// Decompression position for blendshapes (= last frame that is decompressed)
		/// </summary>
		uint16 lastNonCompressedBlendshape[static_cast<uint64>(EKinanimBlendshape::KB_Count)] = {};

		//Don't switch to "short" cuz we need the negative value

		int32 _maxUncompressedTransforms  = -1;
		int32 _maxUncompressedBlendshapes = -1;

		uint16 loadedFrameCount = 0;

	public:
		/// <summary>
		/// Constructor of <see cref="InterpoCompression"/>
		/// </summary>
		/// <param name="maxFramePerLerp">
		/// Maximum frame per interpolation<br/>
		/// <br/>
		/// For example: Interpolation from 2 to 5 has 6 frames
		/// </param>
		/// <param name="threshold">
		/// A value between 0 and 1 that will result in more or less compression
		/// </param>
		/// <param name="blendshapeThrshold">
		/// A value between 0 and 1 for checking the distance between 2 blendshape values
		/// </param>
		InterpoCompression(uint16 maxFramePerLerp = DEFAULT_BATCH_SIZE, float threshold = 0.1f, float blendshapeThrshold = 0.01f);
		virtual ~InterpoCompression() override;

		// Implemented IKinanimCompression
	public:
		/// <inheritdoc/>
		int32 GetMaxUncompressedFrame() override;
		/// <inheritdoc/>
		int32 GetMaxUncompressedTransforms() override;
		/// <inheritdoc/>
		int32 GetMaxUncompressedBlendshapes() override;
		/// <inheritdoc/>
		void InitTarget(FKinanimData* data) override;

		/// <inheritdoc/>
		void Compress() override;
		/// <inheritdoc/>
		void DecompressFrame(uint16 InLoadedFrameCount) override;

		// ------------------------------

	private:
		float CalculateThreshold(float movementQuantity, int frameCount);
		float CalculateMovement(EKinanimTransform tr, FFrameData frames[], int from, int to);
		
		void CompressTransforms(unsigned short& numberOfFrameCut, int frameCount);
		void CompressBlendshapes(unsigned short& numberOfFrameCut, int frameCount);

		void DecompressTransforms();
		void DecompressBlendshapes();

		/// <summary>
		/// Calculates <see cref="_maxUncompressedTransforms"/> and <see cref="_maxUncompressedBlendshapes"/>
		/// </summary>
		void CalculateMaxUncompressedProperties();
};
