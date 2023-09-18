// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/KinetixSubcoreInterface.h"
<<<<<<< Updated upstream:Kinetix/Source/KinetixRuntime/Public/Core/KinetixUGC.h
#include "UObject/NoExportTypes.h"
#include "KinetixUGC.generated.h"

=======
#include "Templates/UniquePtr.h"
#include "KinetixUGC.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogKinetixUGC, Log, All);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUGCUrlFetched, FString, Url);

>>>>>>> Stashed changes:Kinetix/Source/KinetixRuntime/Public/Core/UGC/KinetixUGC.h
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
<<<<<<< Updated upstream:Kinetix/Source/KinetixRuntime/Public/Core/KinetixUGC.h
	void GetUGCUrl() const;
=======
	void GetUGCUrl(const FOnUGCUrlFetched& UrlFetchedCallback);

	UFUNCTION()
	void OnUrlFetched(FString String);

	UFUNCTION(BlueprintCallable, Category = "Kinetix|UGC", meta=(WorldContext="WorldContextObject"))
	UTexture2D* GetQRCode(UObject* WorldContextObject, const FString& QrCodeContent);
		
private:

	UPROPERTY()
	FOnUGCUrlFetched UrlFetchedCallback;
>>>>>>> Stashed changes:Kinetix/Source/KinetixRuntime/Public/Core/UGC/KinetixUGC.h
	
private:
	GENERATED_BODY()
};
