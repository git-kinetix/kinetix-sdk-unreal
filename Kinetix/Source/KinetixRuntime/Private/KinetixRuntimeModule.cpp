// Copyright Epic Games, Inc. All Rights Reserved.

#include "KinetixRuntimeModule.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Data/KinetixDataLibrary.h"

DEFINE_LOG_CATEGORY(LogKinetixRuntime);

#define LOCTEXT_NAMESPACE "FKinetixRuntimeModule"

void FKinetixRuntimeModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Load the asset registry module
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(
		FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	if (AssetRegistry.IsLoadingAssets())
	{
		AssetRegistry.OnFilesLoaded().AddRaw(this, &FKinetixRuntimeModule::OnAssetRegistryInitialized);
	}
	else
	{
		OnAssetRegistryInitialized();
	}
}

void FKinetixRuntimeModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

void FKinetixRuntimeModule::OnAssetRegistryInitialized()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FKinetixRuntimeModule, KinetixRuntime)