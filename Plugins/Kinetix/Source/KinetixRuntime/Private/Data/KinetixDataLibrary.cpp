// // Copyright Kinetix. All Rights Reserved.


#include "Data/KinetixDataLibrary.h"

#include "AssetToolsModule.h"
#include "glTFRuntimeFunctionLibrary.h"
#include "JsonBlueprintFunctionLibrary.h"
#include "JsonObjectWrapper.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Factories/DataAssetFactory.h"
#include "UObject/SavePackage.h"

#include "KinetixRuntimeModule.h"
#include "Core/KinetixCoreSubsystem.h"
#include "Data/AnimationMetadataAsset.h"
#include "Engine/AssetManager.h"

bool UKinetixDataBlueprintFunctionLibrary::GetPluginRelativePath(FString& RelativePath)
{
	RelativePath = FKinetixRuntimeModule::GetPlugin()->GetBaseDir();
	return FPaths::MakePathRelativeTo(RelativePath, *FPaths::ProjectPluginsDir());
}

bool UKinetixDataBlueprintFunctionLibrary::RemoveContentFromPluginPath(FString& RelativePath)
{
	FString KinetixRoot = "Kinetix/";
	int32 Index = RelativePath.Find(KinetixRoot);

	if (Index == INDEX_NONE)
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("Wrong path given %s"), *RelativePath);
		return false;
	}

	FString ContentPath = "Content/";
	RelativePath.RemoveAt(Index + KinetixRoot.Len(), ContentPath.Len());
	return true;
}

bool UKinetixDataBlueprintFunctionLibrary::GetMetadataFiles(TArray<FString>& OutFiles, const FString InFileExtension)
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	PlatformFile.FindFilesRecursively(OutFiles,
	                                  *FKinetixRuntimeModule::GetPlugin()->GetContentDir(),
	                                  *InFileExtension);
	return OutFiles.Num() != 0;
}

