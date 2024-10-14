// Copyright Kinetix. All Rights Reserved.

#include "Managers/MemoryManager.h"

#include "Kismet/GameplayStatics.h"
#include "SmartCache/KinetixCacheSaveGame.h"

DEFINE_LOG_CATEGORY(LogKinetixMemory);

TUniquePtr<FMemoryManager> FMemoryManager::Instance(nullptr);

FMemoryManager::FMemoryManager()
{
	LoadManifest();
}

FMemoryManager::~FMemoryManager()
{
	Instance = nullptr;
}

FMemoryManager& FMemoryManager::Get()
{
	if (!Instance.IsValid())
	{
		Instance = MakeUnique<FMemoryManager>();
		Instance->LoadManifest();
	}

	return *Instance;
}

void FMemoryManager::OnAnimationLoadedOnPlayer(const FAnimationID& InID)
{
	if (!IsValid(CacheManifest))
		return;

	if (!CacheManifest->Contains(InID))
	{
		if (CacheManifest->IsFull())
		{
			CacheManifest->Reorder();
			CacheManifest->RemoveLasts();
		}
		CacheManifest->Add(InID);
	}

	CacheManifest->Reorder();

	SaveManifest();
}

void FMemoryManager::OnAnimationUnloadedOnPlayer(const FAnimationID& InID)
{
	if (!CacheManifest->Contains(InID))
		return;

	FString Path;
	UKinetixDataBlueprintFunctionLibrary::GetCacheAnimationPath(Path, InID);

	if (!FPaths::FileExists(Path))
		return;

	IFileManager::Get().Delete(*Path);

	CacheManifest->Remove(InID);
	CacheManifest->SaveCacheSave();
}

void FMemoryManager::SaveManifest()
{
	if (CacheManifest == nullptr)
	{
		UE_LOG(LogKinetixMemory, Warning, TEXT("SaveManifest: No manifest to save !"));
		CreateCacheSave();
		return;
	}

	CacheManifest->SaveCacheSave();
}

void FMemoryManager::ClearManifest()
{
	UGameplayStatics::DeleteGameInSlot(KINETIXSMARTCACHESLOT, 0);
	Instance = nullptr;
}

const UKinetixCacheSaveGame& FMemoryManager::GetManifest()
{
	if (!IsValid(CacheManifest))
	{
		CreateCacheSave();
	}

	return *CacheManifest;
}

void FMemoryManager::LoadManifest()
{
	FAsyncLoadGameFromSlotDelegate LoadGameDelegate =
		FAsyncLoadGameFromSlotDelegate::CreateRaw(this, &FMemoryManager::OnManifestLoaded);

	// Empty the manifest
	CacheManifest = nullptr;

	UGameplayStatics::AsyncLoadGameFromSlot(KINETIXSMARTCACHESLOT, 0, LoadGameDelegate);
}

void FMemoryManager::LoadManifestSync()
{
	FAsyncLoadGameFromSlotDelegate LoadGameDelegate =
	FAsyncLoadGameFromSlotDelegate::CreateRaw(this, &FMemoryManager::OnManifestLoaded);

	// Empty the manifest
	CacheManifest = nullptr;

	USaveGame* SaveGame = UGameplayStatics::LoadGameFromSlot(KINETIXSMARTCACHESLOT, 0);
	OnManifestLoaded(KINETIXSMARTCACHESLOT, 0, SaveGame);
}

void FMemoryManager::OnManifestLoaded(const FString& SlotName, int UserIndex, USaveGame* SaveGame)
{
	UKinetixCacheSaveGame* KinetixCacheSaveGame = Cast<UKinetixCacheSaveGame>(SaveGame);
	if (KinetixCacheSaveGame == nullptr)
	{
		UE_LOG(LogKinetixSmartCache, Warning, TEXT("[UKinetixCacheSaveGame] OnManifestLoaded: No manifest on disk!"));
		// From this part we should create a new one

		CreateCacheSave();
		return;
	}

	CacheManifest = KinetixCacheSaveGame;
	CacheManifest->AddToRoot();

	FString Path;
	TArray<FAnimationID> AnimationsToRemove;
	for (const FKinetixCacheAnimation& CacheAnimation : CacheManifest->GetAnimations())
	{
		FAnimationID ID = CacheAnimation.GetID();

		UKinetixDataBlueprintFunctionLibrary::GetCacheAnimationPath(Path, ID);

		if (FPaths::FileExists(Path))
			continue;

		AnimationsToRemove.AddUnique(ID);
	}

	for (int i = 0; i < AnimationsToRemove.Num(); ++i)
	{
		CacheManifest->Remove(AnimationsToRemove[i]);
	}
}

void FMemoryManager::CreateCacheSave()
{
	CacheManifest = Cast<UKinetixCacheSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UKinetixCacheSaveGame::StaticClass()));

	if (!IsValid(CacheManifest))
		return;

	// Set up the (optional) delegate.
	FAsyncSaveGameToSlotDelegate KinetixCacheSavedDelegate;

	// USomeUObjectClass::SaveGameDelegateFunction is a void function that takes the following parameters: const FString& SlotName, const int32 UserIndex, bool bSuccess
	KinetixCacheSavedDelegate.BindRaw(this, &FMemoryManager::OnManifestCreated);

	// Start async save process.
	UGameplayStatics::AsyncSaveGameToSlot(
		CacheManifest, KINETIXSMARTCACHESLOT, 0, KinetixCacheSavedDelegate);
}

void FMemoryManager::OnManifestCreated(const FString& SlotName, int UserIndex, bool bSuccess)
{
	LoadManifest();
}
