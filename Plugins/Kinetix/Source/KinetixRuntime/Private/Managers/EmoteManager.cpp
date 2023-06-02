// // Copyright Kinetix. All Rights Reserved.


#include "Managers/EmoteManager.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/AssetManager.h"

FEmoteManager::FEmoteManager()
{
}

FEmoteManager::~FEmoteManager()
{
}

FKinetixEmote* FEmoteManager::GetEmote(const FAnimationID& InAnimationID)
{
	if (!InAnimationID.UUID.IsValid())
		return nullptr;

	if (KinetixEmotes.Contains(InAnimationID))
		return &KinetixEmotes[InAnimationID];

	return &KinetixEmotes.Add(InAnimationID, FKinetixEmote(InAnimationID));
}

TArray<FKinetixEmote*> FEmoteManager::GetEmotes(TArray<FAnimationID>& InAnimationIDs)
{
	if (InAnimationIDs.IsEmpty())
		return TArray<FKinetixEmote*>();

	TArray<FKinetixEmote*> Emotes;
	Emotes.Reserve(InAnimationIDs.Num());

	for (int InAnimationIDIndex = 0; InAnimationIDIndex < InAnimationIDs.Num(); ++InAnimationIDIndex)
	{
		Emotes[InAnimationIDIndex] = GetEmote(InAnimationIDs[InAnimationIDIndex]);
	}
	return Emotes;
}

TArray<FKinetixEmote*> FEmoteManager::GetAllEmotes()
{
	return TArray<FKinetixEmote*>();
}

void FEmoteManager::GetAnimSequence(const FAnimationID& InAnimationID, const TDelegate<void(UAnimSequence*)>& OnSuccess)
{
	FAssetRegistryModule& AssetRegistryModule =
		FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	FKinetixEmote* Emote = GetEmote(InAnimationID);
	if (Emote == nullptr)
		return; // Failed on this case

	if (!Emote->IsLocal())
		return;

	FString PackagePath = Emote->GetPathToGlb();
	FPaths::MakePathRelativeTo(PackagePath, *FPaths::ProjectPluginsDir());
	UKinetixDataBlueprintFunctionLibrary::RemoveContentFromPluginPath(PackagePath);
	PackagePath = FString::Printf(TEXT("/%s"), *PackagePath);
	
	FARFilter Filter;
	Filter.ClassPaths.Add(UAnimSequence::StaticClass()->GetClassPathName());
	Filter.PackagePaths.Add(FName(PackagePath));
	Filter.bRecursivePaths = true;

	TArray<FAssetData> AssetDatas;
	if (!AssetRegistryModule.Get().GetAssets(Filter, AssetDatas))
		return;

	if (AssetDatas.Num() != 1)
		return;

	FStreamableDelegate Delegate = FStreamableDelegate::CreateRaw(
		this,
		&FEmoteManager::AnimSequenceAvailable,
		AssetDatas[0].ToSoftObjectPath(),
		OnSuccess);

	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		TArray{AssetDatas[0].ToSoftObjectPath()},
		Delegate
	);
}

void FEmoteManager::AnimSequenceAvailable(FSoftObjectPath SoftObjectPath, TDelegate<void(UAnimSequence*)> Delegate) const
{
	if (!SoftObjectPath.IsValid())
		return;

	UAnimSequence* AnimSequence = Cast<UAnimSequence>(SoftObjectPath.ResolveObject());
	if (!IsValid(AnimSequence))
		return;

	Delegate.Execute(AnimSequence);
}
