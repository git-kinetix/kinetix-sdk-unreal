// Copyright Kinetix. All Rights Reserved.

#include "Data/KinetixDataLibrary.h"

#if WITH_EDITOR
#include "AssetToolsModule.h"
#include "Factories/DataAssetFactory.h"
#endif

#include "AssetRegistry/AssetRegistryModule.h"
#include "glTFRuntimeFunctionLibrary.h"
#include "JsonBlueprintFunctionLibrary.h"
#include "JsonObjectWrapper.h"
#include "UObject/SavePackage.h"

#include "KinetixRuntimeModule.h"
#include "Core/KinetixCoreSubsystem.h"
#include "Core/Metadata/KinetixMetadata.h"
#include "Data/AnimationMetadataAsset.h"
#include "Engine/AssetManager.h"
#include "Interfaces/KinetixSubcoreInterface.h"
#include "JsonUtils/JsonPointer.h"
#include "Managers/EmoteManager.h"

UKinetixDataBlueprintFunctionLibrary::UKinetixDataBlueprintFunctionLibrary()
{
}

bool UKinetixDataBlueprintFunctionLibrary::GetPluginRelativePath(FString& RelativePath)
{
	RelativePath = FKinetixRuntimeModule::GetPlugin()->GetBaseDir();
	return FPaths::MakePathRelativeTo(RelativePath, *FPaths::ProjectPluginsDir());
}

bool UKinetixDataBlueprintFunctionLibrary::RemoveContentFromPluginPath(FString& RelativePath, FString PathRoot)
{
	int32 Index = RelativePath.Find(PathRoot);

	if (Index == INDEX_NONE)
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("Wrong path given %s"), *RelativePath);
		return false;
	}

	FString ContentPath = "Content/";
	RelativePath.RemoveAt(Index + PathRoot.Len(), ContentPath.Len());
	return true;
}

bool UKinetixDataBlueprintFunctionLibrary::GetCacheAnimationPath(FString& AbsolutePath,
                                                                 const FAnimationID& InAnimationMetadata)
{
	AbsolutePath = FPaths::Combine(FPaths::ProjectUserDir(), TEXT("Kinetix"), TEXT("Animations"));
	AbsolutePath = FString::Printf(TEXT("%s/%s.kinanim"),
	                               *AbsolutePath,
	                               *InAnimationMetadata.UUID.ToString(EGuidFormats::DigitsWithHyphensLower));

	return true;
}

bool UKinetixDataBlueprintFunctionLibrary::GetMetadataFiles(TArray<FString>& OutFiles, const FString InFileExtension)
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();


#if WITH_EDITOR
	PlatformFile.FindFilesRecursively(OutFiles,
	                                  *FKinetixRuntimeModule::GetPlugin()->GetContentDir(),
	                                  *InFileExtension);
#else
	PlatformFile.FindFilesRecursively(OutFiles,
	                                  *FPaths::ProjectContentDir(),
	                                  *InFileExtension);
	UE_LOG(LogKinetixRuntime, Warning, TEXT("FPaths::ProjectContentDir(): %s"), *FPaths::ProjectContentDir());
#endif

	return OutFiles.Num() != 0;
}

bool UKinetixDataBlueprintFunctionLibrary::GetJsonObjectFromFile(
	UObject* WorldContextObject, FString File, FJsonObjectWrapper& OutJsonObjectWrapper)
{
	FFilePath JsonFilePath;
	JsonFilePath.FilePath = File;

#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 4
	return UJsonBlueprintFunctionLibrary::FromFile(JsonFilePath, OutJsonObjectWrapper);
#else
	return UJsonBlueprintFunctionLibrary::FromFile(WorldContextObject, JsonFilePath, OutJsonObjectWrapper);
#endif
}

bool UKinetixDataBlueprintFunctionLibrary::GetUUIDFromJson(FGuid& OutUUID,
                                                           const FJsonObjectWrapper& InJsonObjectWrapper)
{
	const TSharedRef<FJsonValue> UUIDJsonField =
		InJsonObjectWrapper.JsonObject->GetField<EJson::String>(TEXT("uuid")).ToSharedRef();
	FString UUIDValue;
	UUIDJsonField->TryGetString(UUIDValue);
	return FGuid::Parse(UUIDValue, OutUUID);
}

