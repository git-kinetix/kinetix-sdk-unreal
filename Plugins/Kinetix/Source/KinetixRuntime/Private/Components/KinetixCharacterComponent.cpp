// Copyright Kinetix. All Rights Reserved.

#include "Components/KinetixCharacterComponent.h"

#include "Components/SkeletalMeshComponent.h"

#include "glTFRuntimeFunctionLibrary.h"

#include "Kismet/KismetSystemLibrary.h"

// For the logs
#include "KinetixRuntimeModule.h"
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
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	bRegisterPlayerOnLaunch = false;
}


// Called when the game starts
void UKinetixCharacterComponent::BeginPlay()
{
	Super::BeginPlay();

	CheckSkeletalMeshComponent();
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
		                                              &UKinetixCharacterComponent::CheckSkeletalMeshComponent, 0.1f, true);
		return;
	}

	if (!OwnerSkeletalMeshComponent->IsAnimBlueprintInstanced())
	{
		OwnerSkeletalMeshComponent->OnAnimInitialized.AddDynamic(this, &UKinetixCharacterComponent::OnOwnerAnimationInitialized);
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
				UE_LOG(LogKinetixRuntime, Warning, TEXT("[UKinetixComponent] PlayAnimation: %s AnimSequence is null !"));
				return;
			}
			
			OwnerSkeletalMeshComponent->PlayAnimation(AnimSequence, false);
			OnPlayedAnimationDelegate.Broadcast(InAnimationID);
		}));
}

void UKinetixCharacterComponent::LoadAnimationAndPlay(const FString& Url)
{
	if (Url.IsEmpty())
	{
		UKismetSystemLibrary::PrintString(
			this, FString::Printf(TEXT("%s LoadAnimationAndPlay: Url is empty ! Skipping..."),
			                      *GetOwner()->GetName()),
			true, true, FLinearColor::Yellow, 5.f);
		return;
	}

	FglTFRuntimeConfig GltfLoadConfig;
	GltfLoadConfig.TransformBaseType = EglTFRuntimeTransformBaseType::YForward;
	GltfLoadConfig.RuntimeContextObject = this;

	UglTFRuntimeAsset* LoadedGLB =
		UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilename(
			Url, true, GltfLoadConfig);

	if (!IsValid(LoadedGLB))
	{
		UKismetSystemLibrary::PrintString(
			this, FString::Printf(TEXT("%s LoadAnimationAndPlay: GLB is null !"), *GetOwner()->GetName()),
			true, true, FLinearColor::Yellow, 5.f);;
		return;
	}

	// From here we have a filled LoadedGLB
	FglTFRuntimeSkeletalAnimationCurveRemapperHook RemapperHook;
	RemapperHook.Remapper.BindUFunction(this, TEXT("RemapBones"));
	AnimConfig.CurveRemapper = RemapperHook;
	AnimConfig.bRemoveRootMotion = true;
	AnimConfig.bRemoveMorphTargets = true;
	AnimConfig.bRemoveScales = true;
	UAnimSequence* AnimSequenceGLB = LoadedGLB->LoadSkeletalAnimation(
		OwnerSkeletalMeshComponent->GetSkeletalMeshAsset(), 0, AnimConfig);

	if (!IsValid(AnimSequenceGLB))
	{
		UKismetSystemLibrary::PrintString(
			this, FString::Printf(
				TEXT("%s LoadAnimationAndPlay: Loaded asset is NOT an animation !"),
				*GetOwner()->GetName()),
			true, true, FLinearColor::Yellow, 5.f);;
		return;
	}

	OwnerSkeletalMeshComponent->PlayAnimation(AnimSequenceGLB, true);
}
