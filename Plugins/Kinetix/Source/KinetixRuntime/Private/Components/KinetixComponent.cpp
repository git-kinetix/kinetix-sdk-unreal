// Copyright Kinetix. All Rights Reserved.


#include "Components/KinetixComponent.h"

#include "Components/SkeletalMeshComponent.h"

#include "glTFRuntimeFunctionLibrary.h"

#include "Kismet/KismetSystemLibrary.h"

// For the logs
#include "KinetixRuntimeModule.h"

// Sets default values for this component's properties
UKinetixComponent::UKinetixComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	Url.Empty();
	bPlayAnimationOnLaunch = false;
}


// Called when the game starts
void UKinetixComponent::BeginPlay()
{
	Super::BeginPlay();

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
		CurrentOwner->GetWorldTimerManager().SetTimer(CheckSkeletalMeshTimer, this,
		                                              &UKinetixComponent::CheckSkeletalMeshComponent, 0.1f, true);
	}
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
}

void UKinetixComponent::SetSkeletalMeshComponent(USkeletalMeshComponent* InSkeletalMeshComponent)
{
	OwnerSkeletalMeshComponent = InSkeletalMeshComponent;
}

void UKinetixComponent::OnOwnerAnimationInitialized()
{
	UKismetSystemLibrary::PrintString(this,
	                                  FString::Printf(TEXT("%s OnOwnerAnimationInitialized"), *GetName()));
}

FString UKinetixComponent::RemapBones(const int32 NodeIndex, const FString& CurveName, const FString& Path,
                                   UObject* Context)
{
	return CurveName;
}

void UKinetixComponent::LoadAnimationAndPlay()
{
	if (Url.IsEmpty())
	{
		UKismetSystemLibrary::PrintString(
			this, FString::Printf(TEXT("%s LoadAnimationAndPlay: Url is empty ! Skipping..."), *GetOwner()->GetName()),
			true, true, FLinearColor::Yellow, 5.f);;
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
