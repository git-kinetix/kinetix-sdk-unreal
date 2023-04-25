// // Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "KinetixAccount.generated.h"

class FAccountManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdatedAccount);

/**
 * 
 */
UCLASS(BlueprintType)
class KINETIXRUNTIME_API UKinetixAccount : public UObject
{
	GENERATED_BODY()

public:
	
	UKinetixAccount();
	// Needed to be able to use TUniquePtr<> with forward declared classes
	UKinetixAccount(FVTableHelper& Helper);
	~UKinetixAccount();

private:

	void UpdatedAccount();

public:

	UPROPERTY(BlueprintAssignable)
	FOnUpdatedAccount OnUpdatedAccount;
	
private:

	TUniquePtr<FAccountManager> AccountManager;

};
