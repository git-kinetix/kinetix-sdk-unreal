// Copyright Kinetix. All Rights Reserved.


#include "Managers/EmoteManager.h"

#include "glTFRuntimeAsset.h"
#include "glTFRuntimeFunctionLibrary.h"
#include "glTFRuntimeParser.h"
#include "HttpModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Core/Animation/KinetixAnimation.h"
#include "Engine/AssetManager.h"
#include "Interfaces/IHttpResponse.h"

#include "KinanimBonesDataAsset.h"

#include "KinetixRuntimeModule.h"
#include "UObject/SavePackage.h"

#pragma region SmartCache

#include "IKinanimInterface.h"
// #include <KinanimWrapper.h>

#include "KinanimWrapper.h"
#include "KinetixDeveloperSettings.h"
#include "MemoryManager.h"
#include "KinanimWrapper/KinanimParser.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/BufferArchive.h"
#include "SmartCache/KinetixCacheSaveGame.h"

#pragma endregion


// Otherwise there is no exports of static symbols
TUniquePtr<FEmoteManager> FEmoteManager::Instance(nullptr);

FEmoteManager::FEmoteManager()
	: KinanimBoneMapping(nullptr), ReferenceSkeletalMesh(nullptr)
{
}

FEmoteManager::~FEmoteManager()
{
	// if (IsValid(ReferenceSkeletalMesh))
	// {
	// 	ReferenceSkeletalMesh->RemoveFromRoot();
	// }
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

void FEmoteManager::SetBoneMapping(const UKinanimBonesDataAsset* InKinanimBoneMapping)
{
	KinanimBoneMapping = InKinanimBoneMapping;
}

FEmoteManager& FEmoteManager::Get()
{
	if (!Instance.IsValid())
	{
		Instance = MakeUnique<FEmoteManager>();
		Instance->RefSkeletonLoadedDelegate = TDelegate<void(FAssetData)>::CreateRaw(
			Instance.Get(), &FEmoteManager::OnReferenceSkeletonAvailable);
		UKinetixDataBlueprintFunctionLibrary::LoadReferenceSkeletonAsset(Instance->RefSkeletonLoadedDelegate);
		UKinetixDeveloperSettings::GetBlendshapeFlags(Instance->bBlendshapesEnabled);
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

void FEmoteManager::LoadBoneMapping(
	const FKinetixEmote* InEmote, const TDelegate<void()>& OnOperationFinished, FString AvatarUUID)
{
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
			HttpRequest->SetURL(InEmote->GetAnimationMetadata().AvatarMetadatas[i].MappingURL.Map);
			HttpRequest->SetHeader(TEXT("User-Agent"), SDKUSERAGENT);

			i = InEmote->GetAnimationMetadata().AvatarMetadatas.Num();
			bAvatarIDFound = true;
		}

		if (!bAvatarIDFound)
		{
			UE_LOG(LogKinetixAnimation, Error,
			       TEXT("[EmoteManager] LoadMapping(): Given AvatarID not in emote metadata's avatars"));
			return;
		}
	}

	HttpRequest->OnProcessRequestComplete().BindRaw(
		this, &FEmoteManager::MappingRequestComplete,
		InEmote->GetAnimationMetadata(), AvatarUUID, OnOperationFinished);

	//HttpRequest->OnHeaderReceived().BindRaw(this, &FEmoteManager::AnimationHeaderReceived);
	//HttpRequest->OnRequestProgress().BindRaw(this, &FEmoteManager::AnimationRequestProgress);

	if (!HttpRequest->ProcessRequest())
	{
		UE_LOG(LogKinetixAnimation, Warning,
		       TEXT("[EmoteManager] LoadMapping: Unable to process animation request !"));
	}
}

void FEmoteManager::MappingRequestComplete(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> HttpResponse,
                                           bool bSuccess, FAnimationMetadata AnimationMetadata, FString AvatarUUID,
                                           TDelegate<void()> Delegate)
{
	if (!bSuccess
		|| !HttpResponse.IsValid()
		|| !EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		return;

	FString JsonString = HttpResponse->GetContentAsString();

	const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);
	TSharedPtr<FJsonObject> JsonObject;

	const bool bDeserializationResult = FJsonSerializer::Deserialize(JsonReader, JsonObject);
	if (!bDeserializationResult)
	{
		UE_LOG(LogKinetixAnimation,
		       Error, TEXT("[FEmoteManager] MappingRequestComplete: Unable to desrialize Json"));
		return;
	}

	const TSharedPtr<FJsonObject>* JsonValue;
	JsonObject->TryGetObjectField(TEXT("bone_mapping"), JsonValue);

	if (!JsonValue->IsValid())
	{
		UE_LOG(LogKinetixAnimation,
		       Error, TEXT("[FEmoteManager] MappingRequestComplete: Unable to retreive JsonObject"));
		return;
	}

	TArray<FString> MappingKeys;
	JsonValue->Get()->Values.GetKeys(MappingKeys);
	UKinanimBonesDataAsset* NewKinanimBoneMapping = NewObject<UKinanimBonesDataAsset>();
	NewKinanimBoneMapping->AddToRoot();

	FString CacheKey;
	for (int i = 0; i < JsonValue->Get()->Values.Num(); ++i)
	{
		CacheKey = MappingKeys[i];
		if (CacheKey == TEXT("Kinetix_Spine"))
		{
			NewKinanimBoneMapping->Spine = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_Neck"))
		{
			NewKinanimBoneMapping->Neck = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_LeftShoulder"))
		{
			NewKinanimBoneMapping->LeftShoulder = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_RightShoulder"))
		{
			NewKinanimBoneMapping->RightShoulder = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_Hips"))
		{
			NewKinanimBoneMapping->Hips = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_RightUpLeg"))
		{
			NewKinanimBoneMapping->RightUpperLeg = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_LeftUpLeg"))
		{
			NewKinanimBoneMapping->LeftUpperLeg = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_RightLeg"))
		{
			NewKinanimBoneMapping->RightLowerLeg = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_LeftLeg"))
		{
			NewKinanimBoneMapping->LeftLowerLeg = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_RightFoot"))
		{
			NewKinanimBoneMapping->RightFoot = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_LeftFoot"))
		{
			NewKinanimBoneMapping->LeftFoot = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_RightToeBase"))
		{
			NewKinanimBoneMapping->RightToes = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_LeftToeBase"))
		{
			NewKinanimBoneMapping->LeftToes = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_LeftIndex0"))
		{
			NewKinanimBoneMapping->LeftIndexProximal = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_RightIndex0"))
		{
			NewKinanimBoneMapping->RightIndexProximal = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_LeftIndex1"))
		{
			NewKinanimBoneMapping->LeftIndexIntermediate = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_RightIndex1"))
		{
			NewKinanimBoneMapping->RightIndexIntermediate = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_LeftIndex2"))
		{
			NewKinanimBoneMapping->LeftIndexDistal = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_RightIndex2"))
		{
			NewKinanimBoneMapping->RightIndexDistal = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_LeftMiddle0"))
		{
			NewKinanimBoneMapping->LeftMiddleProximal = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_RightMiddle0"))
		{
			NewKinanimBoneMapping->RightMiddleProximal = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_LeftMiddle1"))
		{
			NewKinanimBoneMapping->LeftMiddleIntermediate = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_RightMiddle1"))
		{
			NewKinanimBoneMapping->RightMiddleIntermediate = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_LeftMiddle2"))
		{
			NewKinanimBoneMapping->LeftMiddleDistal = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_RightMiddle2"))
		{
			NewKinanimBoneMapping->RightMiddleDistal = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_LeftPinky0"))
		{
			NewKinanimBoneMapping->LeftLittleProximal = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_RightPinky0"))
		{
			NewKinanimBoneMapping->RightLittleProximal = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_LeftPinky1"))
		{
			NewKinanimBoneMapping->LeftLittleIntermediate = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_RightPinky1"))
		{
			NewKinanimBoneMapping->RightLittleIntermediate = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_LeftPinky2"))
		{
			NewKinanimBoneMapping->LeftLittleDistal = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_RightPinky2"))
		{
			NewKinanimBoneMapping->RightLittleDistal = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_LeftRing0"))
		{
			NewKinanimBoneMapping->LeftRingProximal = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_RightRing0"))
		{
			NewKinanimBoneMapping->RightRingProximal = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_LeftRing1"))
		{
			NewKinanimBoneMapping->LeftRingIntermediate = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_RightRing1"))
		{
			NewKinanimBoneMapping->RightRingIntermediate = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_LeftRing2"))
		{
			NewKinanimBoneMapping->LeftRingDistal = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_RightRing2"))
		{
			NewKinanimBoneMapping->RightRingDistal = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_LeftThumb0"))
		{
			NewKinanimBoneMapping->LeftThumbProximal = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_RightThumb0"))
		{
			NewKinanimBoneMapping->RightThumbProximal = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_LeftThumb1"))
		{
			NewKinanimBoneMapping->LeftThumbIntermediate = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_RightThumb1"))
		{
			NewKinanimBoneMapping->RightThumbIntermediate = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_LeftThumb2"))
		{
			NewKinanimBoneMapping->LeftThumbDistal = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_RightThumb2"))
		{
			NewKinanimBoneMapping->RightThumbDistal = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_Spine1"))
		{
			NewKinanimBoneMapping->Chest = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_Head"))
		{
			NewKinanimBoneMapping->Head = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_Spine2"))
		{
			NewKinanimBoneMapping->UpperChest = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_LeftHand"))
		{
			NewKinanimBoneMapping->LeftHand = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_LeftForeArm"))
		{
			NewKinanimBoneMapping->LeftLowerArm = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_LeftArm"))
		{
			NewKinanimBoneMapping->LeftUpperArm = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_RightHand"))
		{
			NewKinanimBoneMapping->RightHand = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_RightForeArm"))
		{
			NewKinanimBoneMapping->RightLowerArm = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
		else if (CacheKey == TEXT("Kinetix_RightArm"))
		{
			NewKinanimBoneMapping->RightUpperArm = JsonValue->Get()->Values[CacheKey].Get()->AsString();
		}
	}

	FKinetixEmote* Emote = GetEmote(AnimationMetadata.Id);
	if (Emote == nullptr)
	{
		UE_LOG(LogKinetixAnimation,
		       Error, TEXT("[FEmoteManager] MappingRequestComplete: Unable to retreive Emote"));
		return;
	}

	for (int i = 0; i < AnimationMetadata.AvatarMetadatas.Num(); ++i)
	{
		if (AnimationMetadata.AvatarMetadatas[i].AvatarID.ToString(
			EGuidFormats::DigitsWithHyphensLower) != AvatarUUID)
			continue;

		AnimationMetadata.AvatarMetadatas[i].bMappingAvailable = true;
		AnimationMetadata.AvatarMetadatas[i].KinanimBoneMapping = NewKinanimBoneMapping;
		Emote->SetMetadata(AnimationMetadata);
		// AvatarID found
		i = Emote->GetAnimationMetadata().AvatarMetadatas.Num();
	}

	SetBoneMapping(NewKinanimBoneMapping);

	LoadAnimation(Emote, Delegate, AvatarUUID);
}

void FEmoteManager::LoadAnimation(const FKinetixEmote* InEmote,
                                  const TDelegate<void()>& OnOperationFinished,
                                  FString AvatarUUID)
{
	const UKinetixCacheSaveGame& Manifest = FMemoryManager::Get().GetManifest();
	if (Manifest.Contains(InEmote->GetAnimationMetadata().Id))
	{
		FAnimationMetadata Metadatas = InEmote->GetAnimationMetadata();
		for (int i = 0; i < Metadatas.AvatarMetadatas.Num(); ++i)
		{
			if (Metadatas.AvatarMetadatas[i].AvatarID.ToString(
				EGuidFormats::DigitsWithHyphensLower) != AvatarUUID)
				continue;

			if (!Metadatas.AvatarMetadatas[i].bMappingAvailable)
			{
				TSharedRef<IHttpRequest, ESPMode::ThreadSafe> BoneMappingHttpRequest =
					FHttpModule::Get().CreateRequest();
				// No mapping available yet
				BoneMappingHttpRequest->SetURL(InEmote->GetAnimationMetadata().AvatarMetadatas[i].MappingURL.Map);
				BoneMappingHttpRequest->SetHeader(TEXT("User-Agent"), SDKUSERAGENT);

				BoneMappingHttpRequest->OnProcessRequestComplete().BindRaw(
					this, &FEmoteManager::MappingRequestComplete,
					InEmote->GetAnimationMetadata(), AvatarUUID, OnOperationFinished);

				if (!BoneMappingHttpRequest->ProcessRequest())
				{
					UE_LOG(LogKinetixAnimation, Warning,
					       TEXT("[EmoteManager] LoadAnimation: Unable to process animation request !"));
				}

				return;
			}

			SetBoneMapping(Metadatas.AvatarMetadatas[i].KinanimBoneMapping);
			// AvatarID found
			i = Metadatas.AvatarMetadatas.Num();
		}

		FString Path;
		UKinetixDataBlueprintFunctionLibrary::GetCacheAnimationPath(Path,
		                                                            InEmote->GetAnimationMetadata().Id);

		if (!FPaths::FileExists(Path))
		{
			UE_LOG(LogKinetixAnimation,
			       Error,
			       TEXT("[FEmoteManager] LoadAnimation(): Kinanim file not found !"))
			return;
		}

		TArray<uint8> KinanimFileContent;
		if (!FFileHelper::LoadFileToArray(KinanimFileContent, *Path))
		{
			UE_LOG(LogKinetixAnimation, Warning, TEXT("[EmoteManager] LoadAnimation: Failed to load file at %s"),
			       *Path);
			return;
		}

		// const TArray<uint8> JsonContent = HttpResponse->GetContent();
		//
		const char* KinanimFileArray = reinterpret_cast<const char*>(KinanimFileContent.GetData());
		// void* BinaryStream = Kinanim::CreateBinaryStreamFromArray(Datas, JsonContent.Num());
		
		// const TCHAR* File = *Path;
		void* Filestream = Kinanim::CreateBinaryStreamFromArray(
			KinanimFileArray,
			KinanimFileContent.Num());

		// void* Filestream = Kinanim::OpenReadFile(File);

		// if (Filestream == nullptr)
		// {
		// 	UE_LOG(LogKinetixAnimation,
		// 	       Error,
		// 	       TEXT("[FEmoteManager] LoadAnimation(): Failed to open Kinanim file !"))
		// 	return;
		// }

		UAnimSequence* ToReturn = UKinanimParser::LoadSkeletalAnimationFromStream(
			GetReferenceSkeleton(), Filestream, KinanimBoneMapping, bBlendshapesEnabled);

		if (!IsValid(ToReturn))
		{
			UE_LOG(LogKinetixAnimation,
			       Error,
			       TEXT("[FEmoteManager] LoadAnimation(): Failed to create anim sequence !"))
			return;
		}

		ToReturn->AddToRoot();
		// delete File;
		// Kinanim::ifstream_CloseStream(Filestream);
		Kinanim::ioMemStream_CloseStream(Filestream);
		
		FKinetixEmote* Emote = GetEmote(InEmote->GetAnimationMetadata().Id);
		if (Emote != nullptr)
		{
			Emote->SetAnimSequence(ToReturn);
			OnOperationFinished.ExecuteIfBound();
		}

		return;
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest =
		FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb("GET");
	HttpRequest->AppendToHeader(
		TEXT("Range"),
		FString::Printf(TEXT("bytes=0-%i"), HEADER_DOWNLOAD_SIZE));

	bool bAvatarIDFound = false;
	if (!AvatarUUID.IsEmpty()
		&& !InEmote->GetAnimationMetadata().AvatarMetadatas.IsEmpty())
	{
		for (int i = 0; i < InEmote->GetAnimationMetadata().AvatarMetadatas.Num(); ++i)
		{
			if (InEmote->GetAnimationMetadata().AvatarMetadatas[i].AvatarID.ToString(
				EGuidFormats::DigitsWithHyphensLower) != AvatarUUID)
				continue;

			if (!InEmote->GetAnimationMetadata().AvatarMetadatas[i].bMappingAvailable)
			{
				// No mapping available yet
				HttpRequest->SetURL(InEmote->GetAnimationMetadata().AvatarMetadatas[i].MappingURL.Map);
				HttpRequest->SetHeader(TEXT("User-Agent"), SDKUSERAGENT);

				HttpRequest->OnProcessRequestComplete().BindRaw(
					this, &FEmoteManager::MappingRequestComplete,
					InEmote->GetAnimationMetadata(), AvatarUUID, OnOperationFinished);

				if (!HttpRequest->ProcessRequest())
				{
					UE_LOG(LogKinetixAnimation, Warning,
					       TEXT("[EmoteManager] LoadAnimation: Unable to process animation request !"));
				}

				return;
			}

			// Mapping available, good to go
			HttpRequest->SetURL(InEmote->GetAnimationMetadata().AvatarMetadatas[i].AvatarURL.Map);
			HttpRequest->SetHeader(TEXT("User-Agent"), SDKUSERAGENT);
			// AvatarID found
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
		HttpRequest->SetHeader(TEXT("User-Agent"), SDKUSERAGENT);
	}

	// HttpRequest->OnProcessRequestComplete().BindRaw(
	// 	this, &FEmoteManager::AnimationRequestComplete,
	// 	InEmote->GetAnimationMetadata(), OnOperationFinished);

	HttpRequest->OnProcessRequestComplete().BindRaw(
		this, &FEmoteManager::HeaderRequestComplete,
		InEmote->GetAnimationMetadata(), OnOperationFinished);

	//HttpRequest->OnHeaderReceived().BindRaw(this, &FEmoteManager::AnimationHeaderReceived);
	//HttpRequest->OnRequestProgress().BindRaw(this, &FEmoteManager::AnimationRequestProgress);

	if (!HttpRequest->ProcessRequest())
	{
		UE_LOG(LogKinetixAnimation, Warning,
		       TEXT("[EmoteManager] LoadAnimation: Unable to process animation request !"));
	}
}

void FEmoteManager::AnimationHeaderReceived(FHttpRequestPtr Request, const FString& HeaderName,
                                            const FString& HeaderValue)
{
	UE_LOG(LogKinetixAnimation, Log, TEXT("Received Header %s %s"), *HeaderName, *HeaderValue);
}

void FEmoteManager::AnimationRequestProgress(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived)
{
	UE_LOG(LogKinetixAnimation, Log, TEXT("Request in progress... %i bytes sent %i received"), BytesSent,
	       BytesReceived);
}

void FEmoteManager::ClearEmotes()
{
	Instance.Get()->KinetixEmotes.Empty();
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

	const TArray<uint8> JsonContent = HttpResponse->GetContent();

	const char* Datas = reinterpret_cast<const char*>(JsonContent.GetData());
	void* BinaryStream = Kinanim::CreateBinaryStreamFromArray(Datas, JsonContent.Num());
	if (BinaryStream == nullptr)
	{
		UE_LOG(LogKinetixAnimation, Error,
		       TEXT("[FEmoteManager] AnimationRequestComplete(): Unable to create binary stream ! %s"),
		       *JsonString);
		return;
	}

	UAnimSequence* KinanimAnimSequence =
		UKinanimParser::LoadSkeletalAnimationFromStream(GetReferenceSkeleton(), BinaryStream, KinanimBoneMapping,
		                                                bBlendshapesEnabled);

	if (!IsValid(KinanimAnimSequence))
	{
		UE_LOG(LogKinetixAnimation, Error,
		       TEXT("[FEmoteManager] AnimationRequestComplete(): Unable to create anim sequence from binary stream ! %s"
		       ),
		       *JsonString);
		return;
	}

#pragma endregion

	// TArray<TSharedPtr<FJsonValue>> JsonArray;
	// const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);
	//
	// FglTFRuntimeConfig RuntimeConfig;

	// Setup for animation V1
	// RuntimeConfig.TransformBaseType = EglTFRuntimeTransformBaseType::YForward;

	// Setup for animation V2
	// RuntimeConfig.TransformBaseType = EglTFRuntimeTransformBaseType::Transform;
	// RuntimeConfig.BaseTransform.SetRotation(FRotator(0.f, 180.f, 0.f).Quaternion());
	// RuntimeConfig.BaseTransform.SetScale3D(FVector(-1.f, 1.f, 1.f));
	//
	// UglTFRuntimeAsset* GlTFAsset =
	// 	UglTFRuntimeFunctionLibrary::glTFLoadAssetFromData(
	// 		HttpResponse->GetContent(),
	// 		RuntimeConfig);

	// if (!IsValid(GlTFAsset))
	// {
	// 	UE_LOG(LogKinetixAnimation, Warning,
	// 	       TEXT("[FEmoteManager] AnimationRequestComplete(): Unable to deserialize response ! %s"),
	// 	       *JsonString);
	// 	return;
	// }

	// FglTFRuntimeSkeletalAnimationConfig SkeletalAnimConfig;
	// if (CurveRemapper.IsBound())
	// {
	// 	SkeletalAnimConfig.CurveRemapper.Remapper = CurveRemapper;
	// }
	// USkeletalMesh* RefSkeletalMesh = GetReferenceSkeleton();
	// if (!IsValid(RefSkeletalMesh))
	// {
	// 	UE_LOG(LogKinetixAnimation, Warning,
	// 	       TEXT("[FEmoteManager] AnimationRequestComplete(): RefSekeletalMesh is null !"));
	// 	return;
	// }
	//
	// UAnimSequence* AnimSequence = GlTFAsset->LoadSkeletalAnimation(RefSkeletalMesh, 0,
	//                                                                SkeletalAnimConfig);

#pragma region Saving file


	FString Path;
	UKinetixDataBlueprintFunctionLibrary::GetCacheAnimationPath(Path, InAnimationMetadata.Id);

	if (!FPaths::FileExists(Path))
	{
		if (!FFileHelper::SaveArrayToFile(HttpResponse->GetContent(), *Path))
		{
			UE_LOG(LogKinetixAnimation,
			       Error,
			       TEXT("[FEmoteManager] AnimationRequestComplete(): Saving .kinanim file failed !"))
		}
	}

#pragma endregion

#pragma region Loading file into asset

	// UglTFRuntimeAsset* LoadedGltfAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilename(
	// 	Path, false, RuntimeConfig);
	//
	// if (LoadedGltfAsset == nullptr)
	// {
	// 	UE_LOG(LogKinetixAnimation,
	// 	       Error,
	// 	       TEXT("[FEmoteManager] AnimationRequestComplete(): Loaded .glb file failed !"))
	// }

#pragma endregion

	if (!IsValid(KinanimAnimSequence))
	{
		UE_LOG(LogKinetixAnimation, Error,
		       TEXT("[FAccount] AnimationRequestComplete(): AnimSequence is null !"));
		return;
	}

	FKinetixEmote* Emote = GetEmote(InAnimationMetadata.Id);
	if (Emote != nullptr)
	{
		Emote->SetAnimSequence(KinanimAnimSequence);

		OnSuccessDelegate.ExecuteIfBound();

		FMemoryManager::Get().OnAnimationLoadedOnPlayer(InAnimationMetadata.Id);
	}
}

void FEmoteManager::HeaderRequestComplete(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest,
                                          TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse, bool bSuccess,
                                          FAnimationMetadata InAnimationMetadata,
                                          TDelegate<void()> OnSuccessDelegate)
{
	if (!bSuccess
		|| !HttpResponse.IsValid()
		|| !EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		return;

	// TODO: Delete due to debug used
	FString JsonString = HttpResponse->GetContentAsString();

	const TArray<uint8> JsonContent = HttpResponse->GetContent();

	const char* Datas = reinterpret_cast<const char*>(JsonContent.GetData());
	void* BinaryStream = Kinanim::CreateBinaryStreamFromArray(Datas, JsonContent.Num());
	if (BinaryStream == nullptr)
	{
		UE_LOG(LogKinetixAnimation, Error,
		       TEXT("[FEmoteManager] AnimationRequestComplete(): Unable to create binary stream ! %s"),
		       *JsonString);
		return;
	}

	void* Importer = nullptr;
	if (!UKinanimParser::LoadStartDataFromStream(
		GetReferenceSkeleton(), BinaryStream, &Importer))
	{
		UE_LOG(LogKinetixAnimation, Error,
		       TEXT("[FEmoteManager] AnimationRequestComplete(): Unable to create binary stream ! %s"),
		       *JsonString);
		return;
	}

	UKinanimDownloader* KinanimDownloader = NewObject<UKinanimDownloader>();
	KinanimDownloader->SetImporter(&Importer);
	KinanimDownloader->SetBlendshapesEnabled(bBlendshapesEnabled);
	KinanimDownloader->SetUrl(HttpRequest->GetURL());
	KinanimDownloader->SetAnimationMetadataID(InAnimationMetadata.Id.UUID);
	KinanimDownloader->SetupCurveRemapper(CurveRemapper);
	KinanimDownloader->SetupAnimSequence(GetReferenceSkeleton(), KinanimBoneMapping);
	KinanimDownloader->AddToRoot();
	KinanimDownloader->OnKinanimDownloadComplete.BindRaw(
		this, &FEmoteManager::OnKinanimComplete, OnSuccessDelegate);
	KinanimDownloader->OnKinanimPlayAvailable.BindRaw(
		this, &FEmoteManager::OnKinanimAvailableToPlay, OnSuccessDelegate);
	if (!KinanimDownloader->DownloadRemainingFrames())
	{
		UE_LOG(LogKinetixAnimation, Error,
		       TEXT("[FEmoteManager] AnimationRequestComplete(): Unable to create binary stream !"));
		return;
	}
}

void FEmoteManager::OnKinanimAvailableToPlay(UKinanimDownloader* KinanimDownloader, TDelegate<void()> OnSuccessDelegate)
{
	UAnimSequence* KinanimAnimSequence = KinanimDownloader->GetAnimationSequence();

	if (!IsValid(KinanimAnimSequence))
	{
		UE_LOG(LogKinetixAnimation, Error,
		       TEXT("[FAccount] AnimationRequestComplete(): AnimSequence is null !"));
		return;
	}

	FAnimationID InAnimationID;
	UKinetixDataBlueprintFunctionLibrary::GetAnimationIDFromString(
		KinanimDownloader->GetAnimationMetadataID().ToString(), InAnimationID);

	FKinetixEmote* Emote = GetEmote(InAnimationID);
	if (Emote != nullptr)
	{
		Emote->SetAnimSequence(KinanimAnimSequence);

		OnSuccessDelegate.ExecuteIfBound();

		FMemoryManager::Get().OnAnimationLoadedOnPlayer(InAnimationID);

		KinanimDownloader->RemoveFromRoot();
		KinanimDownloader = nullptr;
	}
}

void FEmoteManager::OnKinanimComplete(UKinanimDownloader* KinanimDownloader, TDelegate<void()> OnSuccessDelegate)
{
	if (!IsValid(KinanimDownloader))
	{
		UE_LOG(LogKinetixAnimation, Error,
		       TEXT("[FEmoteManager] OnKinanimComplete(): Kinanim downloader is null !"));
		return;
	}

	UAnimSequence* KinanimAnimSequence = KinanimDownloader->GetAnimationSequence();

	if (!IsValid(KinanimAnimSequence))
	{
		UE_LOG(LogKinetixAnimation, Error,
		       TEXT("[FAccount] AnimationRequestComplete(): AnimSequence is null !"));
		return;
	}

	FAnimationID InAnimationID;
	UKinetixDataBlueprintFunctionLibrary::GetAnimationIDFromString(
		KinanimDownloader->GetAnimationMetadataID().ToString(), InAnimationID);

	FString Path;
	UKinetixDataBlueprintFunctionLibrary::GetCacheAnimationPath(Path, InAnimationID);

	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	if (!FPaths::FileExists(Path))
	{
		if (!FFileHelper::SaveArrayToFile(TArray<uint8>(), *Path))
		{
			UE_LOG(LogKinetixAnimation,
			       Error,
			       TEXT("[FEmoteManager] AnimationRequestComplete(): Saving .kinanim file failed !"))
		}
	}

	if (!FPaths::FileExists(Path))
	{
		int32 Index = 0;
		Path.FindLastChar('/', Index);
		FString PathWithoutFile = Path.Left(Index);
		PathWithoutFile += '/';
		if (!FileManager.CreateDirectory(*PathWithoutFile))
		{
			UE_LOG(LogKinetixAnimation,
			       Error,
			       TEXT("[FEmoteManager] AnimationRequestComplete(): Saving .kinanim file failed !"))
		}
	}

	UE_LOG(LogKinetixAnimation,
	       Warning,
	       TEXT("[FEmoteManager] AnimationRequestComplete(): Try saving file with %d frames !"),
	       KinanimDownloader->GetFrameCount());

	//////////// CALCUL DE LA TAILLE DU FICHIER /////////////
	uint64 TotalSize = 0;
	void* UncompressedHeader = KinanimDownloader->GetUncompressedHeader();
	if (UncompressedHeader == nullptr)
	{
		UE_LOG(LogKinetixAnimation, Error,
		       TEXT("[FEmoteManager] AnimationRequestComplete(): No header on KinanimDownloader !"))
		return;
	}

	TotalSize = KinanimWrapper::KinanimHeader_Get_binarySize(UncompressedHeader);
	for (int i = 0; i < KinanimDownloader->GetFrameCount(); ++i)
	{
		TotalSize += KinanimWrapper::KinanimHeader_Get_frameSizes(UncompressedHeader, i);
	}

	if (TotalSize == 0)
	{
		UE_LOG(LogKinetixAnimation, Error,
		       TEXT("[FEmoteManager] AnimationRequestComplete(): Returned size of Kinanim is 0 !"))
		return;
	}

	//////////// FORMATAGE DU FLUX ET CONVERSION EN TARRAY ///////////////////
	void* ExportStream = Kinanim::CreateBinaryStream(TotalSize);
	KinanimWrapper::KinanimExporter_WriteHeader(ExportStream, KinanimDownloader->GetUncompressedHeader());
	KinanimWrapper::KinanimExporter_Content_WriteFrames(
		ExportStream,
		KinanimDownloader->GetFrames(),
		KinanimDownloader->GetFrameCount(),
		KinanimWrapper::KinanimHeader_Get_hasBlendshapes(KinanimDownloader->GetUncompressedHeader()),
		0);

	char* Buffer = KinanimWrapper::IoMemStream_GetBuffer(ExportStream);
	TArray<uint8> TargetBuffer;
	TargetBuffer.SetNumZeroed(TotalSize / sizeof(uint8));

	for (int i = 0; i < TargetBuffer.Num(); ++i)
	{
		TargetBuffer[i] = *Buffer;
		Buffer += sizeof(uint8);
	}

	FFileHelper::SaveArrayToFile(TargetBuffer, *Path);

	Kinanim::ioMemStream_CloseStream(ExportStream);
	return;

	//void* ExportStream = Kinanim::CreateBinaryStream(100000);
	// void* ExportStream = Kinanim::OpenWriteFile(*Path, false);
	// KinanimWrapper::KinanimExporter_WriteHeader(ExportStream, KinanimDownloader->GetUncompressedHeader());
	// KinanimWrapper::KinanimExporter_Content_WriteFrames(
	// 	ExportStream,
	// 	KinanimDownloader->GetFrames(),
	// 	KinanimDownloader->GetFrameCount(),
	// 	KinanimWrapper::KinanimHeader_Get_hasBlendshapes(KinanimDownloader->GetUncompressedHeader()),
	// 	0);
	//
	// Kinanim::ioMemStream_CloseStream(ExportStream);
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
