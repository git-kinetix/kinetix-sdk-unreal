// // Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "glTFRuntimeParser.h"
#include "Components/ActorComponent.h"
#include "KinetixComponent.generated.h"

class USkeletalMeshComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent, DisplayName="KinetixComponent"))
class KINETIXRUNTIME_API UKinetixComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UKinetixComponent();

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
	void LoadAnimationAndPlay();

	//virtual void EndPlay
	UFUNCTION()
	FString RemapBones(const int32 NodeIndex, const FString& CurveName, const FString& Path, UObject* Context);

private:
	UPROPERTY(VisibleAnywhere, meta=(DisplayName = "Skeletal Mesh in use"))
	USkeletalMeshComponent* OwnerSkeletalMeshComponent;

	UPROPERTY(EditAnywhere, Category="Kinetix|Animation", meta=(AllowPrivateAccess="true"))
	FString Url;
	
	UPROPERTY(EditAnywhere, Category="Kinetix|Animation", meta=(AllowPrivateAccess="true"))
	bool bPlayAnimationOnLaunch;

	FTimerHandle CheckSkeletalMeshTimer;

	UPROPERTY()
	FglTFRuntimeSkeletalAnimationConfig AnimConfig;
};
