// // Copyright Kinetix. All Rights Reserved.


#include "Managers/EmoteManager.h"

#include "glTFRuntimeAsset.h"
#include "glTFRuntimeFunctionLibrary.h"
#include "glTFRuntimeParser.h"
#include "HttpModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Core/Animation/KinetixAnimation.h"
#include "Engine/AssetManager.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

// Otherwise there is no exports of static symbols
TUniquePtr<FEmoteManager> FEmoteManager::Instance(nullptr);

FEmoteManager::FEmoteManager()
	: ReferenceSkeletalMesh(nullptr)
{
}

FEmoteManager::~FEmoteManager()
{
	if (IsValid(ReferenceSkeletalMesh))
	{
		ReferenceSkeletalMesh->RemoveFromRoot();
	}
	Instance = nullptr;
}

FKinetixEmote* FEmoteManager::GetEmote(const FAnimationID& InAnimationID)
{
	if (!InAnimationID.UUID.IsValid())
		return nullptr;

	if (KinetixEmotes.Contains(InAnimationID))
		return &KinetixEmotes[InAnimationID];

	return &KinetixEmotes.Add(InAnimationID, FKinetixEmote(InAnimationID));
}

TArray<FKinetixEmote*> FEmoteManager::GetEmotes(TArray<FAnimationID>& InAnimationIDs)
{
	if (InAnimationIDs.IsEmpty())
		return TArray<FKinetixEmote*>();

	TArray<FKinetixEmote*> Emotes;
	Emotes.Reserve(InAnimationIDs.Num());

	for (int InAnimationIDIndex = 0; InAnimationIDIndex < InAnimationIDs.Num(); ++InAnimationIDIndex)
	{
		Emotes[InAnimationIDIndex] = GetEmote(InAnimationIDs[InAnimationIDIndex]);
	}
	return Emotes;
}

TArray<FKinetixEmote*> FEmoteManager::GetAllEmotes()
{
	return TArray<FKinetixEmote*>();
}

void FEmoteManager::GetAnimSequence(const FAnimationID& InAnimationID, const TDelegate<void(UAnimSequence*)>& OnSuccess)
{
	FAssetRegistryModule& AssetRegistryModule =
		FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	FKinetixEmote* Emote = GetEmote(InAnimationID);
	if (Emote == nullptr)
		return; // Failed on this case

	if (!Emote->IsLocal())
		return;

	OnSuccess.ExecuteIfBound(Emote->GetAnimSequence());

	FString PackagePath = Emote->GetPathToGlb();
	FPaths::MakePathRelativeTo(PackagePath, *FPaths::ProjectPluginsDir());
	UKinetixDataBlueprintFunctionLibrary::RemoveContentFromPluginPath(PackagePath, TEXT("/Kinetix"));
	PackagePath = FString::Printf(TEXT("/%s"), *PackagePath);

	FARFilter Filter;
	Filter.ClassPaths.Add(UAnimSequence::StaticClass()->GetClassPathName());
	Filter.PackagePaths.Add(FName(PackagePath));
	Filter.bRecursivePaths = true;

	TArray<FAssetData> AssetDatas;
	if (!AssetRegistryModule.Get().GetAssets(Filter, AssetDatas))
		return;

	if (AssetDatas.Num() != 1)
		return;

	FStreamableDelegate Delegate = FStreamableDelegate::CreateRaw(
		this,
		&FEmoteManager::AnimSequenceAvailable,
		AssetDatas[0].ToSoftObjectPath(),
		OnSuccess);

	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		TArray{AssetDatas[0].ToSoftObjectPath()},
		Delegate
	);
}

void FEmoteManager::AnimSequenceAvailable(FSoftObjectPath SoftObjectPath,
                                          TDelegate<void(UAnimSequence*)> Delegate) const
{
	if (!SoftObjectPath.IsValid())
		return;

	UAnimSequence* AnimSequence = Cast<UAnimSequence>(SoftObjectPath.ResolveObject());
	if (!IsValid(AnimSequence))
		return;

	Delegate.Execute(AnimSequence);
}

