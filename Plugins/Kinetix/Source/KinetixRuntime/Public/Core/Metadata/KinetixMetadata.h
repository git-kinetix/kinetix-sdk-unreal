// // Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/KinetixDataLibrary.h"
#include "KinetixMetadata.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnMetadataAvailable, bool, bSuccess, FAnimationMetadata&, AnimationData);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnMetadataOwnershipLoaded, bool, bSuccess, bool, bUserOwned);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnMetadatasAvailable, bool, bSuccess, TArray<FAnimationMetadata>, Metadatas);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnTotalNumberOfPagesAvailable, int, NumberOfPages);

/**
 * 
 */
UCLASS(BlueprintType)
class KINETIXRUNTIME_API UKinetixMetadata : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * @brief Get Metadata of a specific animation
	 * @param InID ID of the animation
	 * @param Callback Called with a boolean indicating success of the operation and filled metadata struct
	 */
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Metadata")
	void GetAnimationMetadataByAnimationIDs(FAnimationID InID, const FOnMetadataAvailable& Callback);

	/**
	 * @brief Giving an animation ID, tell if the animation is owned by the local user or not
	 * @param InID ID of the animation
	 * @param Callback Called with 2 booleans indicating the success of the operation and if the user owned the animation
	 */
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Metadata")
	void IsAnimationOwnedByUser(FAnimationID InID, const FOnMetadataOwnershipLoaded& Callback);

	/**
	 * @brief Get all animation metadatas for the local user
	 * @param Callback Called with one boolean indicating the success of the operation and all the metadatas
	 */
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Metadata")
	void GetUserAnimationMetadatas(const FOnMetadatasAvailable& Callback);

	/**
	 * @brief Get a given number of animations per given page
	 * @param InCount Number of animations per page
	 * @param InPageNumber Index of page
	 * @param Callback Called with one boolean indicating the success of the operation and all the metadatas
	 */
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Metadata")
	void GetUserAnimationMetadatasByPage(int InCount, int InPageNumber, const FOnMetadatasAvailable& Callback);

	/**
	 * @brief Get the number of pages based on a given number of animations we want
	 * @param InCountPerPage Number of animation per page we want
	 * @param Callback Called with the number of pages that contains InCountPerPage animations
	 */
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Metadata")
	void GetUserAnimationMetadatasTotalPagesCount(int InCountPerPage, const FOnTotalNumberOfPagesAvailable& Callback);
};
