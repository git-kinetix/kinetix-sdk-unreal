// // Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/KinetixDataLibrary.h"
#include "Interfaces/KinetixSubcoreInterface.h"
#include "KinetixAccount.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogKinetixAccount, Log, All);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnOperationFinished, bool, bSuccess);

class FAccountManager;

/**
 * 
 */
UCLASS(BlueprintType)
class KINETIXRUNTIME_API UKinetixAccount
	: public UObject, public IKinetixSubcoreInterface
{
	GENERATED_BODY()

public:
	
	UKinetixAccount();
	// Needed to be able to use TUniquePtr<> with forward declared classes
	UKinetixAccount(FVTableHelper& Helper);
	~UKinetixAccount();

#pragma region IKinetixSubcoreInterface inheritance
	virtual void Initialize_Implementation(const FKinetixCoreConfiguration& CoreConfiguration, bool& bResult) override;
#pragma endregion

	UFUNCTION(BlueprintCallable, Category="Kinetix|Account")
	void ConnectAccount(const FString& InUserID);
		
	UFUNCTION(BlueprintCallable, Category="Kinetix|Account")
	void AssociateEmoteToUser(const FAnimationID& InAnimationID);

private:

	UFUNCTION()
	void UpdatedAccount();

	UFUNCTION()
	void ConnectedAccount();

	UFUNCTION()
	void AssociatedEmote(const FString& Response);

public:

	UPROPERTY(BlueprintAssignable)
	FOnUpdatedAccount OnUpdatedAccount;
	
	UPROPERTY(BlueprintAssignable)
	FOnAccountConnected OnConnectedAccount;

	UPROPERTY(BlueprintAssignable)
	FOnEmoteAssociated OnEmoteAssociated;

public:

	TUniquePtr<FAccountManager> AccountManager;

};
