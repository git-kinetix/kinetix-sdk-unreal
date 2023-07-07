// // Copyright Kinetix. All Rights Reserved.

#include "Core/Animation/KinetixAnimation.h"

#include "Managers/LocalPlayerManager.h"
#include "Tasks/Task.h"
#include "Utils/Animation/RootMotionConfig.h"

DEFINE_LOG_CATEGORY(LogKinetixAnimation);

UKinetixAnimation::UKinetixAnimation()
{
};

UKinetixAnimation::UKinetixAnimation(FVTableHelper& Helper)
	: Super(Helper)
{
}

UKinetixAnimation::~UKinetixAnimation()
{
	LocalPlayerManager = nullptr;
}

void UKinetixAnimation::Initialize_Implementation(const FKinetixCoreConfiguration& CoreConfiguration, bool& bResult)
{
	LocalPlayerManager = MakeUnique<FLocalPlayerManager>(CoreConfiguration.bPlayAutomaticallyAnimationOnAnimInstances);
	bResult = true;
}

void UKinetixAnimation::Initialize(bool bInPlayAutomaticallyOnAnimInstance)
{
	LocalPlayerManager = MakeUnique<FLocalPlayerManager>(bInPlayAutomaticallyOnAnimInstance);
}

void UKinetixAnimation::RegisterLocalPlayerAnimInstance(UAnimInstance* InAnimInstance)
{
	if (!IsValid(InAnimInstance))
	{
		UE_LOG(LogKinetixAnimation, Warning, TEXT("RegisterLocalPlayerAnimInstance: AnimInstance is null !"));
		return;
	}

	AnimInstance = InAnimInstance;

	LocalPlayerManager.Get()->AddPlayerCharacterComponent(InAnimInstance);

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

void UKinetixAnimation::PlayAnimationOnLocalPlayer(const FAnimationID& InAnimationID)
{
	LocalPlayerManager->PlayAnimation(InAnimationID, OnPlayedKinetixAnimationLocalPlayer);
}

void UKinetixAnimation::PlayAnimationQueueOnLocalPlayer(TArray<FAnimationID>& InAnimationIDs, bool bLoop)
{
}

void UKinetixAnimation::StopAnimationOnLocalPlayer()
{
}

void UKinetixAnimation::LoadLocalPlayerAnimation(const FAnimationID& InAnimationID, FString& InLockID,
                                                 const FOnKinetixLocalAnimationLoadingFinished& OnSuccessDelegate)
{
	LocalPlayerManager.Get()->LoadLocalPlayerAnimation(
		InAnimationID, InLockID,
		TDelegate<void()>::CreateLambda([OnSuccessDelegate]()
		{
			OnSuccessDelegate.ExecuteIfBound(true);
		}));
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

UKinetixCharacterComponent* UKinetixAnimation::GetLocalKCC() const
{
	return nullptr;
}

void UKinetixAnimation::SetReferenceSkeletalMesh(USkeletalMesh* InSkeletalMesh)
{
	ReferenceSkeletalMesh = InSkeletalMesh;
}
