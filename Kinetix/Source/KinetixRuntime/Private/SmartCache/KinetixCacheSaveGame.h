// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "KinetixCacheAnimation.h"
#include "GameFramework/SaveGame.h"
#include "KinetixCacheSaveGame.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogKinetixSmartCache, Log, All);

DECLARE_DELEGATE_OneParam(FOnKinetixCacheLoadedDelegate, class UKinetixCacheSaveGame*);

/**
 * Equivalent of the KinetixCacheManifest
 * Using SaveGame system to be platform & authority agnostic
 */
UCLASS(BlueprintType)
class KINETIXRUNTIME_API UKinetixCacheSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UKinetixCacheSaveGame();
	
	bool IsFull() const;

	bool IsOverMaxCapacity() const;

	bool Contains(const FAnimationID& InID) const;

	void Add(const FAnimationID& InID);

	void RemoveLasts();

	void Remove(const FAnimationID& InID);

	void Reorder();

	static void LoadManifest(FOnKinetixCacheLoadedDelegate& InCallback);

	static void OnManifestCreated(const FString& SlotName, int UserIndex, bool bSuccess);

	static void CreateCacheSave();

	static void OnManifestLoaded(const FString& SlotName, const int32 UserIndex,
	                             USaveGame* ReturnedSaveGame);

	void SaveCacheSave();

	const TArray<FKinetixCacheAnimation>& GetAnimations() const;
	
public:

	UPROPERTY()
	TArray<FKinetixCacheAnimation> Animations;
	
};
