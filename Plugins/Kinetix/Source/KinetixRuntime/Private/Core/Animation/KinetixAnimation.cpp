// // Copyright Kinetix. All Rights Reserved.

#include "Core/Animation/KinetixAnimation.h"

#include "Managers/LocalPlayerManager.h"
#include "Utils/Animation/RootMotionConfig.h"

UKinetixAnimation::UKinetixAnimation()
{
};

UKinetixAnimation::UKinetixAnimation(FVTableHelper& Helper)
	:Super(Helper)
{
}

UKinetixAnimation::~UKinetixAnimation()
{
	LocalPlayerManager = nullptr;
}

void UKinetixAnimation::Initialize(bool bInPlayAutomaticallyOnAnimInstance)
{
	LocalPlayerManager = MakeUnique<FLocalPlayerManager>(bInPlayAutomaticallyOnAnimInstance);
}

void UKinetixAnimation::RegisterLocalPlayerAnimInstance(UAnimInstance* InAnimInstance)
{
	if (!IsValid(InAnimInstance))
		return;

	AnimInstance = InAnimInstance;

	OnRegisterLocalPlayer.Broadcast();
}

void UKinetixAnimation::RegisterLocalPlayerAnimInstanceWithRootMotionConfig(
	UAnimInstance* InAnimInstance, FRootMotionConfig InConfig)
{
}

void UKinetixAnimation::RegisterLocalPlayerCustomAnimInstance(UAnimInstance* InAnimInstance, FTransform InRootTransform,
                                                              EExportType InExportType)
{
}

void UKinetixAnimation::UnregisterLocalPlayer()
{
}

void UKinetixAnimation::PlayAnimationOnLocalPlayer(FAnimationID InAnimationID)
{
}

void UKinetixAnimation::PlayAnimationQueueOnLocalPlayer(TArray<FAnimationID> InAnimationIDs, bool bLoop)
{
}

void UKinetixAnimation::StopAnimationOnLocalPlayer()
{
}

void UKinetixAnimation::LoadLocalPlayerAnimation(FAnimationID InAnimationID,
	const FOnKinetixLocalAnimationLoadingFinished& OnSuccessDelegate)
{
}

void UKinetixAnimation::LoadLocalPlayerAnimations(TArray<FAnimationID> InAnimationIDs,
	const FOnKinetixLocalAnimationLoadingFinished& OnSuccessDelegate)
{
}

void UKinetixAnimation::UnloadLocalPlayerAnimation(FAnimationID AnimationID)
{
}

void UKinetixAnimation::UnloadLocalPlayerAnimations(TArray<FAnimationID> AnimationIDs)
{
}

bool UKinetixAnimation::IsAnimationAvailableOnLocalPlayer(FAnimationID InAnimationID)
{
	return false;
}

UKinetixComponent* UKinetixAnimation::GetLocalKCC() const
{
	return nullptr;
}

void UKinetixAnimation::SetReferenceSkeletalMesh(USkeletalMesh* InSkeletalMesh)
{
	ReferenceSkeletalMesh = InSkeletalMesh;
}
