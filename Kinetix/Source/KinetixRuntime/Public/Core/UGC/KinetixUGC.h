// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/KinetixSubcoreInterface.h"
#include "Managers/UGCManager.h"
#include "Templates/UniquePtr.h"
#include "KinetixUGC.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUGCUrlFetched, FString, Url);

/**
 * 
 */
UCLASS()
class KINETIXRUNTIME_API UKinetixUGC
	: public UObject, public IKinetixSubcoreInterface
{
	GENERATED_BODY()

public:

	UKinetixUGC();
	UKinetixUGC(FVTableHelper& Helper);
	~UKinetixUGC();
	
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
	void GetUGCUrl(const FOnUGCUrlFetched& UrlFetchedCallback);

	UFUNCTION()
	void OnUrlFetched(FString String);
private:

	UPROPERTY()
	FOnUGCUrlFetched UrlFetchedCallback;
	
};
