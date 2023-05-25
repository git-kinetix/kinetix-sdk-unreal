// Copyright Kinetix. All Rights Reserved.


#include "Components/KinetixComponent.h"

#include "Components/SkeletalMeshComponent.h"

#include "glTFRuntimeFunctionLibrary.h"

#include "Kismet/KismetSystemLibrary.h"

// For the logs
#include "KinetixRuntimeModule.h"
#include "Core/KinetixCoreSubsystem.h"
#include "Core/Animation/KinetixAnimation.h"

// Sets default values for this component's properties
UKinetixComponent::UKinetixComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	bRegisterPlayerOnLaunch = false;
}


// Called when the game starts
void UKinetixComponent::BeginPlay()
{
	Super::BeginPlay();

	CheckSkeletalMeshComponent();
}

void UKinetixComponent::CheckSkeletalMeshComponent()
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
		                                              &UKinetixComponent::CheckSkeletalMeshComponent, 0.1f, true);
		return;
	}

	if (!OwnerSkeletalMeshComponent->IsAnimBlueprintInstanced())
	{
		OwnerSkeletalMeshComponent->OnAnimInitialized.AddDynamic(this, &UKinetixComponent::OnOwnerAnimationInitialized);
		return;
	}

	OnOwnerAnimationInitialized();
}

void UKinetixComponent::SetSkeletalMeshComponent(USkeletalMeshComponent* InSkeletalMeshComponent)
{
	OwnerSkeletalMeshComponent = InSkeletalMeshComponent;
}

void UKinetixComponent::OnOwnerAnimationInitialized()
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

FString UKinetixComponent::RemapBones(const int32 NodeIndex, const FString& CurveName, const FString& Path,
                                      UObject* Context)
{
	return CurveName;
}

void UKinetixComponent::LoadAnimationAndPlay(const FString& Url)
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
