// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IPluginManager.h"
#include "Modules/ModuleManager.h"

class IPlugin;

DECLARE_LOG_CATEGORY_EXTERN(LogKinetixRuntime, Log, All);

class FKinetixRuntimeModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FString GetModuleName() { return TEXT("KinetixRuntime"); }
	static FString GetPluginName() { return TEXT("Kinetix"); }

	static FKinetixRuntimeModule& Get()
	{
		return FModuleManager::Get().GetModuleChecked<FKinetixRuntimeModule>(*GetModuleName());
	}
	
	static TSharedPtr<IPlugin> GetPlugin()
	{
		return IPluginManager::Get().FindPlugin(*GetPluginName());
	}

private:
	
	void OnAssetRegistryInitialized();
};
