// Copyright Kinetix. All Rights Reserved.


#include "SmartCache/KinetixCacheAnimation.h"

FKinetixCacheAnimation::FKinetixCacheAnimation()
{}

FKinetixCacheAnimation::FKinetixCacheAnimation(const FAnimationID& InID, const FDateTime& InDateTime)
	: ID(InID)
{}

FKinetixCacheAnimation::~FKinetixCacheAnimation()
{}

const FAnimationID& FKinetixCacheAnimation::GetID() const
{
	return ID;
}

float FKinetixCacheAnimation::GetScore() const
{
	float Score = 1 - (float) (FDateTime::Now() - LastTimePlayed).GetTotalSeconds();
	return Score;
}
