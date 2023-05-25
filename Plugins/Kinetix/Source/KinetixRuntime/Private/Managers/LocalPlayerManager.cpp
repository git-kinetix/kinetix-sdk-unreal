// // Copyright Kinetix. All Rights Reserved.

#include "Managers/LocalPlayerManager.h"

#include "KinetixRuntimeModule.h"
#include "Components/KinetixComponent.h"

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

void FLocalPlayerManager::OnRegisterLocalPlayer()
{
	// for (TTuple<FAnimationID, TList<TDelegate<void()>>> Tuple
	//      : CallbacksOnRetargetedAnimationIDOnLocalPlayer)
	// {
	// 	
	// }
}
