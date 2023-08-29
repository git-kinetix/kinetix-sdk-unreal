// // Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/KinetixDataLibrary.h"
#include "Interfaces/KinetixSubcoreInterface.h"
#include "KinetixAnimation.generated.h"

class UKinetixCharacterComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogKinetixAnimation, Log, All);

class FLocalPlayerManager;

/**
 * 
 */
UCLASS(BlueprintType)
class KINETIXRUNTIME_API UKinetixAnimation
	: public UObject, public IKinetixSubcoreInterface
{
	GENERATED_BODY()

public:
	
	UKinetixAnimation();
	// Needed to have TUniquePtr<> with forward declared classes
	UKinetixAnimation(FVTableHelper& Helper);
	~UKinetixAnimation();

#pragma region IKinetixSubcoreInterface
	virtual void Initialize_Implementation(const FKinetixCoreConfiguration& CoreConfiguration, bool& bResult) override;
#pragma endregion

	void SetReferenceSkeletalMesh(USkeletalMesh* InSkeletalMesh);

	/**
	 * @brief Register the local player AnimInstance with Skeletal setup to play animation on it 
	 * @param InAnimInstance AnimInstance of your local character
	 */
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Animation")
	void RegisterLocalPlayerAnimInstance(UPARAM(ref) UAnimInstance* InAnimInstance);

	/**
	 * @brief Register the local player configuration for custom animation system.
	 * @param InAnimInstance AnimInstance of your character
	 * @param InRootTransform Root transform of your character
	 * @param InExportType The type of file for animations to export
	 */
	// UFUNCTION(BlueprintCallable, Category = "Kinetix|Animation")
	// void RegisterLocalPlayerCustomAnimInstance(UPARAM(ref) UAnimInstance* InAnimInstance, FTransform InRootTransform, EExportType InExportType);

	/**
	 * @brief Unregister the local player AnimInstance
	 */
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Animation")
	void UnregisterLocalPlayer();

	/**
	 * @brief Play animation on local player
	 * @param InAnimationID ID of the animation to play
	 */
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Animation")
	void PlayAnimationOnLocalPlayer(UPARAM(ref) const FAnimationID& InAnimationID);

	/**
	 * @brief Play animations on local player
	 * @param InAnimationIDs IDs of the animations
	 * @param bLoop Should loop the queue
	 */
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Animation")
	void PlayAnimationQueueOnLocalPlayer(TArray<FAnimationID>& InAnimationIDs, bool bLoop = false);

	/**
	 * @brief Stop animation on local player
	 */
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Animation")
	void StopAnimationOnLocalPlayer();

	/**
	 * @brief Load a local player animation
	 * @param InAnimationID ID of the animation
	 * @param OnSuccessDelegate Callback with loading result
	 */
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Animation")
	void LoadLocalPlayerAnimation(const FAnimationID& InAnimationID, FString& InLockID, const FOnKinetixLocalAnimationLoadingFinished& OnSuccessDelegate);

	/**
	 * @brief Load local player animations
	 * @param InAnimationIDs IDs of the animations
	 * @param OnSuccessDelegate Callback with loading result
	 */
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Animation")
	void LoadLocalPlayerAnimations(TArray<FAnimationID>& InAnimationIDs, const FOnKinetixLocalAnimationLoadingFinished& OnSuccessDelegate);

	/**
	 * @brief Unload a local player animation
	 * @param InAnimationID ID of the animation to unload
	 */
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Animation")
	void UnloadLocalPlayerAnimation(const FAnimationID& InAnimationID);

	/**
	 * @brief Unload local player animations
	 * @param AnimationIDs IDs of the animations to unload
	 */
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Animation")
	void UnloadLocalPlayerAnimations(TArray<FAnimationID>& AnimationIDs);

	/**
	 * @brief Check if an animation is available on local player
	 * @param InAnimationID ID of the animation to check
	 * @return True if animation available on local player
	 */
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Animation")
	UPARAM(DisplayName="Available") bool IsAnimationAvailableOnLocalPlayer(const FAnimationID& InAnimationID);

	/**
	 * @brief Getter for the register Kinetix Component
	 * @return The UKinetixComponent registered (if there is one)
	 */
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Animation")
	UKinetixCharacterComponent* GetLocalKCC() const;
	
public:
	/**
	 * @brief Event called when Local Player is registered
	 */
	UPROPERTY(BlueprintAssignable)
	FOnRegisterLocalPlayer OnRegisterLocalPlayer;

	/**
	 * @brief Event called when animation is played from local player
	 * @param AnimationId { UUID of the animation }
	 */
	UPROPERTY(BlueprintAssignable)
	FOnPlayedKinetixAnimationLocalPlayer OnPlayedKinetixAnimationLocalPlayer;

	/**
	 * @brief Event called when animation queue is played from local player
	 * @param AnimationIds { Array of animations UUIDs left }
	 */
	UPROPERTY(BlueprintAssignable)
	FOnPlayedKinetixAnimationQueueLocalPlayer OnPlayedKinetixAnimationQueueLocalPlayer;

	/**
	 * @brief Event called when animation start on animator
	 * @param AnimationId { UUID of the animation }
	 */
	UPROPERTY(BlueprintAssignable)
	FOnKinetixAnimationStartOnLocalPlayer OnKinetixAnimationStartOnLocalPlayer;

	/**
	 * @brief Event called when animation end on animator
	 * @param AnimationId { UUID of the animation }
	 */
	UPROPERTY(BlueprintAssignable)
	FOnKinetixAnimationEndOnLocalPlayer OnKinetixAnimationEndOnLocalPlayer;
	
private:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	UAnimInstance* AnimInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	USkeletalMesh* ReferenceSkeletalMesh;

	TUniquePtr<FLocalPlayerManager> LocalPlayerManager;

};
