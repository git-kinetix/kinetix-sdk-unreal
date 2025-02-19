// Copyright Kinetix. All Rights Reserved.

#include "Components/AnimSequenceSamplerComponent.h"

#include "KinetixDeveloperSettings.h"
#include "KinetixRuntimeModule.h"
#include "Animation/AnimationPoseData.h"
#include "AnimInstances/KinetixAnimInstance.h"
#include "Components/KinetixCharacterComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Core/KinetixCoreSubsystem.h"
#include "Core/Animation/KinetixAnimation.h"
#include "Core/Metadata/KinetixMetadata.h"
#include "Core/Network/KinetixNetworkedPose.h"

UAnimSequenceSamplerComponent::UAnimSequenceSamplerComponent(const FObjectInitializer& ObjectInitializer)
	: LastTickFrame(0), AnimSequenceToPlay(nullptr),
	  SkeletalMeshComponentToPause(nullptr), KCC(nullptr),
	  Duration(0), Time(0), AnimIndex(0),
	  bAnimInstanceWasEnabled(false),
	  BlendState(EBlendState::BS_None),
	  CachedAnimationMode(),
	  BlendTime(0.f)
{
	TimeSinceLastNetUpdate = 0.f;
	TimeBetweenNetUpdates = 1.f / 30.f;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
}

// Sets default values for this component's properties
UAnimSequenceSamplerComponent::UAnimSequenceSamplerComponent(UActorComponent* ActorComponent)
	: LastTickFrame(0), AnimSequenceToPlay(nullptr),
	  SkeletalMeshComponentToPause(nullptr), KCC(nullptr),
	  Duration(0), Time(0), AnimIndex(0),
	  bAnimInstanceWasEnabled(false),
	  BlendState(EBlendState::BS_None),
	  CachedAnimationMode(),
	  BlendTime(0.f), CachePose()
{
	if (!ensure(IsValid(ActorComponent))) return;

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
}

void UAnimSequenceSamplerComponent::BeginPlay()
{
	Super::BeginPlay();

	auto CurrentOwner = GetOwner();
	if (!IsValid(CurrentOwner))
		return;

	KCC = CurrentOwner->GetComponentByClass<UKinetixCharacterComponent>();
	if (!IsValid(KCC))
	{
		if (UKinetixDeveloperSettings::GetLogFlag())
		UE_LOG(LogKinetixRuntime,
		       Warning,
		       TEXT(
			       "[AnimSequenceSamplerComponent] BeginPlay: Trying to use AnimSequenceSamplerComponent without KinetixCharacterComponent !"
		       ));
	}

	KCC->RegisterSampler(this);

	AnimInstanceToNotify = KCC->GetAnimInstanceToNotify();

	SetContext(CurrentOwner);
}

void UAnimSequenceSamplerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(AnimSequenceToPlay))
		return;

	Time += DeltaTime;
	Context.CurrentTime = Time;

	if (AnimSequenceToPlay->GetPlayLength() <= Time)
		return;

	// Needed to avoid crash
	FMemMark Mark(FMemStack::Get());

	// CompactPose is in bone space, so CSPose is here to get it from bone space -> component space
	FCSPose<FCompactPose> CSPose;
	FCompactPose OutPose;

	OutPose.ResetToRefPose(RequiredBones);
	FAnimationPoseData TickPose(OutPose, Curve, Attributes);
	AnimSequenceToPlay->GetAnimationPose(TickPose, Context);
	CSPose.InitPose(MoveTemp(OutPose));

	// ReSharper disable once CppJoinDeclarationAndAssignment
	FTransform BoneTransform;
	for (int i = 0; i < BoneNames.Num(); ++i)
	{
		BoneTransform = CSPose.GetComponentSpaceTransform(FCompactPoseBoneIndex(i));
		CachePose.Bones[i].LocalPosition = BoneTransform.GetLocation();
		CachePose.Bones[i].LocalQuaternion = BoneTransform.GetRotation();
		CachePose.Bones[i].LocalScale = BoneTransform.GetScale3D();
	}

	TimeSinceLastNetUpdate += DeltaTime;
	if (TimeSinceLastNetUpdate < TimeBetweenNetUpdates)
		return;

	ServerSendFramePose(CachePose);
	TimeSinceLastNetUpdate = 0.f;
}

void UAnimSequenceSamplerComponent::SetAnimatorWasEnabled(const bool Value)
{
	bAnimInstanceWasEnabled = Value;
}

bool UAnimSequenceSamplerComponent::GetAnimInstanceWasEnabled() const
{
	return bAnimInstanceWasEnabled;
}

void UAnimSequenceSamplerComponent::Play(UAnimSequence* InAnimSequence, const FAnimationID& InAnimationID)
{
	FAnimationQueue NewAnimQueue;
	NewAnimQueue.AnimationIDs.Add(InAnimationID);
	NewAnimQueue.AnimationSequences.Add(InAnimSequence);

	Play(NewAnimQueue);
}

