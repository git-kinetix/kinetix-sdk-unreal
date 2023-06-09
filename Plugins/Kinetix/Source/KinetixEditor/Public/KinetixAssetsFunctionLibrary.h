// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KinetixAssetsFunctionLibrary.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FReferenceSkeletonLoadedDelegate, FAssetData, AssetData);

/**
 * 
 */
UCLASS()
class KINETIXEDITOR_API UKinetixAssetsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "Kinetix|Data",
	meta = (WorldContext = "WorldContextObject", Keywords = "data"))
	static UPARAM(DisplayName="Assets created") bool GenerateMetadataAssets(
		UObject* WorldContextObject, TArray<FString>& MetadataFiles);

	UFUNCTION(BlueprintCallable, Category = "Kinetix|Data",
		meta = (Keywords = "data|skeleton"))
	static UPARAM(DisplayName="Skeleton available") bool LoadReferenceSkeletonAsset(
		const FReferenceSkeletonLoadedDelegate& Callback);

	UFUNCTION(BlueprintCallable, Category = "Kinetix|Data",
		meta = (WorldContext = "WorldContextObject", Keywords = "data|skeleton"))
	static UPARAM(DisplayName="Animations created") bool GenerateAnimationAssets(
		UObject* WorldContextObject, USkeletalMesh* InSkeletalMesh);
	
};
