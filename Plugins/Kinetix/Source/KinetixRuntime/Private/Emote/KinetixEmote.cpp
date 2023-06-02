// Copyright Kinetix. All Rights Reserved.


#include "Emote/KinetixEmote.h"

FKinetixEmote::FKinetixEmote(const FAnimationID& InAnimationID)
	: AnimationID(InAnimationID), bIsLocal(false)
{
	
}

FKinetixEmote::~FKinetixEmote()
{
}

bool FKinetixEmote::HasMetadata() const
{
	return AnimationMetadata.Id.UUID.IsValid();
}

void FKinetixEmote::SetMetadata(const FAnimationMetadata& InAnimationMetadata)
{
	AnimationMetadata = InAnimationMetadata;
}

void FKinetixEmote::SetLocalMetadata(const FAnimationMetadata& InAnimationMetata, FString InLocalGLBPath)
{
	AnimationMetadata = InAnimationMetata;
	PathToGLB = InLocalGLBPath;
	bIsLocal = true;
}

void FKinetixEmote::SetShortMetadata(const FAnimationID& InAnimationID, FString InAnimationURL)
{
	AnimationID = InAnimationID;
	AnimationMetadata.Id = InAnimationID;
	AnimationMetadata.AnimationURL.Map = InAnimationURL;
}

bool FKinetixEmote::IsFileInUse() const
{
	return false;
}

bool FKinetixEmote::HasAnimationRetargeted(UAnimInstance* InAnimInstance)
{
	return false;
}

bool FKinetixEmote::IsLocal() const
{
	return bIsLocal;
}

FString FKinetixEmote::GetPathToGlb() const
{
	return PathToGLB;
}

bool FKinetixEmote::HasValidPath() const
{
	return false;
}

bool FKinetixEmote::IsRetargeting(UAnimInstance* InAnimInstance)
{
	return false;
}