void UAnimSequenceSamplerComponent::Play(FAnimationQueue& InAnimQueue)
{
	if (!AnimQueue.AnimationIDs.IsEmpty())
	{
		OnAnimationEnd.ExecuteIfBound(AnimQueue.AnimationIDs[AnimIndex]);
	}

	if (InAnimQueue.AnimationSequences.Num() != 0)
	{
		AnimQueue = InAnimQueue;
		AnimIndex = 0;
		Time = 0;
		// StartAnimation();
		ComputeBlendTime();
	}
}

void UAnimSequenceSamplerComponent::SetContext(const AActor* InActor)
{
	if (!IsValid(InActor))
		return;

	TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
	InActor->GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);

	// We are supposed to find a skeletal mesh source and its target
	if (SkeletalMeshComponents.Num() < 1)
		return;

	UAnimInstance* AnimInstance = nullptr;
	for (int i = 0; i < SkeletalMeshComponents.Num(); ++i)
	{
		AnimInstance = SkeletalMeshComponents[i]->GetAnimInstance();
		if (!IsValid(AnimInstance))
			continue;

		KinetixSkeletalMeshComponentSource = Cast<UKinetixAnimInstance>(
			SkeletalMeshComponents[i]->AnimScriptInstance);
		if (!IsValid(KinetixSkeletalMeshComponentSource))
			continue;

		SkeletalMeshComponents[i]->GetBoneNames(BoneNames);
		BoneTransforms.SetNumZeroed(BoneNames.Num());
		bKinetixSkeletalMeshFound = true;
		i = 255;
	}
}

void UAnimSequenceSamplerComponent::PlayAnimation_Implementation(const FAnimationID& InID, const FString& AvatarID, UAnimSequence* InAnimSequence)
{
	if (!IsValid(InAnimSequence))
		return;

	if (!GetDefault<UKinetixDeveloperSettings>()->bSendPose)
	{
		SendServerAnimationID(InID, AvatarID);
		return;
	}

	if (!IsValid(KinetixSkeletalMeshComponentSource))
		return;

	AnimSequenceToPlay = InAnimSequence;

	RequiredBones =
		KinetixSkeletalMeshComponentSource->GetRequiredBones();

	Context.CurrentTime = 0.f;
	for (int i = 0; i < RequiredBones.GetNumBones(); ++i)
	{
		Context.BonesRequired.Add(true);
	}

	CachePose.Guid = FGuid::NewGuid();
	CachePose.bHasArmature = true;
	CachePose.bHasBlendshapes = false;
	CachePose.bPosEnabled = true;
	CachePose.bScaleEnabled = false;
	CachePose.TimeStamp = FApp::GetCurrentTime();
	CachePose.Bones.SetNumZeroed(RequiredBones.GetNumBones());

	SetComponentTickEnabled(true);

	Time = 0.f;

}

void UAnimSequenceSamplerComponent::StopAnimation_Implementation()
{
	IKinetixSamplerInterface::StopAnimation_Implementation();

	SetComponentTickEnabled(false);

	AnimSequenceToPlay = nullptr;
	Time = 0.f;

	ServerSendStopAnimation();
}

void UAnimSequenceSamplerComponent::EnableAnimInstance()
{
	if (!IsValid(AnimSequenceToPlay))
		return;

	SkeletalMeshComponentToPause->SetAnimationMode(CachedAnimationMode);
}

void UAnimSequenceSamplerComponent::DisableAnimInstance()
{
	CachedAnimationMode = SkeletalMeshComponentToPause->GetAnimationMode();
	SkeletalMeshComponentToPause->Stop();
	SkeletalMeshComponentToPause->SetAnimationMode(EAnimationMode::AnimationCustomMode);
}

void UAnimSequenceSamplerComponent::SendServerAnimationID_Implementation(FAnimationID InID, const FString& AvatarID)
{
	AllDispatchPlayedAnimationID(InID, AvatarID);
}

bool UAnimSequenceSamplerComponent::ServerSendFramePose_Validate(FKinetixNetworkedPose NetworkedPose)
{
	if (NetworkedPose.Bones.Num() != BoneNames.Num())
	{
		if (UKinetixDeveloperSettings::GetLogFlag())
		UE_LOG(LogKinetixRuntime,
		       Warning,
		       TEXT(
			       "[AnimSequenceSampler] ServerSendFramePose: Mismatch between local bone mapping (%i bones) and received bone mapping (%i bones)!"
		       ), BoneNames.Num(), NetworkedPose.Bones.Num());
		return false;
	}

	return true;
}

void UAnimSequenceSamplerComponent::ServerSendFramePose_Implementation(FKinetixNetworkedPose NetworkedPose)
{
	if (!bKinetixSkeletalMeshFound)
		return;

	OnServerReceivedPose.Broadcast(NetworkedPose, this);

	AllDispatchPose(NetworkedPose);
}

