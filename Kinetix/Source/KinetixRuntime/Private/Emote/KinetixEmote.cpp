// Copyright Kinetix. All Rights Reserved.


#include "Emote/KinetixEmote.h"

#include "Core/Metadata/KinetixMetadata.h"

FKinetixEmote::FKinetixEmote(): bIsLocal(false), AnimSequence(nullptr)
{
}

FKinetixEmote::FKinetixEmote(const FAnimationID& InAnimationID)
	: AnimationID(InAnimationID), bIsLocal(false), AnimSequence(nullptr)
{
	AnimationMetadata.Id = InAnimationID;
}

FKinetixEmote::~FKinetixEmote()
{
}

bool FKinetixEmote::HasMetadata() const
{
	return AnimationMetadata.Id.UUID.IsValid()
		&& AnimationMetadata.Name.IsValid()
		&& AnimationMetadata.Duration != -1.f
		&& !AnimationMetadata.AnimationURL.Map.IsEmpty()
		&& !AnimationMetadata.IconURL.Map.IsEmpty();
}

void FKinetixEmote::SetMetadata(const FAnimationMetadata& InAnimationMetadata)
{
	AnimationMetadata = InAnimationMetadata;
}

void FKinetixEmote::SetLocalMetadata(const FAnimationMetadata& InAnimationMetadata, FString InLocalGLBPath)
{
	AnimationMetadata = InAnimationMetadata;
	PathToGLB = InLocalGLBPath;
	bIsLocal = true;
}

void FKinetixEmote::SetShortMetadata(const FAnimationID& InAnimationID, FString InAnimationURL)
{
	AnimationID = InAnimationID;
	AnimationMetadata.Id = InAnimationID;
	AnimationMetadata.AnimationURL.Map = InAnimationURL;
}

void FKinetixEmote::SetAnimSequence(UAnimSequence* InAnimSequence)
{
	if (!IsValid(InAnimSequence))
	{
		UE_LOG(LogKinetixMetadata, Warning, TEXT("[KinetixEmote] SetAnimSequence: Given AnimSequence is null !"));
		return;
	}

	bIsLocal = true;
	AnimSequence = InAnimSequence;
	AnimSequence->AddToRoot();
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

const FAnimationMetadata& FKinetixEmote::GetAnimationMetadata() const
{
	return AnimationMetadata;
}

UAnimSequence* FKinetixEmote::GetAnimSequence() const
{
	return AnimSequence;
}

bool FKinetixEmote::HasValidPath() const
{
	return false;
}

bool FKinetixEmote::IsRetargeting(UAnimInstance* InAnimInstance)
{
	return false;
}
