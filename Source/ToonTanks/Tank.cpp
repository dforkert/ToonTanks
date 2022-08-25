// Copyright Dominik Forkert. All Rights Reserved.


#include "Tank.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"

//Set default values

ATank::ATank()
{
	TankSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Tank Spring Arm"));
	TankSpringArmComponent->SetupAttachment(RootComponent);

	TankCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Tank Camera"));
	TankCameraComponent->SetupAttachment(TankSpringArmComponent);
}

void ATank::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (TankPlayerController)
	{
		FHitResult HitResult;
		TankPlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
		RotateTurret(HitResult.ImpactPoint, DeltaTime);
	}
}

void ATank::HandleDestruction()
{
	Super::HandleDestruction();

	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	bTankAlive = false;
}

void ATank::BeginPlay()
{
	Super::BeginPlay();

	TankPlayerController = Cast<APlayerController>(GetController());
	
}


// Called to bind functionality to input
void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ATank::Move);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATank::Turn);

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATank::Fire);
}

void ATank::Move(const float Value)
{
	const float DeltaTime{UGameplayStatics::GetWorldDeltaSeconds(this)};
	const FVector DeltaLocation{Value*Speed*DeltaTime,0.f,0.f};
	AddActorLocalOffset(DeltaLocation, true);
}

void ATank::Turn(const float Value)
{
	const float DeltaTime{UGameplayStatics::GetWorldDeltaSeconds(this)};
	// ReSharper disable once CppLocalVariableMayBeConst
	FRotator DeltaRotation{0.f, Value*RotationSpeed*DeltaTime, 0.f};
	AddActorLocalRotation(DeltaRotation, true);
}



