// // Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "glTFRuntimeParser.h"
#include "Components/ActorComponent.h"
#include "Data/KinetixDataLibrary.h"
#include "KinetixCharacterComponent.generated.h"

class FAnimSequenceSampler;
class USkeletalMeshComponent;
class UPoseableMeshComponent;
class UAnimSamplerComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnimationStart, const FAnimationID&, AnimationID);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnimationEnd, const FAnimationID&, AnimationID);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFramePlayed);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent, DisplayName="KinetixComponent"))
class KINETIXRUNTIME_API UKinetixCharacterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UKinetixCharacterComponent();
	UKinetixCharacterComponent(FVTableHelper& Helper);

	bool RegisterClipSampler();

	void PlayAnimation(const FAnimationID& InAnimationID, bool bLoop,
	                   const FOnPlayedKinetixAnimationLocalPlayer& OnPlayedAnimationDelegate);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void CheckSkeletalMeshComponent();

	UFUNCTION(BlueprintCallable)
	void SetSkeletalMeshComponent(USkeletalMeshComponent* InSkeletalMeshComponent);

	UFUNCTION()
	void OnOwnerAnimationInitialized();

	UFUNCTION(BlueprintCallable)
	void LoadAnimationAndPlay(const FString& Url);

	UFUNCTION()
	FString RemapBones(const int32 NodeIndex, const FString& CurveName, const FString& Path, UObject* Context);

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

private:
	UPROPERTY(VisibleAnywhere, meta=(DisplayName = "Skeletal Mesh in use"))
	USkeletalMeshComponent* OwnerSkeletalMeshComponent;

	UPROPERTY(EditAnywhere, meta=(DisplayName = "[DEBUG] PoseableMeshComp"))
	AActor* DebugPoseableMeshComponent;

	UPROPERTY(EditAnywhere, Category="Kinetix|Animation", meta=(AllowPrivateAccess="true"))
	bool bRegisterPlayerOnLaunch;

	FTimerHandle CheckSkeletalMeshTimer;

	UPROPERTY()
	FglTFRuntimeSkeletalAnimationConfig AnimConfig;

	TSharedPtr<FAnimSequenceSampler> AnimSequenceSampler;
};
