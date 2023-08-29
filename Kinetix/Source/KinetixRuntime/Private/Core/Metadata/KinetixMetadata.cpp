// // Copyright Kinetix. All Rights Reserved.


#include "Core/Metadata/KinetixMetadata.h"

#include "KinetixRuntimeModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Core/KinetixCoreSubsystem.h"
#include "Core/Account/KinetixAccount.h"
#include "Data/AnimationMetadataAsset.h"
#include "Engine/AssetManager.h"
#include "Managers/AccountManager.h"
#include "Managers/EmoteManager.h"
#include "Managers/IconManager.h"
#include "Managers/MetadataOperationManager.h"
#include "Engine/Texture2DDynamic.h"

DEFINE_LOG_CATEGORY(LogKinetixMetadata);

UKinetixMetadata::UKinetixMetadata()
{
}

UKinetixMetadata::UKinetixMetadata(FVTableHelper& Helper)
{
}

UKinetixMetadata::~UKinetixMetadata()
{
	// EmoteManager = nullptr;
}

void UKinetixMetadata::Initialize_Implementation(const FKinetixCoreConfiguration& CoreConfiguration, bool& bResult)
{
	TArray<FSoftObjectPath> MetadatasSoftObjectPaths;
	GetMetadatasSoftObjectPaths(MetadatasSoftObjectPaths);

	UAssetManager& AssetManager = UAssetManager::Get();
	AssetManager.GetStreamableManager().RequestAsyncLoad(
		MetadatasSoftObjectPaths,
		FStreamableDelegate::CreateUObject(this,
		                                   &UKinetixMetadata::InitializeEmoteManager, MetadatasSoftObjectPaths));


	FEmoteManager::Get();

	bResult = true;
}

void UKinetixMetadata::GetAnimationMetadataByAnimationID(const FAnimationID& InID, const FOnMetadataAvailable& Callback)
{
	if (!InID.UUID.IsValid())
		return;

	const FKinetixEmote* Emote = FEmoteManager::Get().GetEmote(InID);
	if (Emote == nullptr)
		return;

	if (Emote->HasMetadata())
	{
		Callback.ExecuteIfBound(true, Emote->GetAnimationMetadata());
		return;
	}

	FMetadataOperationManager::GetAnimationMetadataOfEmote(Emote->GetAnimationMetadata(),
	                                                       TDelegate<void(FAnimationMetadata)>::CreateLambda(
		                                                       [Callback](const FAnimationMetadata& InAnimationMetadata)
		                                                       {
			                                                       Callback.ExecuteIfBound(true, InAnimationMetadata);
		                                                       }));
}

void UKinetixMetadata::IsAnimationOwnedByUser(const FAnimationID& InID, const FOnMetadataOwnershipLoaded& Callback)
{
}

void UKinetixMetadata::GetUserAnimationMetadatas(const FOnMetadatasAvailable& Callback)
{
	UKinetixCoreSubsystem* KinetixCore =
		UGameInstance::GetSubsystem<UKinetixCoreSubsystem>(
			GetWorld()->GetGameInstance());

	if (!IsValid(KinetixCore))
	{
		UE_LOG(LogKinetixMetadata, Warning, TEXT("Unable to find Kinetix Core !"));
		return;
	}

	KinetixCore->KinetixAccount->AccountManager->GetAllUserAnimationMetadatas(
		Callback, TDelegate<void()>());
}

void UKinetixMetadata::GetUserAnimationMetadatasByPage(int InCount, int InPageNumber,
                                                       const FOnMetadatasAvailable& Callback)
{
}

void UKinetixMetadata::GetUserAnimationMetadatasTotalPagesCount(int InCountPerPage,
                                                                const FOnTotalNumberOfPagesAvailable& Callback)
{
}

void UKinetixMetadata::LoadIconByAnimationID(const FAnimationID& InID, const FOnIconAvailable& OnIconAvailableDelegate)
{
	if (!InID.UUID.IsValid())
		return;

	UTexture2DDynamic* Icon = FIconManager::Get().GetIcon(InID, OnIconAvailableDelegate);
	if (IsValid(Icon))
	{
		OnIconAvailableDelegate.ExecuteIfBound(Icon);
		return;
	}
}

