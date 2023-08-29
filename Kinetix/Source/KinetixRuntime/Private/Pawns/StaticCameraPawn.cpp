// Copyright Kinetix. All Rights Reserved.

#include "Pawns/StaticCameraPawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"

#include "KinetixRuntimeModule.h"

// Sets default values
AStaticCameraPawn::AStaticCameraPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CameraRoot = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraRoot"));
	if (!IsValid(CameraRoot))
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("Camera is null !"));
		return;
	}

	SetRootComponent(CameraRoot);

	// Automatically assign the local player if placed on a level
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	TransitionTime = 0.5f;
}

// Called to bind functionality to input
void AStaticCameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	APlayerController* PC = GetController<APlayerController>();
	if (!IsValid(PC))
		return;

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		PC->GetLocalPlayer());
	if (!IsValid(InputSubsystem))
	{
		UKismetSystemLibrary::PrintString(this, TEXT("ERROR: EnhancedInput not configured !"), true, true,
		                                  FLinearColor::Red, 10.f);
		return;
	}

	if (!IsValid(CameraInputMapping))
	{
		UKismetSystemLibrary::PrintString(this, TEXT("%s Input Mapping not set !"), true, true, FLinearColor::Yellow,
		                                  5.f);
		return;
	}

	InputSubsystem->ClearAllMappings();
	InputSubsystem->AddMappingContext(CameraInputMapping, 0);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!IsValid(EnhancedInputComponent))
	{
		UKismetSystemLibrary::PrintString(this, TEXT("%s EnhancedPlayerInputComponent not set in ProjectSettings !"),
		                                  true, true, FLinearColor::Yellow, 5.f);
		return;
	}

	if (!IsValid(InputNextCamera)
		|| !IsValid(InputPreviousCamera))
	{
		UKismetSystemLibrary::PrintString(this, TEXT("%s Input actions not set !"), true, true, FLinearColor::Yellow,
		                                  5.f);
		return;
	}

	EnhancedInputComponent->BindAction(InputNextCamera, ETriggerEvent::Completed, this,
	                                   &AStaticCameraPawn::GoToNextCamera);
	EnhancedInputComponent->BindAction(InputPreviousCamera, ETriggerEvent::Completed, this,
	                                   &AStaticCameraPawn::GoToPreviousCamera);

	EnableInput(PC);
	
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// Called when the game starts or when spawned
void AStaticCameraPawn::BeginPlay()
{
	Super::BeginPlay();
}

void AStaticCameraPawn::GoToCamera(AStaticCameraPawn* InCameraToGo)
{
	if (!IsValid(InCameraToGo))
	{
		UKismetSystemLibrary::PrintString(this,
		                                  FString::Printf(TEXT("%s GoToNextCamera: No camera setup !"), *GetName()),
		                                  true, true, FLinearColor::Yellow, 5.f);
		return;
	}

	APlayerController* PC = GetController<APlayerController>();
	if (!IsValid(PC))
		return;

	TObjectPtr<APlayerCameraManager> PCM = PC->PlayerCameraManager;
	if (!IsValid(PCM))
		return;

	DisableInput(PC);
	
	CameraBlendDelegateHandle = PCM->OnBlendComplete().AddUObject(this, &AStaticCameraPawn::OnBlendEnd, InCameraToGo);
	PC->SetViewTargetWithBlend(InCameraToGo, TransitionTime, VTBlend_EaseIn, 0.5f);
}

void AStaticCameraPawn::GoToNextCamera()
{
	GoToCamera(NextCamera);
}

void AStaticCameraPawn::GoToPreviousCamera()
{
	GoToCamera(PreviousCamera);
}

void AStaticCameraPawn::OnBlendEnd(AStaticCameraPawn* CameraToPossess)
{
	TObjectPtr<APlayerCameraManager> PCM = GetController<APlayerController>()->PlayerCameraManager;
	if (!IsValid(PCM))
		return;

	PCM->OnBlendComplete().Remove(CameraBlendDelegateHandle);
	CameraBlendDelegateHandle.Reset();

	GetController()->Possess(CameraToPossess);
}
