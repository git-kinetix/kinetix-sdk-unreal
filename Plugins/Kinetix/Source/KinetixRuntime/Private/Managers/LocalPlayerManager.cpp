// // Copyright Kinetix. All Rights Reserved.

#include "Managers/LocalPlayerManager.h"

#include "KinetixRuntimeModule.h"
#include "Components/KinetixComponent.h"
#include "Core/Animation/KinetixAnimation.h"

FLocalPlayerManager::FLocalPlayerManager(bool bInPlayAutomaticallyOnAnimator)
	: bPlayAutomatcallyOnAnimInstance(bInPlayAutomaticallyOnAnimator),
	  EmotesToPreload(nullptr),
	  DownloadedEmotesReadyToPlay(nullptr)
{
	UE_LOG(LogKinetixRuntime, Log, TEXT("LocalPlayerManager instanciated"));
}

FLocalPlayerManager::~FLocalPlayerManager()
{
	UE_LOG(LogKinetixRuntime, Log, TEXT("LocalPlayerManager destroyed"));
}

bool FLocalPlayerManager::AddKinetixComponentAndInitialize(UAnimInstance* InAnimInstance)
{
	USkeletalMeshComponent* InSkeletalMeshComponent = InAnimInstance->GetOuterUSkeletalMeshComponent();
	if (!IsValid(InSkeletalMeshComponent))
		return false;

	AActor* AnimInstanceOwner = InSkeletalMeshComponent->GetOwner();
	if (!IsValid(AnimInstanceOwner))
		return false;

	LocalKinetixComponent = AnimInstanceOwner->FindComponentByClass<UKinetixComponent>();
	if (IsValid(LocalKinetixComponent))
		return true;

	LocalKinetixComponent = NewObject<UKinetixComponent>(AnimInstanceOwner);
	if (!IsValid(LocalKinetixComponent))
		return false;

	LocalKinetixComponent->RegisterComponent();
	InSkeletalMeshComponent->GetOwner()->AddInstanceComponent(LocalKinetixComponent);

	return true;
}

bool FLocalPlayerManager::AddPlayerCharacterComponent(UAnimInstance* InAnimInstance)
{
	if (!AddKinetixComponentAndInitialize(InAnimInstance))
		return false;

	OnRegisterLocalPlayer();

	return true;
}

bool FLocalPlayerManager::RemovePlayerCharacterComponent()
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

bool FLocalPlayerManager::IsAnimationAvailable(FAnimationID InAnimationID) const
{
	return false;
}

bool FLocalPlayerManager::IsEmoteUsedByPlayer(FAnimationID InAnimationID) const
{
	return false;
}

void FLocalPlayerManager::GetNotifiedOnAnimationReadyToPlayOnLocalAnimInstance(FAnimationID InAnimationID,
                                                                               TDelegate<void()> OnSucceedDelegate)
{
}

void FLocalPlayerManager::AnimationStartOnLocalPlayerAnimInstance(FAnimationID InAnimationID) const
{
}

void FLocalPlayerManager::AnimationEndOnLocalPlayerAnimInstance(FAnimationID InAnimationID) const
{
}

TArray<FAnimationID> FLocalPlayerManager::GetDownloadedAnimationReadyToPlay() const
{
	return TArray<FAnimationID>();
}

void FLocalPlayerManager::PlayAnimation(const FAnimationID& InAnimationID, const FOnPlayedKinetixAnimationLocalPlayer& OnPlayedAnimation)
{
	if (!IsValid(LocalKinetixComponent))
		return;

	if (bPlayAutomatcallyOnAnimInstance)
	{
		LocalKinetixComponent->PlayAnimation(InAnimationID, false, OnPlayedAnimation);
	}
}

void FLocalPlayerManager::PlayAnimationQueue(TArray<FAnimationID> InAnimationIDs, bool bLoop,
                                             TDelegate<void(TArray<FAnimationID>)> OnPlayedAnimations)
{
}

void FLocalPlayerManager::StopAnimation()
{
}

TObjectPtr<UKinetixComponent> FLocalPlayerManager::GetLocalKinetixComponent() const
{
	return LocalKinetixComponent;
}

void FLocalPlayerManager::OnRegisterLocalPlayer()
{
	// for (TTuple<FAnimationID, TList<TDelegate<void()>>> Tuple
	//      : CallbacksOnRetargetedAnimationIDOnLocalPlayer)
	// {
	// 	
	// }
}
