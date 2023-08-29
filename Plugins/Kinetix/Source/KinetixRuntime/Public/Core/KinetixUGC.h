// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/KinetixSubcoreInterface.h"
#include "UObject/NoExportTypes.h"
#include "KinetixUGC.generated.h"

/**
 * 
 */
UCLASS()
class KINETIXRUNTIME_API UKinetixUGC
	: public UObject, public IKinetixSubcoreInterface
{
public:
	
#pragma region IKinetixSubcoreInterface
	virtual void Initialize_Implementation(
		const FKinetixCoreConfiguration& CoreConfiguration, bool& bResult) override;
#pragma endregion IKinetixSubcoreInterface

	UFUNCTION(BlueprintCallable, Category = "Kinetix|UGC")
	bool IsUGCAvailable() const;
	
	UFUNCTION(BlueprintCallable, Category = "Kinetix|UGC")
	void StartPollingForUGC() const;

	UFUNCTION(BlueprintCallable, Category = "Kinetix|UGC")
	void StartPollingForNewUGCToken() const;

	UFUNCTION(BlueprintCallable, Category = "Kinetix|UGC")
	void GetUGCUrl() const;
	
private:
	GENERATED_BODY()
};
