// Copyright Dominik Forkert. All Rights Reserved.


#include "BasePawnMovementComponent.h"

#include "HollowSphere.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UBasePawnMovementComponent::UBasePawnMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBasePawnMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();
}


// Called every frame
void UBasePawnMovementComponent::TickComponent(const float DeltaTime, const ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


void UBasePawnMovementComponent::PushAwayFromCollision(const FHitResult& MovementHitResult, const FVector& LocalDeltaMovement) const
{
	if (!Owner)
	{
		UE_LOG(LogActor, Warning, TEXT("Trying to move a movement component without an owner!"));
		return;
	}
	
	const FVector GlobalDeltaMovement{Owner->GetActorForwardVector() * LocalDeltaMovement.X};
	const FVector OrthogonalToNormalInXY{-MovementHitResult.Normal.Y, MovementHitResult.Normal.X, 0.f};
	Owner->AddActorWorldOffset(GlobalDeltaMovement.ProjectOnTo(OrthogonalToNormalInXY), true);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UBasePawnMovementComponent::Move(const float ControllerAxisValue)
{
	if (!Owner)
	{
		UE_LOG(LogActor, Warning, TEXT("Trying to move a movement component without an owner!"));
		return;
	}
	const float DeltaTime{UGameplayStatics::GetWorldDeltaSeconds(this)};

	switch (MovementGeometry)
	{
	case EMovementGeometry::Euclidean: MoveInPlane(ControllerAxisValue, DeltaTime);
		break;
	case EMovementGeometry::Spherical: MoveInHollowSphere(ControllerAxisValue, DeltaTime);
		break;
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UBasePawnMovementComponent::Turn(const float ControllerAxisValue)
{
	if (!Owner)
	{
		return;
	}
	
	const float DeltaTime{UGameplayStatics::GetWorldDeltaSeconds(this)};
	// ReSharper disable once CppLocalVariableMayBeConst
	FRotator DeltaRotation{0.f, ControllerAxisValue*RotationSpeed*DeltaTime, 0.f};
	Owner->AddActorLocalRotation(DeltaRotation, true);
}

void UBasePawnMovementComponent::SetMovementGeometry(const EMovementGeometry NewMovementGeometry,
	AHollowSphere* AssociatedHollowSphere)
{
	MovementGeometry = NewMovementGeometry;

	if (MovementGeometry == EMovementGeometry::Spherical && AssociatedHollowSphere)
	{
		this->HollowSphere = AssociatedHollowSphere;
	}
}

void UBasePawnMovementComponent::MoveInHollowSphere(const float ControllerAxisValue, const float DeltaTime) const
{
	if (!HollowSphere)
	{
		UE_LOG(LogActor, Warning, TEXT("Spherical movement requested, but no sphere found!"));
		return;
		}
	
	FTransform Transform{Owner->GetActorTransform()};
	const FVector SphereCenter{HollowSphere->GetActorLocation()}; //{0.f, 0.f, -4000.f};
	const FVector CenterToPawn{Owner->GetActorLocation() - SphereCenter};
	const float Circumference{2.f * PI * static_cast<float>(CenterToPawn.Length())};
	
	const FVector RotationAxis{-Owner->GetActorRightVector()};
	const float AngularSpeedInDeg{ControllerAxisValue * Speed * DeltaTime * 360.f / Circumference};
	const FVector RotatedCenterToPawn{CenterToPawn.RotateAngleAxis(AngularSpeedInDeg, RotationAxis)};
	const FVector MovementAlongSphere{RotatedCenterToPawn - CenterToPawn};
	Transform.AddToTranslation(MovementAlongSphere);
	
	const FRotator RotationAlongSphere{AngularSpeedInDeg, 0.f, 0.f};
	Transform.ConcatenateRotation(RotationAlongSphere.Quaternion());

	FHitResult MovementHitResult;
	Owner->SetActorTransform(Transform, true, &MovementHitResult);
}

void UBasePawnMovementComponent::MoveInPlane(const float ControllerAxisValue, const float DeltaTime) const
{
	const FVector LocalDeltaMovement{ControllerAxisValue * Speed * DeltaTime, 0.f, 0.f};
	FHitResult MovementHitResult;
	Owner->AddActorLocalOffset(LocalDeltaMovement, true, &MovementHitResult);

	if (MovementHitResult.bBlockingHit)
	{
		PushAwayFromCollision(MovementHitResult, LocalDeltaMovement);
	}
}