bool UKinetixDataBlueprintFunctionLibrary::GetJsonObjectFromFile(
	UObject* WorldContextObject, FString File, FJsonObjectWrapper& OutJsonObjectWrapper)
{
	FFilePath JsonFilePath;
	JsonFilePath.FilePath = File;
	return UJsonBlueprintFunctionLibrary::FromFile(WorldContextObject, JsonFilePath, OutJsonObjectWrapper);
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
	bool Result = false;
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

bool UKinetixDataBlueprintFunctionLibrary::GenerateMetadataAssets(UObject* WorldContextObject,
                                                                  TArray<FString>& MetadataFiles)
{
	if (!IsValid(WorldContextObject))
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("World Context is null !"));
		return false;
	}

	FAssetToolsModule& AssetToolsModule = FAssetToolsModule::GetModule();
	FAssetRegistryModule& AssetRegistryModule =
		FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	bool bMetadataCreated = false;

	FAnimationMetadata AnimationMetadata;
	for (int i = 0; i < MetadataFiles.Num(); ++i)
	{
		if (!GetAnimationMetadataFromFile(
			WorldContextObject, MetadataFiles[i], AnimationMetadata))
		{
			UE_LOG(LogKinetixRuntime, Warning, TEXT("Unable to find any metadata from file %s !"), *MetadataFiles[i]);
			continue;
		}

		FString Filepath = MetadataFiles[i];
		FPaths::MakePathRelativeTo(Filepath, *FPaths::ProjectPluginsDir());

		// Add a '/' at the beginning to avoid due to the removal by MakePathRelativeTo
		Filepath = FString::Printf(TEXT("/%s"), *Filepath);

		if (!RemoveContentFromPluginPath(Filepath))
		{
			UE_LOG(LogKinetixRuntime, Warning, TEXT("Wrong path for %s"), *Filepath);
			continue;
		}

		FString PackageName = FPackageName::ObjectPathToPackageName(Filepath);
		const FString PackagePath = FPackageName::GetLongPackagePath(PackageName);

		FAssetData AssetData =
			AssetRegistryModule.Get().GetAssetByObjectPath(
				FSoftObjectPath(PackageName + "." + AnimationMetadata.Name.ToString()));
		if (AssetData.IsValid())
		{
			UE_LOG(LogKinetixRuntime, Warning, TEXT("%s already exist, continuing..."),
			       *AnimationMetadata.Name.ToString());
			bMetadataCreated = true;
			continue;
		}

		UPackage* Package = CreatePackage(*PackageName);
		UAnimationMetadataAsset* AnimationMetadataAsset = Cast<UAnimationMetadataAsset>(
			AssetToolsModule.Get().CreateAsset(
				AnimationMetadata.Name.ToString(),
				*PackagePath,
				UAnimationMetadataAsset::StaticClass(),
				nullptr)
		);

		if (!IsValid(AnimationMetadataAsset))
		{
			UE_LOG(LogKinetixRuntime, Warning, TEXT("%s: Unable to create asset !"),
			       *AnimationMetadata.Name.ToString());
			continue;
		}

		AnimationMetadataAsset->AnimationMetadata = AnimationMetadata;

		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.Error = GWarn;
		SaveArgs.SaveFlags = SAVE_None;

		// To have the asset on disk, LongPackageNameToFilename give the absolute path
		// which is required to save the asset on disk
		FSavePackageResultStruct SaveResult = UPackage::Save(Package, AnimationMetadataAsset,
		                                                     *FPackageName::LongPackageNameToFilename(
			                                                     PackageName, FPackageName::GetAssetPackageExtension()),
		                                                     SaveArgs);

		if (!SaveResult.IsSuccessful())
		{
			UE_LOG(LogKinetixRuntime, Warning, TEXT("Failed to save %s !"),
			       *AnimationMetadata.Name.ToString());
			continue;
		}

		UE_LOG(LogKinetixRuntime, Log, TEXT("Asset created %s"), *Filepath);
		FAssetRegistryModule::AssetCreated(AnimationMetadataAsset);
		bMetadataCreated = true;
	}

	return bMetadataCreated;
}

