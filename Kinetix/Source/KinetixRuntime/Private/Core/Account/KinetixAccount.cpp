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
	UE_LOG(LogKinetixAccount, Log, TEXT("[UKinetixAccount] ~UKinetixAccount"));
	FAccountManager::Get()->StopPolling();
}

void UKinetixAccount::Initialize_Implementation(const FKinetixCoreConfiguration& CoreConfiguration, bool& bResult)
{
	FAccountManager::Get()->SetVirtualWorldID(CoreConfiguration.VirtualWorld);
	FAccountManager::Get()->OnConnectedAccount().AddUObject(this, &UKinetixAccount::ConnectedAccount);
	FAccountManager::Get()->OnUpdatedAccount().AddUObject(this, &UKinetixAccount::UpdatedAccount);
	FAccountManager::Get()->OnAssociatedEmote().AddUObject(this, &UKinetixAccount::AssociatedEmote);

	bResult = true;
}

void UKinetixAccount::ConnectAccount(const FString& InUserID)
{
	if (InUserID.IsEmpty())
	{
		UE_LOG(LogKinetixAccount, Warning, TEXT("ConnectAccount: InUserID is empty !"));
		return ;
	}

	FAccountManager::Get()->ConnectAccount(InUserID);
}

void UKinetixAccount::GetConnectedAccount(FName& OutUserName)
{
	FAccount* LoggedAccount = FAccountManager::Get()->GetConnectedAccount();
	if (LoggedAccount == nullptr)
		return;

	OutUserName = *LoggedAccount->GetAccountID();
}

void UKinetixAccount::DisconnectAccount()
{
	FAccountManager::Get()->DisconnectAccount();
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
