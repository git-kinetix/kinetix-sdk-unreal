// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/KinetixDataLibrary.h"
#include "GameFramework/SaveGame.h"
#include "SmartCache/KinetixCacheSaveGame.h"

DECLARE_LOG_CATEGORY_EXTERN(LogKinetixMemory, Log, All);

/**
 * 
 */
class FMemoryManager
{
public:
	FMemoryManager();
	~FMemoryManager();

	static FMemoryManager& Get();

	void OnAnimationLoadedOnPlayer(const FAnimationID& InID);

	void OnAnimationUnloadedOnPlayer(const FAnimationID& InID);
	
	void SaveManifest();

	void ClearManifest();

	const UKinetixCacheSaveGame& GetManifest();
	
private:
	void LoadManifest();
	void LoadManifestSync();
	void CreateCacheSave();
	void OnManifestCreated(const FString& SlotName, int UserIndex, bool bSuccess);
	void OnManifestLoaded(const FString& SlotName, int UserIndex, USaveGame* SaveGame);

private:

	static TUniquePtr<FMemoryManager> Instance;

	UKinetixCacheSaveGame* CacheManifest;
};
