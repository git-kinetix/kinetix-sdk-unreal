// Copyright Kinetix. All Rights Reserved.

#include "Components/KinetixCharacterComponent.h"

#include "Components/SkeletalMeshComponent.h"

#include "glTFRuntimeFunctionLibrary.h"

#include "Kismet/KismetSystemLibrary.h"

// For the logs
#include "KinetixRuntimeModule.h"
#include "Components/AnimSequenceSamplerComponent.h"
#include "Core/KinetixCoreSubsystem.h"
#include "Core/Animation/KinetixAnimation.h"
#include "Core/Metadata/KinetixMetadata.h"
#include "Engine/ActorChannel.h"
#include "Interfaces/KinetixAnimationInterface.h"
#include "Managers/EmoteManager.h"
#include "Net/UnrealNetwork.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"

// Sets default values for this component's properties
UKinetixCharacterComponent::UKinetixCharacterComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	bRegisterPlayerOnLaunch = false;

	SetIsReplicatedByDefault(true);
}

UKinetixCharacterComponent::UKinetixCharacterComponent(FVTableHelper& Helper)
{
}

void UKinetixCharacterComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UKinetixCharacterComponent, AnimSequenceSamplerComponent);
}

void UKinetixCharacterComponent::ServerCalledFromClientWithData_Implementation(const FString& DataToSend)
{
	auto CurrentOwner = GetOwner();
	UKismetSystemLibrary::PrintString(this,
	                                  FString::Printf(TEXT("%s %s ServerCalledFromClientWithData: %s"),
	                                                  *UEnum::GetValueAsString(CurrentOwner->GetLocalRole()),
	                                                  *UEnum::GetValueAsString(CurrentOwner->GetRemoteRole()),
	                                                  *DataToSend), true, true,
	                                  CurrentOwner->HasAuthority() == true ? FLinearColor::Red : FLinearColor::Blue,
	                                  2.f);
}

void UKinetixCharacterComponent::RegisterSampler(IKinetixSamplerInterface* InAnimSequenceSamplerComponent)
{
	AnimSampler = InAnimSequenceSamplerComponent;
}

// Called when the game starts
void UKinetixCharacterComponent::BeginPlay()
{
	Super::BeginPlay();

	CheckSkeletalMeshComponent();
}

void UKinetixCharacterComponent::CheckAnimInstanceToNotify(AActor* CurrentOwner)
{
	TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
	CurrentOwner->GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);
	UAnimInstance* AnimInstance = nullptr;
	for (int i = 0; i < SkeletalMeshComponents.Num(); ++i)
	{
		AnimInstance = SkeletalMeshComponents[i]->GetAnimInstance();
		if (!IsValid(AnimInstance))
			continue;

		if (AnimInstance->Implements<UKinetixAnimationInterface>())
		{
			AnimInstanceToNotify.SetInterface(Cast<IKinetixAnimationInterface>(AnimInstance));
			AnimInstanceToNotify.SetObject(AnimInstance);
		}
	}
}

void UKinetixCharacterComponent::PlayAnimation_Implementation(const FAnimationID& InAnimationID, bool bLoop,
                                                              const FOnPlayedKinetixAnimationLocalPlayer&
                                                              OnPlayedAnimationDelegate)
{
	if (!InAnimationID.UUID.IsValid())
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("[KinetixComponent] PlayAnimation: %s Given AnimationID is null !"),
		       *GetName());
		return;
	}

	UKinetixCoreSubsystem* KinetixCoreSubsystem = Cast<UKinetixCoreSubsystem>(
		USubsystemBlueprintLibrary::GetGameInstanceSubsystem(this, UKinetixCoreSubsystem::StaticClass()));
	if (!IsValid(KinetixCoreSubsystem))
	{
		UE_LOG(LogKinetixRuntime, Error, TEXT("[UKinetixComponent] PlayAnimation: KinetixCoreSubsystem unavailable !"));
		return;
	}

	FEmoteManager::Get().GetAnimSequence(InAnimationID,
	                                     TDelegate<void(UAnimSequence*)>::CreateLambda([&](UAnimSequence* AnimSequence)
	                                     {
		                                     if (!IsValid(AnimSequence))
		                                     {
			                                     UE_LOG(LogKinetixRuntime, Warning,
			                                            TEXT(
				                                            "[UKinetixComponent] PlayAnimation: %s AnimSequence is null !"
			                                            ), *GetName());
			                                     return;
		                                     }

		                                     OwnerSkeletalMeshComponent->PlayAnimation(AnimSequence, false);
		                                     OnPlayedAnimationDelegate.Broadcast(InAnimationID);
		                                     OnAnimationStart.Broadcast(InAnimationID);
		                                     if (AnimSampler)
			                                     AnimSampler->Execute_PlayAnimation(
				                                     AnimSampler->_getUObject(), AnimSequence);

		                                     GetWorld()->GetTimerManager().SetTimer(
			                                     EndAnimationHandle, this,
			                                     &UKinetixCharacterComponent::OnKinetixAnimationEnded,
			                                     AnimSequence->GetPlayLength());
		                                     if (!IsValid(AnimInstanceToNotify.GetObject()))
			                                     return;
		                                     AnimInstanceToNotify->Execute_SetKinetixAnimationPlaying(
			                                     AnimInstanceToNotify.GetObject(), true);
		                                     CurrentAnimationIDBeingPlayed = InAnimationID;
	                                     }));
}

