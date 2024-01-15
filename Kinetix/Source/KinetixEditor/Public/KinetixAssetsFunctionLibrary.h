// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KinetixAssetsFunctionLibrary.generated.h"

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
		meta = (WorldContext = "WorldContextObject", Keywords = "data|skeleton"))
	static UPARAM(DisplayName="Animations created") bool FindGLBandGenerateAnimationAssets(
		UObject* WorldContextObject, USkeletalMesh* InSkeletalMesh);
	
};
