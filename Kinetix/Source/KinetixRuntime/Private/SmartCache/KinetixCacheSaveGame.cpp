// Copyright Kinetix. All Rights Reserved.


#include "SmartCache/KinetixCacheSaveGame.h"

#include "KinetixDeveloperSettings.h"
#include "Animation/AnimTrace.h"
#include "Kismet/GameplayStatics.h"
#include "Managers/MemoryManager.h"

DEFINE_LOG_CATEGORY(LogKinetixSmartCache);

UKinetixCacheSaveGame::UKinetixCacheSaveGame()
{
}

bool UKinetixCacheSaveGame::IsFull() const
{
	FKinetixCoreConfiguration CoreConfiguration;
	UKinetixDeveloperSettings::GetCoreConfiguration(CoreConfiguration);
	return CoreConfiguration.CachedEmotes == Animations.Num();
}

bool UKinetixCacheSaveGame::IsOverMaxCapacity() const
{
	FKinetixCoreConfiguration CoreConfiguration;
	UKinetixDeveloperSettings::GetCoreConfiguration(CoreConfiguration);
	return CoreConfiguration.CachedEmotes <= Animations.Num();
}

bool UKinetixCacheSaveGame::Contains(const FAnimationID& InID) const
{
	bool bResult = false;

	for (int i = 0; i < Animations.Num(); ++i)
	{
		if (Animations[i].GetID() != InID)
			continue;

		i = Animations.Num();
		bResult = true;
	}

	return bResult;
}

void UKinetixCacheSaveGame::Add(const FAnimationID& InID)
{
	Animations.Insert(FKinetixCacheAnimation(InID, FDateTime::Now()), 0);

	RemoveLasts();
}

void UKinetixCacheSaveGame::RemoveLasts()
{
	if (!IsOverMaxCapacity())
		return;

	do
	{
		Animations.RemoveAt(Animations.Num());
	}
	while (IsOverMaxCapacity());
}

void UKinetixCacheSaveGame::Remove(const FAnimationID& InID)
{
	for (int i = Animations.Num() - 1; i >= 0; --i)
	{
		if (Animations[i].GetID() != InID)
			continue;

		Animations.RemoveAt(i);
	}
}

void UKinetixCacheSaveGame::Reorder()
{
	FKinetixCacheAnimation tmpAnim;
	bool isOrdered = false;

	while (!isOrdered)
	{
		isOrdered = true;

		for (int i = 0; i < Animations.Num(); i++)
		{
			if (i < Animations.Num() - 1 && Animations[i + 1].GetScore() > Animations[i].GetScore())
			{
				tmpAnim = Animations[i];
				Animations[i] = Animations[i + 1];
				Animations[i + 1] = tmpAnim;
				isOrdered = false;
			}
		}
	}
}

void UKinetixCacheSaveGame::LoadManifest(FOnKinetixCacheLoadedDelegate& InCallback)
{
	FAsyncLoadGameFromSlotDelegate LoadGameDelegate =
		FAsyncLoadGameFromSlotDelegate::CreateStatic(&UKinetixCacheSaveGame::OnManifestLoaded);

	UGameplayStatics::AsyncLoadGameFromSlot(KINETIXSMARTCACHESLOT, 0, LoadGameDelegate);
}

void UKinetixCacheSaveGame::OnManifestCreated(const FString& SlotName, int UserIndex, bool bSuccess)
{
	if (!bSuccess)
	{
		if(UKinetixDeveloperSettings::GetLogFlag())
		UE_LOG(LogKinetixSmartCache, Error, TEXT("OnManifestCreated: Failed to save manifest on disk !"));
	}
}

void UKinetixCacheSaveGame::CreateCacheSave()
{
	if (UKinetixCacheSaveGame* SaveGameInstance = Cast<UKinetixCacheSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UKinetixCacheSaveGame::StaticClass())))
	{
		// Set up the (optional) delegate.
		FAsyncSaveGameToSlotDelegate KinetixCacheSavedDelegate;

		// USomeUObjectClass::SaveGameDelegateFunction is a void function that takes the following parameters: const FString& SlotName, const int32 UserIndex, bool bSuccess
		KinetixCacheSavedDelegate.BindStatic(&UKinetixCacheSaveGame::OnManifestCreated);

		// Start async save process.
		UGameplayStatics::AsyncSaveGameToSlot(
			SaveGameInstance, KINETIXSMARTCACHESLOT, 0, KinetixCacheSavedDelegate);
	}
}

void UKinetixCacheSaveGame::OnManifestLoaded(const FString& SlotName, const int32 UserIndex,
                                             USaveGame* ReturnedSaveGame)
{
	UKinetixCacheSaveGame* KinetixCacheSaveGame = Cast<UKinetixCacheSaveGame>(ReturnedSaveGame);
	if (KinetixCacheSaveGame == nullptr)
	{
		if(UKinetixDeveloperSettings::GetLogFlag())
		UE_LOG(LogKinetixSmartCache, Warning, TEXT("[UKinetixCacheSaveGame] OnManifestLoaded: No manifest on disk!"));
		// From this part we should create a new one

		CreateCacheSave();
	}
}

void UKinetixCacheSaveGame::SaveCacheSave()
{
	// Start async save process.
	UGameplayStatics::AsyncSaveGameToSlot(
		this, KINETIXSMARTCACHESLOT, 0);
}

const TArray<FKinetixCacheAnimation>& UKinetixCacheSaveGame::GetAnimations() const
{
	return Animations;
}

void UKinetixCacheSaveGame::ClearCache()
{
	FMemoryManager::Get().ClearManifest();

	FString GeneralPath = FPaths::ProjectDir() + "Kinetix/";

	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	if (!FileManager.DeleteDirectoryRecursively(*GeneralPath))
	{
		if(UKinetixDeveloperSettings::GetLogFlag())
		UE_LOG(LogKinetixSmartCache, Warning, TEXT("Failed to delete Smart Cache folder"));
	}
}
