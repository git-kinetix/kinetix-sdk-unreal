// // Copyright Kinetix. All Rights Reserved.

#include "Core/Animation/KinetixAnimation.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Managers/EmoteManager.h"
#include "Managers/MemoryManager.h"
#include "Managers/PlayerManager.h"
#include "Managers/PlayersManager.h"
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
	LocalPlayerManager = MakeUnique<FPlayerManager>(CoreConfiguration.bPlayAutomaticallyAnimationOnAnimInstances);
	bResult = true;
}

void UKinetixAnimation::RegisterLocalPlayerAnimInstance(UAnimInstance* InAnimInstance, FString AvatarUUID)
{
	if (!IsValid(InAnimInstance))
	{
		UE_LOG(LogKinetixAnimation, Warning, TEXT("RegisterLocalPlayerAnimInstance: AnimInstance is null !"));
		return;
	}

	AnimInstance = InAnimInstance;

	LocalPlayerManager.Get()->AddPlayerCharacterComponent(InAnimInstance, AvatarUUID);

	OnRegisterLocalPlayer.Broadcast();
}

void UKinetixAnimation::RegisterAvatarAnimInstance(UAnimInstance* InAnimInstance, FGuid& OutGuid, FString AvatarUUID)
{
	FPlayersManager::Get().AddPlayerCharacterComponent(InAnimInstance, OutGuid, AvatarUUID);
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

void UKinetixAnimation::PlayAnimationOnAvatar(FGuid InPlayerGUID, const FAnimationID& InAnimationID)
{
	FPlayersManager::Get().PlayAnimation(
		InPlayerGUID,
		InAnimationID,
		OnPlayedKinetixAnimationLocalPlayer);
}

void UKinetixAnimation::StopAnimationOnLocalPlayer()
{
}

void UKinetixAnimation::LoadLocalPlayerAnimation(const FAnimationID& InAnimationID, FString& InLockID,
                                                 FString AvatarUUID,
                                                 const FOnKinetixLocalAnimationLoadingFinished& OnSuccessDelegate)
{
	LocalPlayerManager.Get()->LoadLocalPlayerAnimation(
		InAnimationID, InLockID, AvatarUUID,
		TDelegate<void()>::CreateLambda([OnSuccessDelegate]()
		{
			OnSuccessDelegate.ExecuteIfBound(true);
		}));
}

void UKinetixAnimation::LoadLocalPlayerAnimations(TArray<FAnimationID>& InAnimationIDs,
                                                  const FOnKinetixLocalAnimationLoadingFinished& OnSuccessDelegate)
{
}

void UKinetixAnimation::UnloadLocalPlayerAnimation(const FAnimationID& InAnimationID)
{
}

void UKinetixAnimation::UnloadLocalPlayerAnimations(TArray<FAnimationID>& AnimationIDs)
{
}

bool UKinetixAnimation::IsAnimationAvailableOnLocalPlayer(const FAnimationID& InAnimationID)
{
	return false;
}

UKinetixCharacterComponent* UKinetixAnimation::GetLocalKCC() const
{
	return nullptr;
}

void UKinetixAnimation::SetReferenceSkeleton(USkeletalMesh* InSkeletalMesh)
{
	FEmoteManager::Get().SetReferenceSkeleton(InSkeletalMesh);
	SetReferenceSkeletalMesh(InSkeletalMesh);
}

void UKinetixAnimation::SetCurveRemapper(const FglTFRuntimeAnimationCurveRemapper& InRemapper)
{
	if (!InRemapper.IsBound())
		return;

	FEmoteManager::Get().SetCurveRemapper(InRemapper);
}

void UKinetixAnimation::SetReferenceSkeletalMesh(USkeletalMesh* InSkeletalMesh)
{
	ReferenceSkeletalMesh = InSkeletalMesh;
}
