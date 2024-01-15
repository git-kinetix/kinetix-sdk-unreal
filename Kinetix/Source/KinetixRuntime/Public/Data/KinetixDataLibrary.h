// // Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectWrapper.h"
#include "Engine/DataTable.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KinetixDataLibrary.generated.h"

// static FString* SDKAPIUrlBase;

static FString SDKAPIUsersUrl = TEXT("/v1/virtual-world/users");
#define SDKAPIEmoteUsersUrl TEXT("/v1/users/%s/emotes")
#define SDKAPIEmoteUrl TEXT("/v1/emotes/%s")
#define SDKAPIVirtualWorldEmoteUrl TEXT("/v1/virtual-world/emotes")
#define KINETIXSLOTNAME TEXT("KinetixSlot")
#define KINETIXUGCURL TEXT("/v1/process/token?userId=%s")
#define KINETIXUGCTOKEN TEXT("/v1/process/token/%s")
#define KINETIXALIASURL TEXT("/v1/virtual-world/alias/")

#define KINETIXSMARTCACHESLOT TEXT("KinetixSmartCacheSlot")

UENUM(Category="Kinetix|Animation")
enum class EOwnership : uint8
{
	O_None = 0 UMETA(DisplayName = "None"),
	O_Free UMETA(DisplayNAme="Free"),
	O_Owner UMETA(DisplayNAme="Owner"),
	O_Premium UMETA(DisplayName="Premium"),
	O_MAX UMETA(Hidden)
};

UENUM(Category="Kinetix|Animation")
enum class EEmoteType : uint8
{
	ET_None = 0 UMETA(DisplayName = "None"),
	ET_BackOffice UMETA(DisplayName = "Back Office"),
	ET_UGC UMETA(DisplayName = "User Generated")
};

UENUM(Category="Kinetix|Animation")
enum class EBlendState : uint8
{
	BS_None = 0 UMETA(DisplayName = "None"),
	BS_In UMETA(DisplayNAme="Blend In"),
	BS_Sample UMETA(DisplayNAme="Sample"),
	BS_Out UMETA(DisplayName="Blend Out"),
	BS_MAX UMETA(Hidden)
};

USTRUCT(BlueprintType, Category="Kinetix|Animation")
struct FAnimationID
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FGuid UUID;

	FAnimationID()
	{
		UUID.Invalidate();
	}

	// Needed to be able to use as key in TMap (1/2)
	bool operator==(const FAnimationID& Other) const
	{
		return UUID == Other.UUID;
	}

	bool operator!=(const FAnimationID& Other) const
	{
		return UUID != Other.UUID;
	}
};

// Needed to be able to use as key in TMap (2/2)
FORCEINLINE uint32 GetTypeHash(const FAnimationID& AnimationID)
{
	uint32 Hash = FCrc::MemCrc32(&AnimationID, sizeof(FAnimationID));
	return Hash;
}

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EEmoteType Type;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FDateTime CreatedAt;

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

	FAnimationMetadata& operator=(const FAnimationMetadata& Other)
	{
		if (this == &Other)
		{
			return *this;
		}

		Ownership = Other.Ownership;
		Id = Other.Id;
		Name = Other.Name;
		Description = Other.Description;
		Duration = Other.Duration;
		Type = Other.Type;
		CreatedAt = Other.CreatedAt;
		AnimationURL = Other.AnimationURL;
		IconURL = Other.IconURL;

		return *this;
	}

	// Needed to be usable in TSet
	const bool operator==(const FAnimationMetadata& Other) const
	{
		return Id == Other.Id;
	}
};

// Needed to be able to use as key in TMap (2/2)
FORCEINLINE uint32 GetTypeHash(const FAnimationMetadata& AnimationMetadata)
{
	return GetTypeHash(AnimationMetadata.Id);
}

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
		: bPlayAutomaticallyAnimationOnAnimInstances(true),
		  bEnableAnalytics(true),
		  bShowLogs(false),
		  VirtualWorld(FString())
	{
	}

	GENERATED_BODY()

	UPROPERTY()
	bool bPlayAutomaticallyAnimationOnAnimInstances;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableAnalytics;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShowLogs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 CachedEmotes;
	
	// TODO: Implement network structure
	// FKinetixNetworkConfiguration NetworkConfiguration

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Game API Key"))
	FString VirtualWorld;
};

USTRUCT(BlueprintType)
struct FAnimationQueue
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FAnimationID> AnimationIDs;

	UPROPERTY()
	TArray<UAnimSequence*> AnimationSequences;

	UPROPERTY()
	bool bLoop;

	FAnimationQueue(): bLoop(false)
	{
	}

	FAnimationQueue(const TArray<FAnimationID>& InAnimationIDs, bool bInLoop = false)
	{
		bLoop = bInLoop;
		AnimationIDs = InAnimationIDs;
		AnimationSequences.SetNumZeroed(AnimationIDs.Num());
	}
};

