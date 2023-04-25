// // Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "KinetixNetworkConfiguration.h"
#include "UObject/NoExportTypes.h"
#include "KinetixNetwork.generated.h"

class UKinetixComponent;
/**
 * 
 */
UCLASS()
class KINETIXRUNTIME_API UKinetixNetwork : public UObject
{
	GENERATED_BODY()

	/**
	 * @brief Set the current Kinetix Network Configuration
	 * @param InNetworkConfiguration Reference to the configuration we want
	 */
	UFUNCTION(BlueprintCallable, Category="Kinetix|Network")
	void SetConfiguration(FKinetixNetworkConfiguration& InNetworkConfiguration);

	/**
	 * @brief Returns the current Kinetix Network Configuration
	 * @return A reference to the current configuration
	 */
	UFUNCTION(BlueprintCallable, Category="Kinetix|Network")
	static const FKinetixNetworkConfiguration& GetConfiguration();

	/**
	 * @brief Returns the UKinetixComponent for a remote peer
	 * @param InRemotePeerID The remote peer we want to get the component from
	 * @return The UKinetixComponent if he has one
	 */
	UFUNCTION(BlueprintCallable, Category="Kinetix|Network")
	UKinetixComponent* GetRemoteKCC(FString InRemotePeerID);

	/**
	 * @brief Register remote peer anim instance
	 * @param InRemotePeerID ID of the remote peer in the room
	 * @param InAnimInstance AnimInstance of the remote peer
	 */
	UFUNCTION(BlueprintCallable, Category="Kinetix|Network")
	void RegisterRemotePeerAnimInstance(FString InRemotePeerID, UAnimInstance* InAnimInstance);

	/**
	 * @brief Unregister a remote peer
	 * @param InRemotePeer ID of the remote peer in the room
	 */
	UFUNCTION(BlueprintCallable, Category="Kinetix|Network")
	void UnregisterRemotePeer(FString InRemotePeer);

	/**
	 * @brief Unregister all remote peers
	 */
	UFUNCTION(BlueprintCallable, Category="Kinetix|Network")
	void UnregisterAllRemotePeers();
};
