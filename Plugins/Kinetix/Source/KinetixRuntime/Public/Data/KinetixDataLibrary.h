// // Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KinetixDataLibrary.generated.h"

UENUM(Category="Kinetix|Animation")
enum class EOwnership :uint8
{
	O_None = 0	UMETA(DisplayName = "None"),
	O_Free		UMETA(DisplayNAme="Free"),
	O_Owner		UMETA(DisplayNAme="Owner"),
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

#pragma region URLs

	UPROPERTY(EditAnywhere)
	FURL AnimationURL;
	UPROPERTY(EditAnywhere)
	FURL IconURL;

#pragma endregion

	FAnimationMetadata()
		: Ownership(EOwnership::O_None)
	{}
};

USTRUCT(BlueprintType, Category="Kinetix|Animation")
struct FAnimationMetadataTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAnimationMetadata AnimationMetadata;
};