bool UKinetixDataBlueprintFunctionLibrary::GetNameFromJson(FName& OutName,
                                                           const FJsonObjectWrapper& InJsonObjectWrapper)
{
	bool Result = false;
	const TSharedRef<FJsonValue> NameJsonField =
		InJsonObjectWrapper.JsonObject->GetField<EJson::String>(TEXT("name")).ToSharedRef();
	FString TempValue;
	Result = NameJsonField->TryGetString(TempValue);
	OutName = FName(TempValue);
	return Result;
}

bool UKinetixDataBlueprintFunctionLibrary::GetOwnershipFromJson(EOwnership& OutOwnership,
                                                                const FJsonObjectWrapper& InJsonObjectWrapper)
{
	const TSharedRef<FJsonValue> OwnershipJsonField =
		InJsonObjectWrapper.JsonObject->GetField<EJson::String>(TEXT("origin")).ToSharedRef();
	FString OwnershipValue;
	OwnershipJsonField->TryGetString(OwnershipValue);
	OwnershipValue = FString::Printf(TEXT("o_%s"), *OwnershipValue);
	OutOwnership = static_cast<EOwnership>(StaticEnum<EOwnership>()->GetValueByNameString(OwnershipValue));
	return true;
}

bool UKinetixDataBlueprintFunctionLibrary::GetDurationFromJson(float& OutDuration,
                                                               const FJsonObjectWrapper& InJsonObjectWrapper)
{
	const TSharedRef<FJsonValue> DurationJsonField =
		InJsonObjectWrapper.JsonObject->GetField<EJson::Number>(TEXT("duration")).ToSharedRef();
	return DurationJsonField->TryGetNumber(OutDuration);
}

bool UKinetixDataBlueprintFunctionLibrary::GetAnimationIDFromString(const FString& InAnimationID,
                                                                    FAnimationID& OutAnimationID)
{
	return FGuid::Parse(InAnimationID, OutAnimationID.UUID);
}

bool UKinetixDataBlueprintFunctionLibrary::GetStringFromAnimationID(const FAnimationID& InAnimationID,
                                                                    FString& OutAnimationID)
{
	OutAnimationID = InAnimationID.UUID.ToString(EGuidFormats::DigitsWithHyphensLower);
	return true;
}

bool UKinetixDataBlueprintFunctionLibrary::GetAnimationMetadataFromFile(UObject* WorldContextObject, FString File,
                                                                        FAnimationMetadata& AnimationMetadata)
{
	// Retrieve the Json object context
	FJsonObjectWrapper JsonObjectRef;

	if (!GetJsonObjectFromFile(WorldContextObject, File, JsonObjectRef))
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("File doesn't exist !"));
		return false;
	}

	if (!GetUUIDFromJson(AnimationMetadata.Id.UUID, JsonObjectRef))
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("Unable to get UUID !"));
		return false;
	}

	if (!GetNameFromJson(AnimationMetadata.Name, JsonObjectRef))
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("Unable to get Name !"));
		return false;
	}

	if (!GetOwnershipFromJson(AnimationMetadata.Ownership, JsonObjectRef))
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("Unable to get Ownership !"));
		return false;
	}

	if (!GetDurationFromJson(AnimationMetadata.Duration, JsonObjectRef))
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("Unable to get Duration !"));
		return false;
	}

	return true;
}