void UKinetixMetadata::ResolveMetadatas(TArray<FSoftObjectPath>& SoftObjectPaths,
                                        TArray<FAnimationMetadata>& Metadatas,
                                        TArray<FString>& OutLocalPaths)
{
	UAnimationMetadataAsset* CurrentMetadata = nullptr;
	OutLocalPaths.SetNumZeroed(SoftObjectPaths.Num());
	Metadatas.SetNumZeroed(SoftObjectPaths.Num());
	for (int i = 0; i < SoftObjectPaths.Num(); ++i)
	{
		CurrentMetadata = Cast<UAnimationMetadataAsset>(SoftObjectPaths[i].ResolveObject());
		UPackage* Package = CurrentMetadata->GetPackage();
		OutLocalPaths[i] = IsValid(Package)
			                   ? FPackageName::GetLongPackagePath
			                   (Package->GetLoadedPath().GetLocalFullPath())
			                   : FString();
		Metadatas[i] = CurrentMetadata->AnimationMetadata;
	}
}

void UKinetixMetadata::OnMetadatasAvailable(TArray<FSoftObjectPath> SoftObjectPaths,
                                            FOnMetadatasAvailable OnMetadatasAvailable)
{
	TArray<FAnimationMetadata> Metadatas;
	TArray<FString> Dummy;
	ResolveMetadatas(SoftObjectPaths, Metadatas, Dummy);
	OnMetadatasAvailable.ExecuteIfBound(true, Metadatas);
}

bool UKinetixMetadata::GetMetadatasSoftObjectPaths(TArray<FSoftObjectPath>& ObjectPaths)
{
	FAssetRegistryModule& AssetRegistry = FModuleManager::GetModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	FString GeneralPluginPath = FKinetixRuntimeModule::GetPlugin()->GetContentDir();
	GeneralPluginPath = FPaths::CreateStandardFilename(GeneralPluginPath);
	GeneralPluginPath = FPaths::GetPath(GeneralPluginPath);

	FString TopLevelObjectPath = GeneralPluginPath;
	FPaths::MakePathRelativeTo(TopLevelObjectPath, *FPaths::ProjectPluginsDir());
	UKinetixDataBlueprintFunctionLibrary::RemoveContentFromPluginPath(TopLevelObjectPath, TEXT("/Kinetix"));
	TopLevelObjectPath = FString::Printf(TEXT("/%s/"), *TopLevelObjectPath);

	FARFilter Filter;
	Filter.ClassPaths.Add(UAnimationMetadataAsset::StaticClass()->GetClassPathName());
	Filter.PackagePaths.Add(FName(TopLevelObjectPath));
	Filter.bRecursiveClasses = true;
	Filter.bRecursivePaths = true;

	TArray<FAssetData> AssetDatas;
	AssetRegistry.Get().GetAssets(Filter, AssetDatas);

	if (AssetDatas.IsEmpty())
		return false;

	for (int i = 0; i < AssetDatas.Num(); ++i)
	{
		ObjectPaths.Add(AssetDatas[i].GetSoftObjectPath());
	}

	return true;
}

void UKinetixMetadata::InitializeEmoteManager(TArray<FSoftObjectPath> SoftObjectPaths)
{
	if (SoftObjectPaths.IsEmpty())
	{
		return;
	}

	TArray<FAnimationMetadata> AnimationMetadatas;
	TArray<FString> LocalPaths;
	ResolveMetadatas(SoftObjectPaths, AnimationMetadatas, LocalPaths);

	FKinetixEmote* CurrentEmote = nullptr;
	FAnimationMetadata CurrentMetadata;
	for (int i = 0; i < AnimationMetadatas.Num(); ++i)
	{
		CurrentMetadata = AnimationMetadatas[i];
		CurrentEmote = FEmoteManager::Get().GetEmote(AnimationMetadatas[i].Id);
		if (!LocalPaths[i].IsEmpty())
		{
			CurrentEmote->SetLocalMetadata(CurrentMetadata, LocalPaths[i]);
			continue;
		}

		CurrentEmote->SetMetadata(CurrentMetadata);
	}
}
