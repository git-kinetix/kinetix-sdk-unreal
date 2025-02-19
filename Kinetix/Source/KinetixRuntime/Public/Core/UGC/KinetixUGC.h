// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/KinetixSubcoreInterface.h"
#include "Templates/UniquePtr.h"
#include "KinetixUGC.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogKinetixUGC, Log, All);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUGCUrlFetched, FString, Url);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUGCTokenStateFetched, bool, bTokenValid);

/**
 * 
 */
UCLASS()
class KINETIXRUNTIME_API UKinetixUGC
	: public UObject, public IKinetixSubcoreInterface
{
	GENERATED_BODY()

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
	void GetUGCUrl(const FOnUGCUrlFetched& UrlFetchedCallback);

	UFUNCTION()
	void OnUrlFetched(FString String);

	UFUNCTION(BlueprintCallable, Category = "Kinetix|UGC", meta=(WorldContext="WorldContextObject"))
	UTexture2D* GetQRCode(UObject* WorldContextObject, const FString& QrCodeContent);
		
private:

	UPROPERTY()
	FOnUGCUrlFetched UrlFetchedCallback;
	
	UPROPERTY()
	FOnUGCTokenStateFetched UGCTokenStateFetchedCallback;
};