bool UKinetixDataBlueprintFunctionLibrary::GetAnimationMetadataFromJson(
	const TSharedPtr<FJsonObject>& JsonObject,
	FAnimationMetadata& OutAnimationMetadata)
{
	FString StringField;
	if (!JsonObject->TryGetStringField(TEXT("uuid"), StringField))
		return false;
	FGuid::Parse(StringField, OutAnimationMetadata.Id.UUID);

	if (!JsonObject->TryGetStringField(TEXT("name"), StringField))
		return false;
	OutAnimationMetadata.Name = FName(StringField);

	const TSharedPtr<FJsonObject>* MetadataObject = nullptr;
	JsonObject->TryGetObjectField(TEXT("metadata"), MetadataObject);
	if (!(MetadataObject && MetadataObject->IsValid()))
	{
		UE_LOG(LogKinetixRuntime, Error, TEXT("WRONG WRONG WRONG"));
		return false;
	}

	(*MetadataObject)->TryGetNumberField(TEXT("duration"), OutAnimationMetadata.Duration);
	(*MetadataObject)->TryGetStringField(TEXT("description"), StringField);
	OutAnimationMetadata.Description = FText::FromString(StringField);

	const TSharedPtr<FJsonObject>* AvatarObject;
	JsonObject->TryGetObjectField(TEXT("avatars"), AvatarObject);

	bool bHasAvatarMetadata = false;
	if (AvatarObject && AvatarObject->IsValid() && !(*AvatarObject)->Values.IsEmpty())
		bHasAvatarMetadata = true;

	if (bHasAvatarMetadata)
	{
		OutAnimationMetadata.AvatarMetadatas.Empty();
		OutAnimationMetadata.AvatarMetadatas.SetNum((*AvatarObject)->Values.Num());

		for (int i = 0; i < (*AvatarObject)->Values.Num(); ++i)
		{
			FString AvatarID = (*AvatarObject)->Values.Array()[i].Key;
			TSharedPtr<FJsonValue> AvatarValue = (*AvatarObject)->Values.Array()[i].Value;

			const TArray<TSharedPtr<FJsonValue>>* AvatarMetadatas;
			AvatarObject->Get()->TryGetArrayField(AvatarID, AvatarMetadatas);

			FGuid::Parse(AvatarID, OutAnimationMetadata.AvatarMetadatas[i].AvatarID);

			if (!(AvatarValue && AvatarValue.IsValid()))
				return false;

			if (!(AvatarMetadatas && AvatarMetadatas->Num()))
				return false;

			for (int j = 0; j < AvatarMetadatas->Num(); ++j)
			{
				TSharedPtr<FJsonObject> AvatarMetadata = (*AvatarMetadatas)[j]->AsObject();
				if (!AvatarMetadata.IsValid())
					continue;

				AvatarMetadata->TryGetStringField(TEXT("name"), StringField);
				if (StringField == TEXT("thumbnail"))
				{
					AvatarMetadata->TryGetStringField(TEXT("extension"), StringField);
					if (StringField != TEXT("png"))
						continue;
					AvatarMetadata->TryGetStringField(TEXT("url"), OutAnimationMetadata.AvatarMetadatas[i].IconURL.Map);
					continue;
				}

				if (StringField == TEXT("userData"))
				{
					UE_LOG(LogKinetixRuntime, Warning,
					       TEXT("[FAccount] GetAnimationMetadataFromJson(): Found UserDatas !"));

					AvatarMetadata->TryGetStringField(TEXT("url"), 
					OutAnimationMetadata.AvatarMetadatas[i].MappingURL.Map);
					continue;
				}

				// if (StringField == TEXT("unreal"))
				if (StringField == TEXT("animation"))
				{
					// Ready for kinanim integration
					FString Extension;
					AvatarMetadata->TryGetStringField(TEXT("extension"), Extension);
					// if (Extension == TEXT("glb"))
					if (Extension == TEXT("kinanim"))
					{
						AvatarMetadata->TryGetStringField(
							TEXT("url"), OutAnimationMetadata.AvatarMetadatas[i].AvatarURL.Map);
					}
				}
			}
		}
	}

	const TArray<TSharedPtr<FJsonValue>>* FilesObject;
	JsonObject->TryGetArrayField(TEXT("files"), FilesObject);
	if (!(FilesObject && FilesObject->Num()))
		return false;

	for (int j = 0; j < FilesObject->Num(); ++j)
	{
		TSharedPtr<FJsonObject> FileObject = (*FilesObject)[j]->AsObject();
		if (!FileObject.IsValid())
			continue;

		FileObject->TryGetStringField(TEXT("name"), StringField);
		if (StringField == TEXT("thumbnail"))
		{
			FileObject->TryGetStringField(TEXT("extension"), StringField);
			if (StringField != TEXT("png"))
				continue;
			FileObject->TryGetStringField(TEXT("url"), OutAnimationMetadata.IconURL.Map);
			continue;
		}

		// if (StringField == TEXT("unreal"))
		if (StringField == TEXT("animation-v2"))
		{
			// Ready for kinanim integration
			FString Extension;
			FileObject->TryGetStringField(TEXT("extension"), Extension);
			// if (Extension == TEXT("glb"))
			if (Extension == TEXT("kinanim"))
			{
				FileObject->TryGetStringField(TEXT("url"), OutAnimationMetadata.AnimationURL.Map);
			}
		}
		else if ((StringField == TEXT("animation")) && (!OutAnimationMetadata.AnimationURL.Map.Contains(TEXT("https"))))
			FileObject->TryGetStringField(TEXT("url"), OutAnimationMetadata.AnimationURL.Map);

		// Will be removed when kinanim in place
		// FString CacheURL;
		// FileObject->TryGetStringField(TEXT("url"), CacheURL);
		// if (!CacheURL.Contains(".glb"))
		// 	continue;

		// OutAnimationMetadata.AnimationURL.Map = CacheURL;
	}


	UE_LOG(LogKinetixRuntime, Warning,
	       TEXT("[FKinetixDataLibrary] MetadataRequestComplete(): Generated AnimationMetadata: %s %s %f %s %s"),
	       *OutAnimationMetadata.Id.UUID.ToString(EGuidFormats::DigitsWithHyphensLower),
	       *OutAnimationMetadata.Name.ToString(),
	       OutAnimationMetadata.Duration,
	       *OutAnimationMetadata.IconURL.Map,
	       *OutAnimationMetadata.AnimationURL.Map);
	return true;
}

