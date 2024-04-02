// // Copyright Kinetix. All Rights Reserved.


#include "Core/KinetixCoreSubsystem.h"

#include "glTFRuntimeFunctionLibrary.h"
#include "KinetixDeveloperSettings.h"
#include "KinetixRuntimeModule.h"
#include "Core/Account/KinetixAccount.h"
#include "Core/Animation/KinetixAnimation.h"
#include "Core/Metadata/KinetixMetadata.h"
#include "Data/KinetixDataLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

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

bool UKinetixCoreSubsystem::InitializeSubcore(const UClass* SubcoreClass, UObject** OutSubcore)
{
	UObject* SubcoreObject = NewObject<UObject>(this, SubcoreClass);
	if (!IsValid(SubcoreObject) || !SubcoreObject->Implements<UKinetixSubcoreInterface>())
		return false;
	
	bool bInterfaceInitialized = false;
	IKinetixSubcoreInterface::Execute_Initialize(SubcoreObject, CoreConfiguration, bInterfaceInitialized);
	if(!bInterfaceInitialized)
		return false;

	*OutSubcore = SubcoreObject;
	
	return true;
}

bool UKinetixCoreSubsystem::Setup(const FKinetixCoreConfiguration& InConfiguration)
{
	if (bCoreInitialized) return true;

	CoreConfiguration = InConfiguration;

	UObject* SubcoreObject = nullptr;
	if (!InitializeSubcore(UKinetixAnimation::StaticClass(), &SubcoreObject))
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("KinetixAnimation failed to initialize !"));
		return false;
	}
	KinetixAnimation = Cast<UKinetixAnimation>(SubcoreObject);

	SubcoreObject = nullptr;
	if (!InitializeSubcore(UKinetixMetadata::StaticClass(), &SubcoreObject))
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("KinetixMetadata failed to initialize !"));
		return false;
	}
	KinetixMetadata = Cast<UKinetixMetadata>(SubcoreObject);

	SubcoreObject = nullptr;
	if (!InitializeSubcore(UKinetixAccount::StaticClass(), &SubcoreObject))
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("KinetixAccount failed to initialize !"));
		return false;
	}
	KinetixAccount = Cast<UKinetixAccount>(SubcoreObject);

	SubcoreObject = nullptr;
	if (!InitializeSubcore(UKinetixUGC::StaticClass(), &SubcoreObject))
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("KinetixUGC failed to initialize !"));
		return false;
	}
	KinetixUGC = Cast<UKinetixUGC>(SubcoreObject);
	bCoreInitialized = true;
	
	for (int i = 0; i < OnCoreInitializedDelegates.Num(); ++i)
	{
		OnCoreInitializedDelegates[i].Execute();
	}

	UE_LOG(LogKinetixRuntime, Log, TEXT("%s successfully initialized !"), *GetName());
	
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
