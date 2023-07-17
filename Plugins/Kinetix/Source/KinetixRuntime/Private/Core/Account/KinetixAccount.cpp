// Copyright Kinetix. All Rights Reserved.

#include "Core/Account/KinetixAccount.h"

#include "Managers/AccountManager.h"

DEFINE_LOG_CATEGORY(LogKinetixAccount);

UKinetixAccount::UKinetixAccount()
{
}

UKinetixAccount::UKinetixAccount(FVTableHelper& Helper)
	:Super(Helper)
{}

UKinetixAccount::~UKinetixAccount()
{
	AccountManager = nullptr;
}

void UKinetixAccount::Initialize_Implementation(const FKinetixCoreConfiguration& CoreConfiguration, bool& bResult)
{
	AccountManager = MakeUnique<FAccountManager>(CoreConfiguration.VirtualWorld);
	AccountManager.Get()->OnConnectedAccount().AddUObject(this, &UKinetixAccount::ConnectedAccount);
	AccountManager.Get()->OnUpdatedAccount().AddUObject(this, &UKinetixAccount::UpdatedAccount);
	AccountManager.Get()->OnAssociatedEmote().AddUObject(this, &UKinetixAccount::AssociatedEmote);

	bResult = true;
}

void UKinetixAccount::ConnectAccount(const FString& InUserID)
{
	if (InUserID.IsEmpty())
	{
		UE_LOG(LogKinetixAccount, Warning, TEXT("ConnectAccount: InUserID is empty !"));
		return ;
	}

	AccountManager.Get()->ConnectAccount(InUserID);
}

void UKinetixAccount::AssociateEmoteToUser(const FAnimationID& InAnimationID)
{
	AccountManager.Get()->AssociateEmoteToUser(InAnimationID);
}

void UKinetixAccount::UpdatedAccount()
{
	OnUpdatedAccount.Broadcast();
}

void UKinetixAccount::ConnectedAccount()
{
	OnConnectedAccount.Broadcast();
}

void UKinetixAccount::AssociatedEmote(const FString& Response)
{
	OnEmoteAssociated.Broadcast(Response);
}
