// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/KinetixDataLibrary.h"
#include "KinetixCacheAnimation.generated.h"

/**
 * 
 */
USTRUCT()
struct FKinetixCacheAnimation
{
	GENERATED_BODY()
	
public:
	
	FKinetixCacheAnimation();
	FKinetixCacheAnimation(const FAnimationID& InID, const FDateTime& InDateTime);
	~FKinetixCacheAnimation();

	const FAnimationID& GetID() const;
	
	float GetScore() const;
	
private:

	UPROPERTY(VisibleAnywhere)
	FAnimationID ID;

	UPROPERTY()
	FDateTime LastTimePlayed;
	
};
