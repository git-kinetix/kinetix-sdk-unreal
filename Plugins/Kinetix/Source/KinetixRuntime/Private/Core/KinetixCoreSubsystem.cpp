// // Copyright Kinetix. All Rights Reserved.


#include "Core/KinetixCoreSubsystem.h"

#include "KinetixDeveloperSettings.h"
#include "KinetixRuntimeModule.h"
#include "Core/Animation/KinetixAnimation.h"
#include "Data/KinetixDataLibrary.h"

void UKinetixCoreSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UKinetixDeveloperSettings* KinetixSettings = GetDefault<UKinetixDeveloperSettings>();
	if (!IsValid(KinetixSettings))
	{
		UE_LOG(LogKinetixRuntime, Error, TEXT("ERROR: Unable to load settings!"));
		return;
	}

	if (!KinetixSettings->bLoadAtStartup) return;

	Setup();
}

void UKinetixCoreSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UKinetixCoreSubsystem::Setup(const FKinetixCoreConfiguration& InConfiguration)
{
	CoreConfiguration = InConfiguration;

	TArray<FString> MetadataFiles;
	if (!UKinetixDataBlueprintFunctionLibrary::GetMetadataFiles(MetadataFiles))
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("Unable to find any metadata files !"));
		return false;
	}

	if (!UKinetixDataBlueprintFunctionLibrary::GenerateMetadataAssets(this, MetadataFiles))
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("No Metadata files found !"));
		return false;
	}

	KinetixAnimation = NewObject<UKinetixAnimation>(this);

	FReferenceSkeletonLoadedDelegate Callback;
	Callback.BindUFunction(this, TEXT("OnReferenceSkeletonAvailable"));
	UKinetixDataBlueprintFunctionLibrary::LoadReferenceSkeletonAsset(Callback);

	bCoreInitialized = true;

	for (int i = 0; i < OnCoreInitializedDelegates.Num(); ++i)
	{
		OnCoreInitializedDelegates[i].Execute();
	}

	return true;
}

void UKinetixCoreSubsystem::RegisterOrCallOnInitialized(const FKinetixCoreInitializedDelegate& Callback)
{
	if (bCoreInitialized)
	{
		Callback.Execute();
	}

	OnCoreInitializedDelegates.AddUnique(Callback);
}

void UKinetixCoreSubsystem::OnReferenceSkeletonAvailable(FAssetData AssetData)
{
	if (!AssetData.IsValid() || !AssetData.IsAssetLoaded())
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("Asset still not loaded !"));
		return;
	}

	USkeletalMesh* LoadedSkeletalMesh = Cast<USkeletalMesh>(AssetData.GetAsset());
	if (!IsValid(LoadedSkeletalMesh))
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("Loaded asset is still null !"));
		return;
	}

	KinetixAnimation->SetReferenceSkeletalMesh(LoadedSkeletalMesh);
	
	UE_LOG(LogKinetixRuntime, Log, TEXT("%s asset loaded and available"), *LoadedSkeletalMesh->GetName());
}
