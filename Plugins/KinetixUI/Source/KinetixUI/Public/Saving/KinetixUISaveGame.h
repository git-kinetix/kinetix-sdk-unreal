// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Data/KinetixDataLibrary.h"
#include "KinetixUISaveGame.generated.h"

/**
 * 
 */
UCLASS()
class KINETIXUI_API UKinetixUISaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI|Wheel")
	FName UserName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI|Wheel")
	TArray<FAnimationMetadata> AnimationMetadatas;
};
