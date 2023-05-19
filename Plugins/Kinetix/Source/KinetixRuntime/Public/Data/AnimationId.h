// // Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
// #include "Serialization/JsonSerializer.h"
#include "AnimationId.generated.h"

/**
 * 
 */
UCLASS()
class KINETIXRUNTIME_API UKinetixAnimationId: public UObject
{
	GENERATED_BODY()

public:
	
	UKinetixAnimationId(const FObjectInitializer& ObjectInitializer);

#pragma region FJsonSerializable inheritance
	// virtual void Serialize(FJsonSerializerBase& Serializer, bool bFlatObject) override;
#pragma endregion

public:
	
	FGuid UUID;
};
