// // Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/AnimationId.h"
#include "KinetixAnimation.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRegisterLocalPlayer);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayedKinetixAnimationLocalPlayer, UKinetixAnimationId*, AnimationId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKinetixAnimationStartOnLocalPlayer, UKinetixAnimationId*, AnimationId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKinetixAnimationEndOnLocalPlayer, UKinetixAnimationId*, AnimationId);

/**
 * 
 */
UCLASS(BlueprintType)
class KINETIXRUNTIME_API UKinetixAnimation
	: public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Kinetix|Animation")
	void RegisterLocalPlayerAnimationInstance(UPARAM(ref) UAnimInstance* InAnimInstance);

	void SetReferenceSkeletalMesh(USkeletalMesh* InSkeletalMesh);

public:
	UPROPERTY(BlueprintAssignable)
	FOnRegisterLocalPlayer OnRegisterLocalPlayer;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	UAnimInstance* AnimInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	USkeletalMesh* ReferenceSkeletalMesh;

};