bool UKinetixDataBlueprintFunctionLibrary::GetAnimationMetadataFromJson(const FString& JsonString,
                                                                        FAnimationMetadata& OutAnimationMetadata)
{
	const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);
	TSharedPtr<FJsonObject> JsonObject;

	const bool bDeserializationResult = FJsonSerializer::Deserialize(JsonReader, JsonObject);
	if (!bDeserializationResult)
	{
		UE_LOG(LogKinetixRuntime, Warning,
		       TEXT("GetAnimationMetadataFromJson(): Unable to deserialize response ! %s"),
		       *JsonReader.Get().GetErrorMessage());
		return false;
	}

	if (!GetAnimationMetadataFromJson(JsonObject, OutAnimationMetadata))
	{
		UE_LOG(LogKinetixRuntime, Warning,
		       TEXT("GetAnimationMetadataFromJson(): Unable to get datas from Json object ! %s"),
		       *JsonReader.Get().GetErrorMessage());
		return false;
	}

	FString StringField;
	const TSharedPtr<FJsonObject>* MetadataObject = nullptr;
	JsonObject->TryGetObjectField(TEXT("metadata"), MetadataObject);
	if (!(MetadataObject && MetadataObject->IsValid()))
		return false;

	(*MetadataObject)->TryGetNumberField(TEXT("duration"), OutAnimationMetadata.Duration);
	(*MetadataObject)->TryGetStringField(TEXT("description"), StringField);
	OutAnimationMetadata.Description = FText::FromString(StringField);

	const TSharedPtr<FJsonObject>* AvatarObject;
	JsonObject->TryGetObjectField(TEXT("avatars"), AvatarObject);

	bool bHasAvatarMetadata = false;
	if (AvatarObject && AvatarObject->IsValid() && !(*AvatarObject)->Values.IsEmpty())
		bHasAvatarMetadata = true;

	if (bHasAvatarMetadata)
	{
		OutAnimationMetadata.AvatarMetadatas.Empty();
		OutAnimationMetadata.AvatarMetadatas.SetNum((*AvatarObject)->Values.Num());

		for (int i = 0; i < (*AvatarObject)->Values.Num(); ++i)
		{
			FString AvatarID = (*AvatarObject)->Values.Array()[i].Key;
			TSharedPtr<FJsonValue> AvatarValue = (*AvatarObject)->Values.Array()[i].Value;

			const TArray<TSharedPtr<FJsonValue>>* AvatarMetadatas;
			AvatarObject->Get()->TryGetArrayField(AvatarID, AvatarMetadatas);

			FGuid::Parse(AvatarID, OutAnimationMetadata.AvatarMetadatas[i].AvatarID);

			if (!(AvatarValue && AvatarValue.IsValid()))
				return false;

			if (!(AvatarMetadatas && AvatarMetadatas->Num()))
				return false;

			for (int j = 0; j < AvatarMetadatas->Num(); ++j)
			{
				TSharedPtr<FJsonObject> AvatarMetadata = (*AvatarMetadatas)[j]->AsObject();
				if (!AvatarMetadata.IsValid())
					continue;

				AvatarMetadata->TryGetStringField(TEXT("name"), StringField);
				if (StringField == TEXT("thumbnail"))
				{
					AvatarMetadata->TryGetStringField(TEXT("extension"), StringField);
					if (StringField != TEXT("png"))
						continue;
					AvatarMetadata->TryGetStringField(TEXT("url"), OutAnimationMetadata.AvatarMetadatas[i].IconURL.Map);
					continue;
				}

				if (StringField == TEXT("userData"))
				{
					UE_LOG(LogKinetixRuntime, Warning,
					       TEXT("[FAccount] GetAnimationMetadataFromJson(): Found UserDatas !"));

					AvatarMetadata->TryGetStringField(TEXT("url"), 
					OutAnimationMetadata.AvatarMetadatas[i].MappingURL.Map);
					continue;
				}

				// if (StringField == TEXT("unreal"))
				if (StringField == TEXT("animation"))
				{
					// Ready for kinanim integration
					FString Extension;
					AvatarMetadata->TryGetStringField(TEXT("extension"), Extension);
					// if (Extension == TEXT("glb"))
					if (Extension == TEXT("kinanim"))
					{
						AvatarMetadata->TryGetStringField(
							TEXT("url"), OutAnimationMetadata.AvatarMetadatas[i].AvatarURL.Map);
					}
				}
			}
		}
	}

	const TArray<TSharedPtr<FJsonValue>>* FilesObject;
	JsonObject->TryGetArrayField(TEXT("files"), FilesObject);
	if (!(FilesObject && FilesObject->Num()))
		return false;

	for (int j = 0; j < FilesObject->Num(); ++j)
	{
		TSharedPtr<FJsonObject> FileObject = (*FilesObject)[j]->AsObject();
		if (!FileObject.IsValid())
			continue;

		FileObject->TryGetStringField(TEXT("name"), StringField);
		if (StringField == TEXT("thumbnail"))
		{
			FileObject->TryGetStringField(TEXT("extension"), StringField);
			if (StringField != TEXT("png"))
				continue;
			FileObject->TryGetStringField(TEXT("url"), OutAnimationMetadata.IconURL.Map);
			continue;
		}

		// if (StringField == TEXT("unreal"))
		if (StringField == TEXT("animation-v2"))
		{
			// Ready for kinanim integration
			FString Extension;
			FileObject->TryGetStringField(TEXT("extension"), Extension);
			// if (Extension == TEXT("glb"))
			if (Extension == TEXT("kinanim"))
			{
				FileObject->TryGetStringField(TEXT("url"), OutAnimationMetadata.AnimationURL.Map);
			}
		}
		else if ((StringField == TEXT("animation")) && (!OutAnimationMetadata.AnimationURL.Map.Contains(TEXT("https"))))
			FileObject->TryGetStringField(TEXT("url"), OutAnimationMetadata.AnimationURL.Map);

		// Will be removed when kinanim in place
		// FString CacheURL;
		// FileObject->TryGetStringField(TEXT("url"), CacheURL);
		// if (!CacheURL.Contains(".glb"))
		// 	continue;

		// OutAnimationMetadata.AnimationURL.Map = CacheURL;
	}


	UE_LOG(LogKinetixRuntime, Warning,
	       TEXT("[FKinetixDataLibrary] MetadataRequestComplete(): Generated AnimationMetadata: %s %s %f %s %s"),
	       *OutAnimationMetadata.Id.UUID.ToString(EGuidFormats::DigitsWithHyphensLower),
	       *OutAnimationMetadata.Name.ToString(),
	       OutAnimationMetadata.Duration,
	       *OutAnimationMetadata.IconURL.Map,
	       *OutAnimationMetadata.AnimationURL.Map);

	return true;
}