bool UKinetixDataBlueprintFunctionLibrary::LoadReferenceSkeletonAsset(const FReferenceSkeletonLoadedDelegate& Callback)
{
	FAssetRegistryModule& AssetRegistryModule =
		FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	// Normalize path
	FString GeneralPath = FKinetixRuntimeModule::GetPlugin()->GetContentDir() + TEXT("/Characters/Sam/");
	GeneralPath = FPaths::CreateStandardFilename(GeneralPath);
	GeneralPath = FPaths::GetPath(GeneralPath);

	// Format package path
	FString SkeletalMeshPackagePath = GeneralPath;
	FPaths::MakePathRelativeTo(SkeletalMeshPackagePath, *FPaths::ProjectPluginsDir());
	RemoveContentFromPluginPath(SkeletalMeshPackagePath);
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

bool UKinetixDataBlueprintFunctionLibrary::GenerateAnimationAssets(UObject* WorldContextObject,
                                                                   USkeletalMesh* InSkeletalMesh)
{
	FAssetRegistryModule& AssetRegistryModule =
		FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	// Normalize path
	FString GeneralPath = FKinetixRuntimeModule::GetPlugin()->GetContentDir() + TEXT("/FreeAnimations/");
	GeneralPath = FPaths::CreateStandardFilename(GeneralPath);
	GeneralPath = FPaths::GetPath(GeneralPath);

	// Format package path
	FString AnimationRootPath = GeneralPath;
	FPaths::MakePathRelativeTo(AnimationRootPath, *FPaths::ProjectPluginsDir());
	RemoveContentFromPluginPath(AnimationRootPath);
	AnimationRootPath = FString::Printf(TEXT("/%s/"), *AnimationRootPath);

	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	TArray<FString> FoundGLBs;
	FileManager.FindFilesRecursively(FoundGLBs, *GeneralPath, TEXT(".glb"));

	if (FoundGLBs.IsEmpty())
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("No .glb files found !"));
		return false;
	}

	for (int i = 0; i < FoundGLBs.Num(); ++i)
	{
		UE_LOG(LogKinetixRuntime, Log, TEXT("Found .glb: %s"), *FoundGLBs[i]);

		FglTFRuntimeConfig GlbLoaderConfig;
		GlbLoaderConfig.TransformBaseType = EglTFRuntimeTransformBaseType::YForward;

		UglTFRuntimeAsset* GlbAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilename(
			FoundGLBs[i], false, GlbLoaderConfig);
		if (!IsValid(GlbAsset))
		{
			UE_LOG(LogKinetixRuntime, Warning, TEXT("%s Glb asset unable to be loaded !"), *FoundGLBs[i]);
			continue;
		}

		FglTFRuntimeSkeletalAnimationConfig SkeletalAnimationConfig;
		UAnimSequence* GlbAnimation = GlbAsset->LoadSkeletalAnimation(InSkeletalMesh, 0, SkeletalAnimationConfig);

		if (!IsValid(GlbAnimation))
		{
			UE_LOG(LogKinetixRuntime, Warning, TEXT("%s Glb animation unable to be loaded !"), *FoundGLBs[i]);
			continue;
		}

		FString GlbFilename = FPaths::GetBaseFilename(FoundGLBs[i]);
		UE_LOG(LogKinetixRuntime, Log, TEXT("Glb loaded %s !"), *GlbFilename);

		FString PackageName = FPackageName::ObjectPathToPackageName(FoundGLBs[i]);
		FPaths::MakePathRelativeTo(PackageName, *FPaths::ProjectPluginsDir());
		PackageName = FString::Printf(TEXT("/%s"), *PackageName);
		RemoveContentFromPluginPath(PackageName);
		FString PackagePath = FPackageName::GetLongPackagePath(PackageName);
		
		FString AnimationFilename = FString::Printf(TEXT("A_%s"), *GlbFilename);
		PackageName = FString::Printf(TEXT("%s/%s"), *PackagePath, *AnimationFilename);
		PackageName = FPackageName::GetNormalizedObjectPath(PackagePath + TEXT("/") + AnimationFilename);

		FAssetData AssetData =
			AssetRegistryModule.Get().GetAssetByObjectPath(
				FSoftObjectPath(PackageName + "." + AnimationFilename));
		if (AssetData.IsValid())
		{
			UE_LOG(LogKinetixRuntime, Warning, TEXT("%s already exist, continuing..."),
				   *AnimationFilename);
			continue;
		}
		
		UPackage* AnimationPackage = CreatePackage(*PackageName);
		if (!IsValid(AnimationPackage))
		{
			UE_LOG(LogKinetixRuntime, Warning, TEXT("%s unable to create package !"), *FoundGLBs[i]);
			continue;
		}

		if (!GlbAnimation->Rename(*AnimationFilename, AnimationPackage))
		{
			UE_LOG(LogKinetixRuntime, Warning, TEXT("%s unable to rename animation %s !"), *FoundGLBs[i], *GlbFilename);
			continue;
		}

		FString PackageFilename = FPackageName::LongPackageNameToFilename(
			PackageName, FPackageName::GetAssetPackageExtension());

		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.Error = GWarn;
		SaveArgs.SaveFlags = SAVE_None;

		FSavePackageResultStruct SaveResult =
			UPackage::Save(
				AnimationPackage, GlbAnimation,
				*PackageFilename,
				SaveArgs);

		if (!SaveResult.IsSuccessful())
		{
			UE_LOG(LogKinetixRuntime, Warning, TEXT("%s unable to save package %s !"), *FoundGLBs[i],
			       *AnimationFilename);
			continue;
		}

		AssetRegistryModule.Get().AssetCreated(GlbAnimation);
	}

	return true;
}
