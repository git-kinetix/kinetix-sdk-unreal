// Copyright Kinetix. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_CLASS(LogKinetixEditor, Log, All);

class FKinetixEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

};