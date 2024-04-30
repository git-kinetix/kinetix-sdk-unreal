// Copyright Kinetix. All Rights Reserved.


#include "KinanimTypes.h"

FFrameData FrameDataUtils::Clone(const FFrameData& InFrame)
{
	FFrameData toReturn = {};
	toReturn.TransformDeclarationFlag = InFrame.TransformDeclarationFlag;
	toReturn.BlendshapeDeclarationFlag = InFrame.BlendshapeDeclarationFlag;

	for (uint8 i = 0; i < static_cast<uint8>(EKinanimTransform::KT_Count); i++)
	{
		toReturn.Transforms[i] = InFrame.Transforms[i];
	}

	for (uint8 i = 0; i < static_cast<uint8>(EKinanimBlendshape::KB_Count); i++)
	{
		toReturn.Blendshapes[i] = InFrame.Blendshapes[i];
	}

	return FFrameData();
}
