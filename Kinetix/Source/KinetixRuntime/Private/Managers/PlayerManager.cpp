// // Copyright Kinetix. All Rights Reserved.

#include "PlayerManager.h"

#include "EmoteManager.h"
#include "KinetixRuntimeModule.h"
#include "MemoryManager.h"
#include "Components/KinetixCharacterComponent.h"
#include "Core/Animation/KinetixAnimation.h"

FPlayerManager::FPlayerManager(bool bInPlayAutomaticallyOnAnimator)
	: UUID(FGuid::NewGuid()),
	  bPlayAutomatcallyOnAnimInstance(bInPlayAutomaticallyOnAnimator),
	  EmotesToPreload(nullptr)
{
	UE_LOG(LogKinetixRuntime, Log, TEXT("PlayerManager instanciated"));
}

FPlayerManager::~FPlayerManager()
{
	UE_LOG(LogKinetixRuntime, Log, TEXT("LocalPlayerManager destroyed"));
}

bool FPlayerManager::AddKinetixComponentAndInitialize(UAnimInstance* InAnimInstance, FString AvatarUUID)
{
	USkeletalMeshComponent* InSkeletalMeshComponent = InAnimInstance->GetOuterUSkeletalMeshComponent();
	if (!IsValid(InSkeletalMeshComponent))
		return false;

	AActor* AnimInstanceOwner = InSkeletalMeshComponent->GetOwner();
	if (!IsValid(AnimInstanceOwner))
		return false;

	LocalKinetixComponent = AnimInstanceOwner->FindComponentByClass<UKinetixCharacterComponent>();
	if (IsValid(LocalKinetixComponent))
	{
		LocalKinetixComponent->SetAvatarID(AvatarUUID);
		return true;
	}

	LocalKinetixComponent = NewObject<UKinetixCharacterComponent>(AnimInstanceOwner);
	if (!IsValid(LocalKinetixComponent))
		return false;

	LocalKinetixComponent->SetAvatarID(AvatarUUID);
	LocalKinetixComponent->RegisterComponent();
	InSkeletalMeshComponent->GetOwner()->AddInstanceComponent(LocalKinetixComponent);

	return true;
}

bool FPlayerManager::AddPlayerCharacterComponent(UAnimInstance* InAnimInstance, FString AvatarUUID)
{
	if (!AddKinetixComponentAndInitialize(InAnimInstance, AvatarUUID))
		return false;

	OnRegisterLocalPlayer();

	return true;
}

bool FPlayerManager::RemovePlayerCharacterComponent()
{
	// Component already removed or never spawn 
	if (!IsValid(LocalKinetixComponent))
		return true;

	AActor* LocalOwner = LocalKinetixComponent->GetOwner();
	if (!IsValid(LocalOwner))
	{
		UE_LOG(LogKinetixAnimation, Warning, TEXT("[LocalPlayerManager]: %s is valid but his owner is null !"),
		       *LocalKinetixComponent->GetName());
		LocalKinetixComponent->DestroyComponent();
		return false;
	}

	LocalKinetixComponent->DestroyComponent();
	return true;
}

bool FPlayerManager::IsAnimationAvailable(FAnimationID InAnimationID) const
{
	return false;
}

bool FPlayerManager::IsEmoteUsedByPlayer(FAnimationID InAnimationID) const
{
	return false;
}

void FPlayerManager::GetNotifiedOnAnimationReadyToPlayOnLocalAnimInstance(FAnimationID InAnimationID,
                                                                          TDelegate<void()> OnSucceedDelegate)
{
}

void FPlayerManager::AnimationStartOnLocalPlayerAnimInstance(FAnimationID InAnimationID) const
{
}

void FPlayerManager::AnimationEndOnLocalPlayerAnimInstance(FAnimationID InAnimationID) const
{
}

TArray<FAnimationID> FPlayerManager::GetDownloadedAnimationReadyToPlay() const
{
	return TArray<FAnimationID>();
}

void FPlayerManager::PlayAnimation(const FAnimationID& InAnimationID,
                                   const FOnPlayedKinetixAnimationLocalPlayer& OnPlayedAnimation)
{
	if (!IsValid(LocalKinetixComponent))
		return;

	if (bPlayAutomatcallyOnAnimInstance)
	{
		LocalKinetixComponent->PlayAnimation(InAnimationID, false, OnPlayedAnimation);
	}
}

void FPlayerManager::PlayAnimationQueue(TArray<FAnimationID> InAnimationIDs, bool bLoop,
                                        TDelegate<void(TArray<FAnimationID>)> OnPlayedAnimations)
{
}

void FPlayerManager::StopAnimation()
{
}

TObjectPtr<UKinetixCharacterComponent> FPlayerManager::GetLocalKinetixComponent() const
{
	return LocalKinetixComponent;
}

void FPlayerManager::UnregisterKinetixComponent()
{
}

void FPlayerManager::LoadLocalPlayerAnimation(const FAnimationID& InAnimationID, FString InLockID, FString AvatarUUID,
                                              const TDelegate<void()>& OnSuccess,
                                              const TDelegate<void()>& OnFailure)
{
	FKinetixEmote* Emote = FEmoteManager::Get().GetEmote(InAnimationID);
	if (Emote == nullptr)
	{
		OnFailure.ExecuteIfBound();
		return;
	}

	if (DownloadedEmotesReadyToPlay.Contains(InAnimationID))
		OnSuccess.ExecuteIfBound();

	Async(EAsyncExecution::TaskGraphMainThread, [InAnimationID, Emote, AvatarUUID, OnSuccess, OnFailure, this]()
	{
		FEmoteManager::Get().LoadAnimation(Emote, TDelegate<void()>::CreateLambda(
			                                   [InAnimationID, Emote, OnSuccess, this]()
			                                   {
				                                   DownloadedEmotesReadyToPlay.Add(
					                                   InAnimationID, Emote->GetAnimSequence());
				                                   UE_LOG(LogKinetixAnimation, Log,
				                                          TEXT(
					                                          "[LocalPlayerManager] LoadLocalPlayerAnimation: AnimationLoaded"
				                                          ));

				                                   OnSuccess.ExecuteIfBound();
			                                   }), AvatarUUID);
	});
}

void FPlayerManager::UnloadPlayerAnimation(const FAnimationID& InAnimationID)
{
	FMemoryManager::Get().OnAnimationUnloadedOnPlayer(InAnimationID);
}

void FPlayerManager::OnRegisterLocalPlayer()
{
}
