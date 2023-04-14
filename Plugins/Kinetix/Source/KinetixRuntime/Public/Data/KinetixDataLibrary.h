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
	O_None = 0	UMETA(DisplayName = "None"),
	O_Free		UMETA(DisplayNAme="Free"),
	O_Owner		UMETA(DisplayNAme="Owner"),
	O_Premium	UMETA(DisplayName="Premium"),
	O_MAX		UMETA(Hidden)
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EOwnership Ownership;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ShowOnlyInnerProperties))
	FAnimationID Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration;

#pragma region URLs

	UPROPERTY(EditAnywhere)
	FURL AnimationURL;
	UPROPERTY(EditAnywhere)
	FURL IconURL;

#pragma endregion

	FAnimationMetadata()
		: Ownership(EOwnership::O_None), Duration(-1.f)
	{}
};

USTRUCT(BlueprintType, Category="Kinetix|Animation")
struct FAnimationMetadataTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAnimationMetadata AnimationMetadata;
};

/**
* 
*/
UCLASS()
class KINETIXRUNTIME_API UKinetixDataBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Helper function to retrieve animation metadata from a json file */
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Data", meta = (WorldContext = "WorldContextObject", Keywords = "data"))
	static UPARAM(DisplayName="Success") bool GetAnimationMetadataFromFile(UObject* WorldContextObject,
		FString File, FAnimationMetadata& AnimationMetadata);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Data", meta = (WorldContext = "WorldContextObject", Keywords = "data"))
	static UPARAM(DisplayName="Success") bool GetJsonObjectFromFile(UObject* WorldContextObject,
		FString File, FJsonObjectWrapper& OutJsonObjectWrapper);

	UFUNCTION(BlueprintCallable, Category = "Kinetix|Data", meta = (Keywords = "data"))
	static UPARAM(DisplayName="Success") bool GetUUIDFromJson(FGuid& OutUUID,
		const FJsonObjectWrapper& InJsonObjectWrapper);

	UFUNCTION(BlueprintCallable, Category = "Kinetix|Data", meta = (WorldContext = "WorldContextObject", Keywords = "data"))
	static UPARAM(DisplayName="Success") bool GetNameFromJson(FName& OutName,
		const FJsonObjectWrapper& InJsonObjectWrapper);

	UFUNCTION(BlueprintCallable, Category = "Kinetix|Data", meta = (WorldContext = "WorldContextObject", Keywords = "data"))
	static UPARAM(DisplayName="Success") bool GetOwnershipFromJson(EOwnership& OutOwnership,
		const FJsonObjectWrapper& InJsonObjectWrapper);
	
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Data", meta = (WorldContext = "WorldContextObject", Keywords = "data"))
	static UPARAM(DisplayName="Success") bool GetDurationFromJson(float& OutDuration,
		const FJsonObjectWrapper& InJsonObjectWrapper);

};

