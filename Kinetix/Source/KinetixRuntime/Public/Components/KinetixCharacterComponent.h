// // Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "glTFRuntimeParser.h"
#include "Components/ActorComponent.h"
#include "Data/KinetixDataLibrary.h"
#include "Interfaces/KinetixAnimationInterface.h"
#include "Interfaces/KinetixSamplerInterface.h"
#include "KinetixCharacterComponent.generated.h"

class UAnimSequenceSamplerComponent;
class USkeletalMeshComponent;
class UPoseableMeshComponent;
class UAnimSamplerComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnimationStart, const FAnimationID&, AnimationID)	;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnimationEnd, const FAnimationID&, AnimationID);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFramePlayed);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent, DisplayName="KinetixComponent"))
class KINETIXRUNTIME_API UKinetixCharacterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UKinetixCharacterComponent();
	UKinetixCharacterComponent(FVTableHelper& Helper);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void CheckAnimInstanceToNotify(AActor* CurrentOwner);

	UFUNCTION(Client, Reliable)
	void PlayAnimation(const FAnimationID& InAnimationID, bool bLoop,
	                   const FOnPlayedKinetixAnimationLocalPlayer& OnPlayedAnimationDelegate);

	UFUNCTION(Server, Reliable)
	void ServerCalledFromClientWithData(const FString& DataToSend);

	void RegisterSampler(IKinetixSamplerInterface* AnimSequenceSamplerComponent);

	TScriptInterface<IKinetixAnimationInterface> GetAnimInstanceToNotify() const;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void CheckSkeletalMeshComponent();

	UFUNCTION(BlueprintCallable)
	void SetSkeletalMeshComponent(USkeletalMeshComponent* InSkeletalMeshComponent);

	UFUNCTION()
	void OnOwnerAnimationInitialized();

	UFUNCTION()
	FString RemapBones(const int32 NodeIndex, const FString& CurveName, const FString& Path, UObject* Context);
	void PlayAnimationOld(const FAnimationID& InAnimationID, bool bLoop,
	                      const FOnPlayedKinetixAnimationLocalPlayer& OnPlayedAnimationDelegate);

private:
	UFUNCTION()
	void OnKinetixAnimationEnded();

public:
	/**
	 * @brief Called when an animation start playing
	 */
	UPROPERTY(BlueprintAssignable, Category="Kinetix|Animation|Events")
	FOnAnimationStart OnAnimationStart;

	/**
	 * @brief Called when an animation stop playing
	 */
	UPROPERTY(BlueprintAssignable, Category="Kinetix|Animation|Events")
	FOnAnimationEnd OnAnimationEnd;

	/**
	 * @brief Called when an animation stop playing
	 */
	UPROPERTY(BlueprintAssignable, Category="Kinetix|Animation|Events")
	FOnFramePlayed OnFramePlayed;
	
	IKinetixSamplerInterface* AnimSampler;
	
	FKinetixCoreInitializedDelegate Callback;

private:
	UPROPERTY(VisibleAnywhere, meta=(DisplayName = "Skeletal Mesh in use"))
	USkeletalMeshComponent* OwnerSkeletalMeshComponent;

	UPROPERTY(EditAnywhere, Category="Kinetix|Animation", meta=(AllowPrivateAccess="true"))
	bool bRegisterPlayerOnLaunch;

	FTimerHandle CheckSkeletalMeshTimer;

	UPROPERTY()
	TScriptInterface<IKinetixAnimationInterface> AnimInstanceToNotify;

	UPROPERTY()
	FglTFRuntimeSkeletalAnimationConfig AnimConfig;

	UPROPERTY(Replicated)
	TObjectPtr<UAnimSequenceSamplerComponent> AnimSequenceSamplerComponent;

	FAnimationID CurrentAnimationIDBeingPlayed;

	FTimerHandle EndAnimationHandle;
};