void UAnimSequenceSamplerComponent::AllDispatchPose_Implementation(FKinetixNetworkedPose NetworkedPose)
{
	if (!bKinetixSkeletalMeshFound)
		return;

	FVector TempVector = FVector::ZeroVector;

	TArray<FName> Copy;
	for (int i = 0; i < BoneNames.Num(); ++i)
	{
		TempVector = NetworkedPose.Bones[i].LocalPosition.ToFVector();
		BoneTransforms[i].SetLocation(TempVector);
		BoneTransforms[i].SetRotation(NetworkedPose.Bones[i].LocalQuaternion.ToQuat());
		BoneTransforms[i].SetScale3D(NetworkedPose.Bones[i].LocalScale.ToFVector());
	}

	KinetixSkeletalMeshComponentSource->NativeSendNetworkedPose(BoneNames, BoneTransforms);
	if (!IsValid(AnimInstanceToNotify.GetObject()))
		return;

	if (AnimInstanceToNotify->Execute_IsKinetixAnimationPlaying(AnimInstanceToNotify.GetObject()))
		return;

	AnimInstanceToNotify->Execute_SetKinetixAnimationPlaying(
		AnimInstanceToNotify.GetObject(), true);
}

void UAnimSequenceSamplerComponent::AllDispatchPlayedAnimationID_Implementation(FAnimationID InID, const FString& AvatarID)
{
	if (GetOwnerRole() != ENetRole::ROLE_SimulatedProxy)
		return;

	UWorld* CurrentWorld = GetWorld();
	if (!IsValid(CurrentWorld))
		return;

	UGameInstance* CurrentGI = CurrentWorld->GetGameInstance();
	if (!IsValid(CurrentGI))
		return;

	UKinetixCoreSubsystem* KinetixCore = CurrentGI->GetSubsystem<UKinetixCoreSubsystem>();
	if (!IsValid(KinetixCore))
		return;

	FOnMetadataAvailable OnMetadataAvailable;
	OnMetadataAvailable.BindUFunction(this, TEXT("OnMetadataAvailable"));
	CacheAvatarID = AvatarID;
	KinetixCore->KinetixMetadata->GetAnimationMetadataByAnimationID(InID, OnMetadataAvailable);
}

void UAnimSequenceSamplerComponent::OnMetadataAvailable(bool bSuccess, const FAnimationMetadata& AnimationMetadata)
{
	FString InLockID;
	FOnKinetixLocalAnimationLoadingFinished AnimationLoadingFinished;
	AnimationLoadingFinished.BindUFunction(this, TEXT("OnAnimationLoadingFinished"));
	RemoteAnimationIDToPlay = AnimationMetadata.Id;

	UWorld* CurrentWorld = GetWorld();
	if (!IsValid(CurrentWorld))
		return;

	UGameInstance* CurrentGI = CurrentWorld->GetGameInstance();
	if (!IsValid(CurrentGI))
		return;

	UKinetixCoreSubsystem* KinetixCore = CurrentGI->GetSubsystem<UKinetixCoreSubsystem>();
	if (!IsValid(KinetixCore))
		return;

	KinetixCore->KinetixAnimation->LoadLocalPlayerAnimation(AnimationMetadata.Id, InLockID, CacheAvatarID,
	                                                        AnimationLoadingFinished);
}

void UAnimSequenceSamplerComponent::OnAnimationLaunchedOnServer(const FAnimationID& AnimationID)
{
}

void UAnimSequenceSamplerComponent::PlayAnimationOnComponent(FAnimationID InID)
{
	AActor* CurrentOwner = GetOwner();
	if (!IsValid(CurrentOwner))
		return;

	UKinetixCharacterComponent* KinetixCharacterComponent = CurrentOwner->GetComponentByClass<UKinetixCharacterComponent>();
	if (!IsValid(KinetixCharacterComponent))
		return;

	KinetixCharacterComponent->PlayAnimationInternal(InID, false,
	                           TDelegate<void(const FAnimationID&)>::CreateUObject(
		                           this, &UAnimSequenceSamplerComponent::OnAnimationLaunchedOnServer));
}

void UAnimSequenceSamplerComponent::OnAnimationLoadingFinished(bool bSuccess)
{
	if (!bSuccess)
		return;

	PlayAnimationOnComponent(RemoteAnimationIDToPlay);
	RemoteAnimationIDToPlay.UUID.Invalidate();
}

void UAnimSequenceSamplerComponent::KinetixStartAnimation(const FAnimationID& AnimationID)
{
}

void UAnimSequenceSamplerComponent::ServerSendStopAnimation_Implementation()
{
	AllSendStopAnimation();
}

void UAnimSequenceSamplerComponent::AllSendStopAnimation_Implementation()
{
	KinetixSkeletalMeshComponentSource->NativeSetNetworkMode(false);

	if (!IsValid(AnimInstanceToNotify.GetObject()))
		return;
	AnimInstanceToNotify->Execute_SetKinetixAnimationPlaying(
		AnimInstanceToNotify.GetObject(), false);
}

void UAnimSequenceSamplerComponent::OnBlendInEnded()
{
}

void UAnimSequenceSamplerComponent::OnBlendOutEnded()
{
}

void UAnimSequenceSamplerComponent::ComputeBlendTime()
{
}

void UAnimSequenceSamplerComponent::ForceBlendOut()
{
}

void UAnimSequenceSamplerComponent::NextAnimation()
{
}
