// Copyright Kinetix. All Rights Reserved.


#include "KinetixAssetsFunctionLibrary.h"

#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "UObject/SavePackage.h"
#include "Misc/OutputDevice.h"
#include "Misc/FeedbackContext.h"

#include "glTFRuntimeAsset.h"
#include "glTFRuntimeParser.h"
#include "glTFRuntimeFunctionLibrary.h"

#include "KinetixEditorModule.h"

#include "KinetixRuntimeModule.h"
#include "Data/AnimationMetadataAsset.h"
#include "Data/KinetixDataLibrary.h"

bool UKinetixAssetsFunctionLibrary::GenerateMetadataAssets(UObject* WorldContextObject,
                                                           TArray<FString>& MetadataFiles)
{
	if (!IsValid(WorldContextObject))
	{
		UE_LOG(LogKinetixEditor, Warning, TEXT("World Context is null !"));
		return false;
	}

#if WITH_EDITOR
  	FAssetToolsModule& AssetToolsModule = FAssetToolsModule::GetModule();
#endif

	FAssetRegistryModule& AssetRegistryModule =
		FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	bool bMetadataCreated = false;

	FAnimationMetadata AnimationMetadata;
	for (int i = 0; i < MetadataFiles.Num(); ++i)
	{
		if (!UKinetixDataBlueprintFunctionLibrary::GetAnimationMetadataFromFile(
			WorldContextObject, MetadataFiles[i], AnimationMetadata))
		{
			UE_LOG(LogKinetixEditor, Warning, TEXT("Unable to find any metadata from file %s !"), *MetadataFiles[i]);
			continue;
		}

#if WITH_EDITOR
  		FString Filepath = MetadataFiles[i];
		FPaths::MakePathRelativeTo(Filepath, *FPaths::ProjectPluginsDir());

		// Add a '/' at the beginning to avoid due to the removal by MakePathRelativeTo
		Filepath = FString::Printf(TEXT("/%s"), *Filepath);

		if (!UKinetixDataBlueprintFunctionLibrary::RemoveContentFromPluginPath(Filepath, TEXT("/Kinetix")))
		{
			UE_LOG(LogKinetixEditor, Warning, TEXT("Wrong path for %s"), *Filepath);
			continue;
		}

		FString PackageName = FPackageName::ObjectPathToPackageName(Filepath);
		const FString PackagePath = FPackageName::GetLongPackagePath(PackageName);

		FAssetData AssetData =
			AssetRegistryModule.Get().GetAssetByObjectPath(
				FSoftObjectPath(PackageName + "." + AnimationMetadata.Name.ToString()));
		if (AssetData.IsValid())
		{
			UE_LOG(LogKinetixEditor, Warning, TEXT("%s already exist, continuing..."),
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
			UE_LOG(LogKinetixEditor, Warning, TEXT("%s: Unable to create asset !"),
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
			UE_LOG(LogKinetixEditor, Warning, TEXT("Failed to save %s !"),
			       *AnimationMetadata.Name.ToString());
			continue;
		}

		UE_LOG(LogKinetixEditor, Log, TEXT("Asset created %s"), *Filepath);
		FAssetRegistryModule::AssetCreated(AnimationMetadataAsset);
#endif


		bMetadataCreated = true;
	}

	return bMetadataCreated;
}

bool UKinetixAssetsFunctionLibrary::GenerateAnimationAssets(UObject* WorldContextObject,
                                                                   USkeletalMesh* InSkeletalMesh)
{
	FAssetRegistryModule& AssetRegistryModule =
		FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	// Normalize path
	FString GeneralPath = FKinetixRuntimeModule::GetPlugin()->GetContentDir() + TEXT("/FreeAnimations/");
	GeneralPath = FPaths::CreateStandardFilename(GeneralPath);
	GeneralPath = FPaths::GetPath(GeneralPath);

	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	TArray<FString> FoundGLBs;
	FileManager.FindFilesRecursively(FoundGLBs, *GeneralPath, TEXT(".glb"));

	if (FoundGLBs.IsEmpty())
	{
		UE_LOG(LogKinetixEditor, Warning, TEXT("No .glb files found !"));
		return false;
	}

	for (int i = 0; i < FoundGLBs.Num(); ++i)
	{
		UE_LOG(LogKinetixEditor, Log, TEXT("Found .glb: %s"), *FoundGLBs[i]);

		FglTFRuntimeConfig GlbLoaderConfig;
		GlbLoaderConfig.TransformBaseType = EglTFRuntimeTransformBaseType::YForward;

		UglTFRuntimeAsset* GlbAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilename(
			FoundGLBs[i], false, GlbLoaderConfig);
		if (!IsValid(GlbAsset))
		{
			UE_LOG(LogKinetixEditor, Warning, TEXT("%s Glb asset unable to be loaded !"), *FoundGLBs[i]);
			continue;
		}

		FString PackageName = FPackageName::ObjectPathToPackageName(FoundGLBs[i]);
		FPaths::MakePathRelativeTo(PackageName, *FPaths::ProjectPluginsDir());
		PackageName = FString::Printf(TEXT("/%s"), *PackageName);
		UKinetixDataBlueprintFunctionLibrary::RemoveContentFromPluginPath(PackageName, TEXT("/Kinetix"));
		FString PackagePath = FPackageName::GetLongPackagePath(PackageName);

		FString GlbFilename = FPaths::GetBaseFilename(FoundGLBs[i]);
		FString AnimationFilename = FString::Printf(TEXT("A_%s"), *GlbFilename);
		PackageName = FString::Printf(TEXT("%s/%s"), *PackagePath, *AnimationFilename);
		PackageName = FPackageName::GetNormalizedObjectPath(PackagePath + TEXT("/") + AnimationFilename);

		FAssetData AssetData =
			AssetRegistryModule.Get().GetAssetByObjectPath(
				FSoftObjectPath(PackageName + "." + AnimationFilename));
		if (AssetData.IsValid())
		{
			UE_LOG(LogKinetixEditor, Warning, TEXT("%s already exist, continuing..."),
			       *AnimationFilename);
			continue;
		}

		FglTFRuntimeSkeletalAnimationConfig SkeletalAnimationConfig;
		UAnimSequence* GlbAnimation = GlbAsset->LoadSkeletalAnimation(InSkeletalMesh, 0, SkeletalAnimationConfig);

		if (!IsValid(GlbAnimation))
		{
			UE_LOG(LogKinetixEditor, Warning, TEXT("%s Glb animation unable to be loaded !"), *FoundGLBs[i]);
			continue;
		}
		
#if WITH_EDITOR
		UPackage* AnimationPackage = CreatePackage(*PackageName);
		if (!IsValid(AnimationPackage))
		{
			UE_LOG(LogKinetixEditor, Warning, TEXT("%s unable to create package !"), *FoundGLBs[i]);
			continue;
		}

		if (!GlbAnimation->Rename(*AnimationFilename, AnimationPackage))
		{
			UE_LOG(LogKinetixEditor, Warning, TEXT("%s unable to rename animation %s !"), *FoundGLBs[i], *GlbFilename);
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
			UE_LOG(LogKinetixEditor, Warning, TEXT("%s unable to save package %s !"), *FoundGLBs[i],
			       *AnimationFilename);
			continue;
		}

		AssetRegistryModule.Get().AssetCreated(GlbAnimation);
#endif
		
	}

	return true;
}
