// // Copyright Kinetix. All Rights Reserved.


#include "Core/Network/KinetixNetwork.h"

void UKinetixNetwork::SetConfiguration(FKinetixNetworkConfiguration& InNetworkConfiguration)
{
}

// const FKinetixNetworkConfiguration& UKinetixNetwork::GetConfiguration()
// {
// 	return NetworkConfiguration;
// }

UKinetixComponent* UKinetixNetwork::GetRemoteKCC(FString InRemotePeerID)
{
	return nullptr;
}

void UKinetixNetwork::RegisterRemotePeerAnimInstance(FString InRemotePeerID, UAnimInstance* InAnimInstance)
{
}

void UKinetixNetwork::UnregisterRemotePeer(FString InRemotePeer)
{
}

void UKinetixNetwork::UnregisterAllRemotePeers()
{
}