void UKinetixCharacterComponent::CheckSkeletalMeshComponent()
{
	AActor* CurrentOwner = GetOwner();
	if (!IsValid(CurrentOwner))
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("%s's owner is NULL !"), *GetName());
		return;
	}

	OwnerSkeletalMeshComponent = Cast<USkeletalMeshComponent>(
		CurrentOwner->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (!IsValid(OwnerSkeletalMeshComponent))
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("%s's owner doesn't have SkeletalMeshComponent !"), *GetName());
		CurrentOwner->GetWorldTimerManager().SetTimer(CheckSkeletalMeshTimer, this,
		                                              &UKinetixCharacterComponent::CheckSkeletalMeshComponent, 0.1f,
		                                              true);
		return;
	}

	if (!OwnerSkeletalMeshComponent->IsAnimBlueprintInstanced())
	{
		OwnerSkeletalMeshComponent->OnAnimInitialized.AddDynamic(
			this, &UKinetixCharacterComponent::OnOwnerAnimationInitialized);
		return;
	}

	OnOwnerAnimationInitialized();

	if (GetOwnerRole() != ROLE_AutonomousProxy && GetNetMode() != NM_Standalone)
	{
		OwnerSkeletalMeshComponent->SetHiddenInGame(true, true);
	}

	CheckAnimInstanceToNotify(CurrentOwner);
}

void UKinetixCharacterComponent::SetSkeletalMeshComponent(USkeletalMeshComponent* InSkeletalMeshComponent)
{
	OwnerSkeletalMeshComponent = InSkeletalMeshComponent;
}

void UKinetixCharacterComponent::OnOwnerAnimationInitialized()
{
	UWorld* CurrentWorld = GetWorld();
	if (!IsValid(CurrentWorld))
	{
		UKismetSystemLibrary::PrintString(this,
		                                  FString::Printf(
			                                  TEXT("%s OnOwnerAnimationInitialized: GetWorld() returned null!"),
			                                  *GetName()),
		                                  true,
		                                  true,
		                                  FLinearColor::Yellow,
		                                  10.f);

		return;
	}

	UGameInstance* GameInstance = CurrentWorld->GetGameInstance();
	if (!IsValid(GameInstance))
	{
		UKismetSystemLibrary::PrintString(this,
		                                  FString::Printf(
			                                  TEXT("%s OnOwnerAnimationInitialized: GetGameInstance() returned null!"),
			                                  *GetName()),
		                                  true,
		                                  true,
		                                  FLinearColor::Yellow,
		                                  10.f);

		return;
	}

	UKinetixCoreSubsystem* KinetixCoreSubsystem = GameInstance->GetSubsystem<UKinetixCoreSubsystem>();
	if (!IsValid(KinetixCoreSubsystem))
	{
		UKismetSystemLibrary::PrintString(this,
		                                  FString::Printf(
			                                  TEXT("%s OnOwnerAnimationInitialized: GetSubsystem() returned null!"),
			                                  *GetName()),
		                                  true,
		                                  true,
		                                  FLinearColor::Yellow,
		                                  10.f);
		return;
	}

	// KinetixCoreSubsystem->KinetixAnimation->RegisterLocalPlayerAnimInstance(
	// 	OwnerSkeletalMeshComponent->GetAnimInstance());
}

FString UKinetixCharacterComponent::RemapBones(const int32 NodeIndex, const FString& CurveName, const FString& Path,
                                               UObject* Context)
{
	return CurveName;
}

void UKinetixCharacterComponent::PlayAnimationOld(const FAnimationID& InAnimationID, bool bLoop,
                                                  const FOnPlayedKinetixAnimationLocalPlayer& OnPlayedAnimationDelegate)
{
}

void UKinetixCharacterComponent::OnKinetixAnimationEnded()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
		World->GetTimerManager().ClearTimer(EndAnimationHandle);

	OwnerSkeletalMeshComponent->GetAnimInstance()->OnMontageEnded.RemoveAll(this);
	OnAnimationEnd.Broadcast(CurrentAnimationIDBeingPlayed);

	if (AnimSampler)
		AnimSampler->Execute_StopAnimation(
			AnimSampler->_getUObject());
	
	if (!IsValid(AnimInstanceToNotify.GetObject()))
		return;
	AnimInstanceToNotify->Execute_SetKinetixAnimationPlaying(AnimInstanceToNotify.GetObject(), false);
}