USTRUCT(BlueprintType)
struct FPlayerAnimSequenceMap
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	class UAnimSequenceSamplerComponent* PlayerAnimSequenceSampler;

	UPROPERTY(BlueprintReadWrite)
	TArray<UAnimSequenceSamplerComponent*> FakeAnimSequenceSampler;
};

#pragma region Delagates

#pragma region Animation

DECLARE_DYNAMIC_DELEGATE_OneParam(FReferenceSkeletonLoadedDelegate, FAssetData, AssetData);

DECLARE_DYNAMIC_DELEGATE(FOnKinetixAnimationPlayed);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRegisterLocalPlayer);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayedKinetixAnimationLocalPlayer, const FAnimationID&, AnimationID);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayedKinetixAnimationQueueLocalPlayer, const TArray<FAnimationID>&,
                                            AnimationIDs);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKinetixAnimationStartOnLocalPlayer, const FAnimationID&, AnimationID);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKinetixAnimationEndOnLocalPlayer, const FAnimationID&, AnimationID);

// Maybe adding a UEnum result for better understanding in case of error 
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnKinetixLocalAnimationLoadingFinished, bool, Success);

#pragma endregion

#pragma region Core
DECLARE_DYNAMIC_DELEGATE(FKinetixCoreInitializedDelegate);
#pragma endregion

#pragma region Account
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdatedAccount);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnAccountConnectedDelegate, bool, bInSuccess);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAccountConnected);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEmoteAssociated, const FString&, Response);
#pragma endregion

#pragma region Metadatas
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnMetadataAvailable, bool, bSuccess, const FAnimationMetadata&, AnimationData);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnMetadataOwnershipLoaded, bool, bSuccess, bool, bUserOwned);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnMetadatasAvailable, bool, bSuccess, const TArray<FAnimationMetadata>&, Metadatas);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnTotalNumberOfPagesAvailable, int, NumberOfPages);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnIconAvailable, UTexture2DDynamic*, Icon);

DECLARE_DYNAMIC_DELEGATE(FOnIconFail);

#pragma endregion

#pragma endregion

/**
* 
*/
UCLASS()
class KINETIXRUNTIME_API UKinetixDataBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UKinetixDataBlueprintFunctionLibrary();

public:
#pragma region Paths
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Data", meta = (Keywords = "data"))
	static UPARAM(DisplayName="Success") bool GetPluginRelativePath(FString& RelativePath);

	UFUNCTION(BlueprintCallable, Category = "Kinetix|Data", meta = (Keywords = "data"))
	static UPARAM(DisplayName="Success") bool RemoveContentFromPluginPath(FString& RelativePath, FString PathRoot);

	UFUNCTION(BlueprintCallable, Category = "Kinetix|Data", meta = (Keywords = "data"))
	static UPARAM(DisplayName="Success") bool GetCacheAnimationPath(FString& AbsolutePath, const FAnimationID& InAnimationMetadata);
	
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

	/** Helper function to retrieve animation metadata from a json object */
	static bool GetAnimationMetadataFromJson(const TSharedPtr<FJsonObject>& JsonObject,
	                                         FAnimationMetadata& AnimationMetadata);

	static bool GetAnimationMetadataFromJson(const FString& JsonString, FAnimationMetadata& OutAnimationMetadata);

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

	UFUNCTION(BlueprintPure, Category = "Kinetix|Data",
		meta = (Keywords = "id"))
	static UPARAM(DisplayName="Success") bool GetAnimationIDFromString(const FString& InAnimationID,
	                                                                   FAnimationID& OutAnimationID);

	UFUNCTION(BlueprintPure, Category = "Kinetix|Data",
		meta = (Keywords = "id"))
	static UPARAM(DisplayName="Success") bool GetStringFromAnimationID(const FAnimationID& InAnimationID,
	                                                                   FString& OutAnimationID);

	UFUNCTION(BlueprintCallable)
	static UPARAM(DisplayName="bValid") bool GetIconURL(const FAnimationMetadata& InAnimationMetadata,
	                                                    FString& OutIconURL);

	/** Returns true if Animation Metadata A refers to the same animation than B */
	UFUNCTION(BlueprintPure,
		meta=(DisplayName = "Equal (AnimationMetadata)", CompactNodeTitle = "==", ScriptOperator = "==", Keywords =
			"== equal"), Category="Kinetix|Metadata")
	static bool EqualEqual_AnimationMetadataAnimationMetadata(FAnimationMetadata A, FAnimationMetadata B);

	/** Returns true if the Metadata as a valid ID */
	UFUNCTION(BlueprintPure,
		meta=(DisplayName = "IsValid (AnimationMetadata)", CompactNodeTitle = "Valid", Keywords = "valid"),
		Category="Kinetix|Metadata")
	static bool IsValid(UPARAM(ref) FAnimationMetadata& InAnimationMetadata);

#pragma endregion

#pragma region General

	static bool LoadReferenceSkeletonAsset(const TDelegate<void(FAssetData)>& Callback);

	UFUNCTION(BlueprintPure, Category="Kinetix|Save")
	static FString GetKinetixSlotName();

#pragma endregion

};
