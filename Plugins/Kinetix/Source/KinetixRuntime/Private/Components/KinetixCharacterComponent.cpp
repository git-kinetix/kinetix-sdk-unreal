// Copyright Kinetix. All Rights Reserved.

#include "Components/KinetixCharacterComponent.h"

#include "Components/SkeletalMeshComponent.h"

#include "glTFRuntimeFunctionLibrary.h"

#include "Kismet/KismetSystemLibrary.h"

// For the logs
#include "KinetixRuntimeModule.h"
#include "Core/Animation/AnimSequenceSampler.h"
#include "Core/KinetixCoreSubsystem.h"
#include "Core/Animation/KinetixAnimation.h"
#include "Core/Metadata/KinetixMetadata.h"
#include "Managers/EmoteManager.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"

// Sets default values for this component's properties
UKinetixCharacterComponent::UKinetixCharacterComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bRegisterPlayerOnLaunch = false;

	SetIsReplicatedByDefault(true);
}

UKinetixCharacterComponent::UKinetixCharacterComponent(FVTableHelper& Helper)
{
}

void UKinetixCharacterComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UKismetSystemLibrary::PrintString(
		this,
		FString::Printf(TEXT("KCC Network Role: %s"), *UEnum::GetValueAsString(GetOwnerRole())),
		true, false, FLinearColor::Yellow, 0.f);
}

// Called when the game starts
void UKinetixCharacterComponent::BeginPlay()
{
	Super::BeginPlay();

	CheckSkeletalMeshComponent();

	if (!RegisterClipSampler())
	{
		UE_LOG(LogKinetixRuntime, Warning,
		       TEXT("[KinetixCharacterComponent] BeginPlay: ERROR! Unable to create Clip Sampler on %s !"),
		       *GetOwner()->GetName());
	}
}

bool UKinetixCharacterComponent::RegisterClipSampler()
{
	AActor* CurrentOwner = GetOwner();
	if (!IsValid(CurrentOwner))
		return false;

	AnimSequenceSampler = MakeShared<FAnimSequenceSampler>(this);

	return true;
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

void UKinetixCharacterComponent::PlayAnimation(const FAnimationID& InAnimationID, bool bLoop,
                                               const FOnPlayedKinetixAnimationLocalPlayer& OnPlayedAnimationDelegate)
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

		                                     AnimSequenceSampler.Get()->Play(AnimSequence, InAnimationID);
		                                     // OwnerClipSampler->Play(AnimSequence, InAnimationID);
		                                     OwnerSkeletalMeshComponent->PlayAnimation(AnimSequence, false);
		                                     OnPlayedAnimationDelegate.Broadcast(InAnimationID);
	                                     }));
}