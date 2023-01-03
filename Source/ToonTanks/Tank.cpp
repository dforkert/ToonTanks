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

void ATank::DestroyPawn()
{
	Super::DestroyPawn();

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

void ATank::PushTankAwayFromCollision(const FHitResult& MovementHitResult, const FVector& LocalDeltaMovement)
{
	const FVector GlobalDeltaMovement{GetActorForwardVector() * LocalDeltaMovement.X};
	const FVector OrthogonalToNormalInXY{-MovementHitResult.Normal.Y, MovementHitResult.Normal.X, 0.f};
	AddActorWorldOffset(GlobalDeltaMovement.ProjectOnTo(OrthogonalToNormalInXY), true);
}

void ATank::Move(const float ControllerAxisValue)
{
	const float DeltaTime{UGameplayStatics::GetWorldDeltaSeconds(this)};

	 MoveInHollowSphere(ControllerAxisValue, DeltaTime);
	 //MoveInPlane(ControllerAxisValue, DeltaTime);

}

void ATank::Turn(const float ControllerAxisValue)
{
	const float DeltaTime{UGameplayStatics::GetWorldDeltaSeconds(this)};
	// ReSharper disable once CppLocalVariableMayBeConst
	FRotator DeltaRotation{0.f, ControllerAxisValue*RotationSpeed*DeltaTime, 0.f};
	AddActorLocalRotation(DeltaRotation, true);
}

void ATank::MoveInHollowSphere(const float ControllerAxisValue, const float DeltaTime)
{
	FTransform Transform{GetActorTransform()};
	const FVector SphereCenter{0.f, 0.f, -4000.f};
	const FVector CenterToPawn{GetActorLocation() - SphereCenter};
	const float Circumference{2.f * PI * static_cast<float>(CenterToPawn.Length())};
	
	const FVector RotationAxis{-GetActorRightVector()};
	const float AngularSpeedInDeg{ControllerAxisValue * Speed * DeltaTime * 360.f / Circumference};
	const FVector RotatedCenterToPawn{CenterToPawn.RotateAngleAxis(AngularSpeedInDeg, RotationAxis)};
	const FVector MovementAlongSphere{RotatedCenterToPawn - CenterToPawn};
	Transform.AddToTranslation(MovementAlongSphere);
	
	const FRotator RotationAlongSphere{AngularSpeedInDeg, 0.f, 0.f};
	Transform.ConcatenateRotation(RotationAlongSphere.Quaternion());

	FHitResult MovementHitResult;
	SetActorTransform(Transform, true, &MovementHitResult);

	// TODO: Add PushTankAwayFromCollision; maybe refactor code for both plane and sphere
	
}

void ATank::MoveInPlane(const float ControllerAxisValue, const float DeltaTime)
{
	const FVector LocalDeltaMovement{ControllerAxisValue * Speed * DeltaTime, 0.f, 0.f};
	FHitResult MovementHitResult;
	AddActorLocalOffset(LocalDeltaMovement, true, &MovementHitResult);

	if (MovementHitResult.bBlockingHit)
	{
		PushTankAwayFromCollision(MovementHitResult, LocalDeltaMovement);
	}
}

// TODO
/*
bool ATank::TryToSetHollowSphere()
{
	UWorld::SweepSingleByObjectType()
}
*/



