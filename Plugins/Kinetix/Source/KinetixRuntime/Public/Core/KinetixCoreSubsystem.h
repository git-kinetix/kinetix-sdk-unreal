// // Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KinetixCoreSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class KINETIXRUNTIME_API UKinetixCoreSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	
#pragma region USubsystem inheritance

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

#pragma endregion

private:

	
	
};