bool UKinetixDataBlueprintFunctionLibrary::GetIconURL(const FAnimationMetadata& InAnimationMetadata,
                                                      FString& OutIconURL)
{
	if (InAnimationMetadata.IconURL.Map.IsEmpty())
		return false;

	OutIconURL = InAnimationMetadata.IconURL.Map;
	return true;
}

bool UKinetixDataBlueprintFunctionLibrary::EqualEqual_AnimationMetadataAnimationMetadata(FAnimationMetadata A,
	FAnimationMetadata B)
{
	return A == B;
}

bool UKinetixDataBlueprintFunctionLibrary::IsValid(FAnimationMetadata& InAnimationMetadata)
{
	return InAnimationMetadata.Id.UUID.IsValid();
}

bool UKinetixDataBlueprintFunctionLibrary::LoadReferenceSkeletonAsset(const TDelegate<void(FAssetData)>& Callback)
{
	FAssetRegistryModule& AssetRegistryModule =
		FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	// /Script/Engine.Skeleton'/ReadyPlayerMe/Character/Fullbody/Mesh/RPM_Mixamo_Skeleton.RPM_Mixamo_Skeleton'
	// Normalize path D:/Kinetix/kinetix-sdk-unreal/Plugins/ReadyPlayerMe/Content/Character/Fullbody/Mesh/RPM_Mixamo_SkeletalMesh.uasset
	FString GeneralPath = IPluginManager::Get().FindPlugin(TEXT("Kinetix"))->GetContentDir() + TEXT(
		"/Characters/Sam");
	GeneralPath = FPaths::CreateStandardFilename(GeneralPath);

	// Format package path
	FString SkeletalMeshPackagePath = GeneralPath;
	FPaths::MakePathRelativeTo(SkeletalMeshPackagePath, *FPaths::ProjectPluginsDir());
	RemoveContentFromPluginPath(SkeletalMeshPackagePath, TEXT("Kinetix"));
	SkeletalMeshPackagePath = FString::Printf(TEXT("/%s"), *SkeletalMeshPackagePath);

	FARFilter Filter;
	Filter.ClassPaths.Add(USkeletalMesh::StaticClass()->GetClassPathName());
	Filter.PackagePaths.Add(FName(SkeletalMeshPackagePath));
	Filter.bRecursiveClasses = true;
	Filter.bRecursivePaths = true;

	TArray<FAssetData> SkeletalMeshDatas;
	AssetRegistryModule.Get().GetAssets(Filter, SkeletalMeshDatas);

	if (SkeletalMeshDatas.IsEmpty())
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("No asset found"));
		return false;
	}

	if (!SkeletalMeshDatas[0].IsAssetLoaded())
	{
		UE_LOG(LogKinetixRuntime, Log, TEXT("%s not loaded yet, launch loading..."),
		       *SkeletalMeshDatas[0].AssetName.ToString());

		FStreamableDelegate TempDelegate = FStreamableDelegate::CreateLambda(
			[Callback, SkeletalMeshDatas]()
			{
				Callback.Execute(SkeletalMeshDatas[0]);
			});

		// Launch async loading
		UAssetManager::GetStreamableManager().RequestAsyncLoad(
			SkeletalMeshDatas[0].ToSoftObjectPath(),
			TempDelegate);
		return false;
	}

	Callback.Execute(SkeletalMeshDatas[0]);

	return true;
}

FString UKinetixDataBlueprintFunctionLibrary::GetKinetixSlotName()
{
	return KINETIXSLOTNAME;
}

void UKinetixDataBlueprintFunctionLibrary::ClearSmartCache()
{
	UKinetixCacheSaveGame::ClearCache();
	FEmoteManager::Get().ClearEmotes();
}
