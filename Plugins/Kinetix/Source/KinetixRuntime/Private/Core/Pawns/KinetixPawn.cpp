// // Copyright Kinetix. All Rights Reserved.


#include "Core/Pawns/KinetixPawn.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Components/KinetixCharacterComponent.h"

// Sets default values
AKinetixPawn::AKinetixPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	SetRootComponent(CapsuleComponent);
	SkeletalMeshSource = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshSource"));
	SkeletalMeshSource->SetupAttachment(GetRootComponent());
	SkeletalMeshTarget = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshTarget"));
	SkeletalMeshTarget->SetupAttachment(SkeletalMeshSource);

	KinetixComponent = CreateDefaultSubobject<UKinetixCharacterComponent>(TEXT("KinetixComponent"));
}

// Called when the game starts or when spawned
void AKinetixPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called to bind functionality to input
void AKinetixPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

