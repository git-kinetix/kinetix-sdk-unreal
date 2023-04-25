// // Copyright Kinetix. All Rights Reserved.


#include "Core/Account/KinetixAccount.h"

#include "Managers/AccountManager.h"

UKinetixAccount::UKinetixAccount()
{
	AccountManager = MakeUnique<FAccountManager>();
}

UKinetixAccount::UKinetixAccount(FVTableHelper& Helper)
	:Super(Helper)
{}

UKinetixAccount::~UKinetixAccount()
{
	AccountManager = nullptr;
}

void UKinetixAccount::UpdatedAccount()
{
	OnUpdatedAccount.Broadcast();
}