FEmoteManager& FEmoteManager::Get()
{
	if (!Instance.IsValid())
	{
		Instance = MakeUnique<FEmoteManager>();
		Instance->RefSkeletonLoadedDelegate = TDelegate<void(FAssetData)>::CreateRaw(
			Instance.Get(), &FEmoteManager::OnReferenceSkeletonAvailable);
		UKinetixDataBlueprintFunctionLibrary::LoadReferenceSkeletonAsset(Instance->RefSkeletonLoadedDelegate);
	}
	return *Instance;
}

USkeletalMesh* FEmoteManager::GetReferenceSkeleton() const
{
	return ReferenceSkeletalMesh;
}

void FEmoteManager::LoadAnimation(const FKinetixEmote* InEmote,
                                  const TDelegate<void()>& OnOperationFinished)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest =
		FHttpModule::Get().CreateRequest();

	HttpRequest->SetURL(InEmote->GetAnimationMetadata().AnimationURL.Map);
	HttpRequest->OnProcessRequestComplete().BindRaw(
		this, &FEmoteManager::AnimationRequestComplete,
		InEmote->GetAnimationMetadata(), OnOperationFinished);

	if (!HttpRequest->ProcessRequest())
	{
		UE_LOG(LogKinetixAnimation, Warning,
		       TEXT("[EmoteManager] LoadAnimation: Unable to process animation request !"));
	}
}

void FEmoteManager::AnimationRequestComplete(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest,
                                             TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse, bool bSuccess,
                                             FAnimationMetadata InAnimationMetadata,
                                             TDelegate<void()> OnSuccessDelegate)
{
	if (!bSuccess
		|| !HttpResponse.IsValid()
		|| !EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		return;

	FString JsonString = HttpResponse->GetContentAsString();

	TArray<TSharedPtr<FJsonValue>> JsonArray;
	const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

	FglTFRuntimeConfig RuntimeConfig;
	RuntimeConfig.TransformBaseType = EglTFRuntimeTransformBaseType::YForward;
	UglTFRuntimeAsset* GlTFAsset =
		UglTFRuntimeFunctionLibrary::glTFLoadAssetFromData(
			HttpResponse->GetContent(),
			RuntimeConfig);

	if (!IsValid(GlTFAsset))
	{
		UE_LOG(LogKinetixAnimation, Warning,
		       TEXT("[FAccount] AnimationRequestComplete(): Unable to deserialize response ! %s"),
		       *JsonString);
		return;
	}

	FglTFRuntimeSkeletalAnimationConfig SkeletalAnimConfig;
	USkeletalMesh* RefSkeletalMesh = GetReferenceSkeleton();

	UAnimSequence* AnimSequence = GlTFAsset->LoadSkeletalAnimation(RefSkeletalMesh, 0,
	                                                               SkeletalAnimConfig);

	if (!IsValid(AnimSequence))
	{
		UE_LOG(LogKinetixAnimation, Warning,
		       TEXT("[FAccount] AnimationRequestComplete(): AnimSequence is null !"));
		return;
	}

	FKinetixEmote* Emote = GetEmote(InAnimationMetadata.Id);
	if (Emote != nullptr)
	{
		Emote->SetAnimSequence(AnimSequence);
		OnSuccessDelegate.ExecuteIfBound();
	}
}

void FEmoteManager::OnReferenceSkeletonAvailable(FAssetData AssetData)
{
	if (!AssetData.IsValid())
	{
		UE_LOG(LogKinetixAnimation, Warning,
		       TEXT("[EmoteManager] OnReferenceSkeletonAvailable: AssetData not valid !"));
		return;
	}

	ReferenceSkeletalMesh = Cast<USkeletalMesh>(AssetData.GetAsset());
	if (!IsValid(ReferenceSkeletalMesh))
	{
		UE_LOG(LogKinetixAnimation, Warning,
		       TEXT("[EmoteManager] OnReferenceSkeletonAvailable: ReferenceSkeletalMesh not valid !"));
		return;
	}

	// Prevent of being GC
	ReferenceSkeletalMesh->AddToRoot();
}
