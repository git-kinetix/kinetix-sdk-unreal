// // Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/UGC/KinetixUGC.h"
#include "Data/KinetixDataLibrary.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KinetixCoreSubsystem.generated.h"

class UKinetixAccount;
class UKinetixMetadata;
class UKinetixAnimation;

/**
 * 
 */
UCLASS()
class KINETIXRUNTIME_API UKinetixCoreSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UKinetixCoreSubsystem()
		: bCoreInitialized(false)
	{
	}

#pragma region USubsystem inheritance

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	bool InitializeSubcore(const UClass* SubcoreClass, UObject** OutSubcore);

#pragma endregion

	UFUNCTION(BlueprintCallable, Category = "Initialization", meta=(DisplayName="Initialize"))
	UPARAM(DisplayName = "Success") bool Setup(const FKinetixCoreConfiguration& InConfiguration = FKinetixCoreConfiguration());	
	
	UFUNCTION(BlueprintCallable, Category = "Initialization")
	void RegisterOrCallOnInitialized(const FKinetixCoreInitializedDelegate& Callback);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Initialization")
	bool IsInitialized() { return bCoreInitialized; };

protected:

	UFUNCTION()
	void OnReferenceSkeletonAvailable(FAssetData AssetData);
	
public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UKinetixAnimation* KinetixAnimation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UKinetixMetadata* KinetixMetadata;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UKinetixAccount* KinetixAccount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UKinetixUGC* KinetixUGC;
	
private:
	UPROPERTY()
	TArray<FKinetixCoreInitializedDelegate> OnCoreInitializedDelegates;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bCoreInitialized;

	UPROPERTY()
	FKinetixCoreConfiguration CoreConfiguration;

};
