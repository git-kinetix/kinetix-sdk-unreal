// Copyright Kinetix. All Rights Reserved.


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

#include "KinetixRuntimeModule.h"
#include "UObject/SavePackage.h"

#pragma region SmartCache

// #include <IKinanimInterface.h>
// #include <KinanimWrapper.h>

#include "MemoryManager.h"
#include "Kismet/GameplayStatics.h"
#include "SmartCache/KinetixCacheSaveGame.h"

#pragma endregion


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

void FEmoteManager::SetCurveRemapper(const FglTFRuntimeAnimationCurveRemapper& InRemapper)
{
	CurveRemapper = InRemapper;
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

void FEmoteManager::SetReferenceSkeleton(USkeletalMesh* SkeletalMesh)
{
	if (!IsValid(SkeletalMesh))
	{
		UE_LOG(LogKinetixAnimation, Warning, TEXT("[EmoteManager] SetReferenceSkeleton: Given skeleton is null !"))
		return;
	}

	ReferenceSkeletalMesh = SkeletalMesh;
}

USkeletalMesh* FEmoteManager::GetReferenceSkeleton() const
{
	return ReferenceSkeletalMesh;
}

bool FEmoteManager::GetAnimSequenceFromGltfAsset(const FKinetixEmote* InEmote, UglTFRuntimeAsset* LoadedGltfAsset)
{
	FglTFRuntimeSkeletalAnimationConfig SkeletalAnimConfig;
	SkeletalAnimConfig.bRemoveMorphTargets = true;

	if (CurveRemapper.IsBound())
	{
		SkeletalAnimConfig.CurveRemapper.Remapper = CurveRemapper;
	}

	USkeletalMesh* RefSkeletalMesh = GetReferenceSkeleton();
	if (!IsValid(RefSkeletalMesh))
	{
		UE_LOG(LogKinetixAnimation, Warning,
		       TEXT("[FAccount] AnimationRequestComplete(): RefSekeletalMesh is null !"));
		return true;
	}
	UAnimSequence* AnimSequence = LoadedGltfAsset->LoadSkeletalAnimation(RefSkeletalMesh, 0,
	                                                                     SkeletalAnimConfig);

	if (!IsValid(AnimSequence))
	{
		UE_LOG(LogKinetixAnimation, Error,
		       TEXT("[FAccount] AnimationRequestComplete(): AnimSequence is null !"));
		return true;
	}

	FKinetixEmote* Emote = GetEmote(InEmote->GetAnimationMetadata().Id);
	if (Emote != nullptr)
	{
		Emote->SetAnimSequence(AnimSequence);
		return true;
	}
	return false;
}

void FEmoteManager::LoadAnimation(const FKinetixEmote* InEmote,
                                  const TDelegate<void()>& OnOperationFinished,
                                  FString AvatarUUID)
{
	const UKinetixCacheSaveGame& Manifest = FMemoryManager::Get().GetManifest();
	if (Manifest.Contains(InEmote->GetAnimationMetadata().Id))
	{
		FString Path;
		UKinetixDataBlueprintFunctionLibrary::GetCacheAnimationPath(Path,
		                                                            InEmote->GetAnimationMetadata().Id);

		FglTFRuntimeConfig RuntimeConfig;

		// Setup for animation V1
		// RuntimeConfig.TransformBaseType = EglTFRuntimeTransformBaseType::YForward;

		// Setup for animation V2
		RuntimeConfig.TransformBaseType = EglTFRuntimeTransformBaseType::Transform;
		RuntimeConfig.BaseTransform.SetRotation(FRotator(0.f, 180.f, 0.f).Quaternion());
		RuntimeConfig.BaseTransform.SetScale3D(FVector(-1.f, 1.f, 1.f));

		// FglTFRuntimeHttpResponse GltfHttpResponse;
		
		// GltfHttpResponse.BindUFunction();
		UglTFRuntimeAsset* LoadedGltfAsset =
			UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilename(
				Path, false, RuntimeConfig);

		if (LoadedGltfAsset == nullptr)
		{
			UE_LOG(LogKinetixAnimation,
			       Error,
			       TEXT("[FEmoteManager] LoadAnimation(): Loaded .glb file failed !"))
		}

		if (GetAnimSequenceFromGltfAsset(InEmote, LoadedGltfAsset))
		{
			OnOperationFinished.ExecuteIfBound();
		}

		return;
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest =
		FHttpModule::Get().CreateRequest();

	bool bAvatarIDFound = false;
	if (!AvatarUUID.IsEmpty()
		&& !InEmote->GetAnimationMetadata().AvatarMetadatas.IsEmpty())
	{
		for (int i = 0; i < InEmote->GetAnimationMetadata().AvatarMetadatas.Num(); ++i)
		{
			if (InEmote->GetAnimationMetadata().AvatarMetadatas[i].AvatarID.ToString(
				EGuidFormats::DigitsWithHyphensLower) != AvatarUUID)
				continue;

			// AvatarID found
			HttpRequest->SetURL(InEmote->GetAnimationMetadata().AvatarMetadatas[i].AvatarURL.Map);
			i = InEmote->GetAnimationMetadata().AvatarMetadatas.Num();
			bAvatarIDFound = true;
		}

		if (!bAvatarIDFound)
		{
			UE_LOG(LogKinetixAnimation, Error,
			       TEXT("[EmoteManager] LoadAnimation(): Given AvatarID not in emote metadata's avatars"));
			return;
		}
	}
	else
	{
		HttpRequest->SetURL(InEmote->GetAnimationMetadata().AnimationURL.Map);
	}

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

#pragma region Kinanim implementation


	// const TArray<uint8> JsonContent = HttpResponse->GetContent();
	//
	// const char* Datas = reinterpret_cast<const char*>(JsonContent.GetData());
	// void* BinaryStream = Kinanim::CreateBinaryStreamFromArray(Datas, JsonContent.Num());
	// if (BinaryStream == nullptr)
	// {
	// 	UE_LOG(LogKinetixAnimation, Error,
	// 	       TEXT("[FEmoteManager] AnimationRequestComplete(): Unable to create binary stream ! %s"),
	// 	       *JsonString);
	// 	return;
	// }
	//
	// void* InterpoCompression = KinanimWrapper::Ctor_InterpoCompression();
	// if (InterpoCompression == nullptr)
	// {
	// 	UE_LOG(LogKinetixAnimation, Error,
	// 	       TEXT("[FEmoteManager] AnimationRequestComplete(): Unable to create InterpoCompression ! %s"),
	// 	       *JsonString);
	// 	return;
	// }
	//
	// void* KinanimImporter = KinanimWrapper::Ctor_KinanimImporter(InterpoCompression);
	// if (KinanimImporter == nullptr)
	// {
	// 	UE_LOG(LogKinetixAnimation, Error,
	// 	       TEXT("[FEmoteManager] AnimationRequestComplete(): Unable to create KinanimImporter ! %s"),
	// 	       *JsonString);
	// 	return;
	// }
	//
	// KinanimWrapper::KinanimImporter_ReadFile(KinanimImporter, BinaryStream);
	//
	// void* KinanimResult = KinanimWrapper::KinanimImporter_GetResult(KinanimImporter);
	// if (KinanimResult == nullptr)
	// {
	// 	UE_LOG(LogKinetixAnimation, Error,
	// 	       TEXT("[FEmoteManager] AnimationRequestComplete(): Unable to get KinanimResult ! %s"),
	// 	       *JsonString);
	// 	return;
	// }
	//
	// unsigned short FrameCount = KinanimWrapper::KinanimHeader_GetFrameCount(
	// 	KinanimWrapper::KinanimData_Get_header(KinanimResult));
	//
	// if (FrameCount == 0)
	// {
	// 	UE_LOG(LogKinetixAnimation, Error,
	// 	       TEXT("[FEmoteManager] AnimationRequestComplete(): Unable to get FrameCount ! %s"),
	// 	       *JsonString);
	// 	return;
	// }
	//
	// TArray<FrameData> Frames;
	// Frames.SetNumUninitialized(FrameCount);
	// void* Content = KinanimWrapper::KinanimData_Get_content(KinanimResult);
	// for (unsigned int i = 0; i < FrameCount; ++i)
	// {
	// 	Frames[i] = KinanimWrapper::KinanimContent_Get_frames(Content, i);
	// }

#pragma endregion

	TArray<TSharedPtr<FJsonValue>> JsonArray;
	const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

	FglTFRuntimeConfig RuntimeConfig;

	// Setup for animation V1
	// RuntimeConfig.TransformBaseType = EglTFRuntimeTransformBaseType::YForward;

	// Setup for animation V2
	RuntimeConfig.TransformBaseType = EglTFRuntimeTransformBaseType::Transform;
	RuntimeConfig.BaseTransform.SetRotation(FRotator(0.f, 180.f, 0.f).Quaternion());
	RuntimeConfig.BaseTransform.SetScale3D(FVector(-1.f, 1.f, 1.f));

	UglTFRuntimeAsset* GlTFAsset =
		UglTFRuntimeFunctionLibrary::glTFLoadAssetFromData(
			HttpResponse->GetContent(),
			RuntimeConfig);

	if (!IsValid(GlTFAsset))
	{
		UE_LOG(LogKinetixAnimation, Warning,
		       TEXT("[FEmoteManager] AnimationRequestComplete(): Unable to deserialize response ! %s"),
		       *JsonString);
		return;
	}

	FglTFRuntimeSkeletalAnimationConfig SkeletalAnimConfig;
	if (CurveRemapper.IsBound())
	{
		SkeletalAnimConfig.CurveRemapper.Remapper = CurveRemapper;
	}
	USkeletalMesh* RefSkeletalMesh = GetReferenceSkeleton();
	if (!IsValid(RefSkeletalMesh))
	{
		UE_LOG(LogKinetixAnimation, Warning,
		       TEXT("[FEmoteManager] AnimationRequestComplete(): RefSekeletalMesh is null !"));
		return;
	}

	UAnimSequence* AnimSequence = GlTFAsset->LoadSkeletalAnimation(RefSkeletalMesh, 0,
	                                                               SkeletalAnimConfig);

#pragma region Saving file

	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	FString Path;
	UKinetixDataBlueprintFunctionLibrary::GetCacheAnimationPath(Path, InAnimationMetadata.Id);

	if (!FPaths::FileExists(Path))
	{
		if (!FFileHelper::SaveArrayToFile(HttpResponse->GetContent(), *Path))
		{
			UE_LOG(LogKinetixAnimation,
			       Error,
			       TEXT("[FEmoteManager] AnimationRequestComplete(): Saving .glb file failed !"))
		}
	}

#pragma endregion

#pragma region Loading file into asset

	UglTFRuntimeAsset* LoadedGltfAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilename(
		Path, false, RuntimeConfig);

	if (LoadedGltfAsset == nullptr)
	{
		UE_LOG(LogKinetixAnimation,
		       Error,
		       TEXT("[FEmoteManager] AnimationRequestComplete(): Loaded .glb file failed !"))
	}

#pragma endregion

	if (!IsValid(AnimSequence))
	{
		UE_LOG(LogKinetixAnimation, Error,
		       TEXT("[FAccount] AnimationRequestComplete(): AnimSequence is null !"));
		return;
	}

	FKinetixEmote* Emote = GetEmote(InAnimationMetadata.Id);
	if (Emote != nullptr)
	{
		Emote->SetAnimSequence(AnimSequence);

		OnSuccessDelegate.ExecuteIfBound();

		FMemoryManager::Get().OnAnimationLoadedOnPlayer(InAnimationMetadata.Id);
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
