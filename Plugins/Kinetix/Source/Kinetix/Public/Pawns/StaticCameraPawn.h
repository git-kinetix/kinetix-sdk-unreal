// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "GameFramework/Pawn.h"
#include "StaticCameraPawn.generated.h"

class UCameraComponent;
class UInputMappingContext;

UCLASS()
class KINETIX_API AStaticCameraPawn : public APawn
{
	GENERATED_BODY()

public:
	
	// Sets default values for this pawn's properties
	AStaticCameraPawn();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void GoToCamera(AStaticCameraPawn* InCameraToGo);
	
	UFUNCTION(BlueprintCallable)
	void GoToNextCamera();
	
	UFUNCTION(BlueprintCallable)
	void GoToPreviousCamera();

	UFUNCTION()
	void OnBlendEnd(AStaticCameraPawn* CameraToPossess);
	
private:

	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		UCameraComponent* CameraRoot;

	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		AStaticCameraPawn* NextCamera;

	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		AStaticCameraPawn* PreviousCamera;

	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float TransitionTime;

#pragma region Input

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input", meta=(AllowPrivateAccess="true"))
	UInputMappingContext* CameraInputMapping;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input", meta=(AllowPrivateAccess="true"))
	const UInputAction* InputNextCamera;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input", meta=(AllowPrivateAccess="true"))
	const UInputAction* InputPreviousCamera;

	FDelegateHandle CameraBlendDelegateHandle;

#pragma endregion
};
