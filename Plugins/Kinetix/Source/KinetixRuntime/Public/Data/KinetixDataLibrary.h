// // Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectWrapper.h"
#include "Engine/DataTable.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KinetixDataLibrary.generated.h"

UENUM(Category="Kinetix|Animation")
enum class EOwnership : uint8
{
	O_None = 0 UMETA(DisplayName = "None"),
	O_Free UMETA(DisplayNAme="Free"),
	O_Owner UMETA(DisplayNAme="Owner"),
	O_Premium UMETA(DisplayName="Premium"),
	O_MAX UMETA(Hidden)
};

USTRUCT(BlueprintType, Category="Kinetix|Animation")
struct FAnimationID
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FGuid UUID;
};

USTRUCT(BlueprintType)
struct FAnimationMetadata
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EOwnership Ownership;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(ShowOnlyInnerProperties))
	FAnimationID Id;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName Name;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FText Description;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Duration;

#pragma region URLs

	UPROPERTY(VisibleAnywhere)
	FURL AnimationURL;
	UPROPERTY(VisibleAnywhere)
	FURL IconURL;

#pragma endregion

	FAnimationMetadata()
		: Ownership(EOwnership::O_None), Duration(-1.f)
	{
	}
};

USTRUCT(BlueprintType, Category="Kinetix|Animation")
struct FAnimationMetadataTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAnimationMetadata AnimationMetadata;
};

USTRUCT(BlueprintType)
struct FKinetixCoreConfiguration
{
	FKinetixCoreConfiguration()
		: bPlayAutomaticallyAnimationOnAnimators(true),
		  MaxPersistentDataStorageInMB(50),
		  MaxRAMCacheInMB(50),
		  bEnableAnalytics(true),
		  bShowLogs(false)
	{}

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPlayAutomaticallyAnimationOnAnimators;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxPersistentDataStorageInMB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxRAMCacheInMB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableAnalytics;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShowLogs;

	// TODO: Implement network structure
	// FKinetixNetworkConfiguration NetworkConfiguration
	
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FReferenceSkeletonLoadedDelegate, FAssetData, AssetData);

/**
* 
*/
UCLASS()
class KINETIXRUNTIME_API UKinetixDataBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
#pragma region Paths
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Data", meta = (Keywords = "data"))
	static UPARAM(DisplayName="Success") bool GetPluginRelativePath(FString& RelativePath);

	UFUNCTION(BlueprintCallable, Category = "Kinetix|Data", meta = (Keywords = "data"))
	static UPARAM(DisplayName="Success") bool RemoveContentFromPluginPath(FString& RelativePath);
#pragma endregion

#pragma region Metadatas
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Data", meta = (Keywords = "data"))
	static UPARAM(DisplayName="Success") bool GetMetadataFiles(TArray<FString>& OutFiles,
	                                                           const FString InFileExtension = TEXT("json"));

	/** Helper function to retrieve animation metadata from a json file */
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Data",
		meta = (WorldContext = "WorldContextObject", Keywords = "data"))
	static UPARAM(DisplayName="Success") bool GetAnimationMetadataFromFile(UObject* WorldContextObject,
	                                                                       FString File,
	                                                                       FAnimationMetadata& AnimationMetadata);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Data",
		meta = (WorldContext = "WorldContextObject", Keywords = "data"))
	static UPARAM(DisplayName="Success") bool GetJsonObjectFromFile(UObject* WorldContextObject,
	                                                                FString File,
	                                                                FJsonObjectWrapper& OutJsonObjectWrapper);

	UFUNCTION(BlueprintCallable, Category = "Kinetix|Data", meta = (Keywords = "data"))
	static UPARAM(DisplayName="Success") bool GetUUIDFromJson(FGuid& OutUUID,
	                                                          const FJsonObjectWrapper& InJsonObjectWrapper);

	UFUNCTION(BlueprintCallable, Category = "Kinetix|Data",
		meta = (WorldContext = "WorldContextObject", Keywords = "data"))
	static UPARAM(DisplayName="Success") bool GetNameFromJson(FName& OutName,
	                                                          const FJsonObjectWrapper& InJsonObjectWrapper);

	UFUNCTION(BlueprintCallable, Category = "Kinetix|Data",
		meta = (WorldContext = "WorldContextObject", Keywords = "data"))
	static UPARAM(DisplayName="Success") bool GetOwnershipFromJson(EOwnership& OutOwnership,
	                                                               const FJsonObjectWrapper& InJsonObjectWrapper);

	UFUNCTION(BlueprintCallable, Category = "Kinetix|Data",
		meta = (WorldContext = "WorldContextObject", Keywords = "data"))
	static UPARAM(DisplayName="Success") bool GetDurationFromJson(float& OutDuration,
	                                                              const FJsonObjectWrapper& InJsonObjectWrapper);
#pragma endregion

#pragma region General
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Data",
		meta = (WorldContext = "WorldContextObject", Keywords = "data"))
	static UPARAM(DisplayName="Assets created") bool GenerateMetadataAssets(
		UObject* WorldContextObject, TArray<FString>& MetadataFiles);

	UFUNCTION(BlueprintCallable, Category = "Kinetix|Data",
		meta = (Keywords = "data|skeleton"))
	static UPARAM(DisplayName="Skeleton available") bool LoadReferenceSkeletonAsset(const FReferenceSkeletonLoadedDelegate& Callback);
	
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Data",
		meta = (WorldContext = "WorldContextObject" , Keywords = "data|skeleton"))
	static UPARAM(DisplayName="Animations created") bool GenerateAnimationAssets(UObject* WorldContextObject, USkeletalMesh* InSkeletalMesh);
	
#pragma endregion

};
