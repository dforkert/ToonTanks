// Copyright Dominik Forkert. All Rights Reserved.


#include "ProjectileHomingBase.h"

#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
UProjectileHomingBase::UProjectileHomingBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = true;
	bAllowConcurrentTick = true;
	SetComponentTickInterval(0.04f);
}

UProjectileHomingBase* UProjectileHomingBase::SpawnProjectileHomingComponent(
	UObject* Outer,
	const UClass* ProjectileHomingClass,
	UProjectileMovementComponent* InProjectileMovement,
	const AActor* InTargetActor
)
{
	UProjectileHomingBase* ProjectileHoming = NewObject<UProjectileHomingBase>(Outer, ProjectileHomingClass, TEXT("Projectile Homing Component"));
	
	ProjectileHoming->ProjectileMovement = InProjectileMovement;
	ProjectileHoming->TargetActor = InTargetActor;
	InProjectileMovement->HomingTargetComponent = ProjectileHoming;

	// MovementComponent ticks before ProjectileHomingComponent
	ProjectileHoming->AddTickPrerequisiteComponent(InProjectileMovement);

	ProjectileHoming->RegisterComponent();


	return ProjectileHoming;
}

// Called when the game starts or when spawned
void UProjectileHomingBase::BeginPlay()
{
	Super::BeginPlay();
	
}


void UProjectileHomingBase::UpdateProjectileHomingLocation(const float DeltaTime)
{
	SetWorldLocation(TargetActor->GetActorLocation());
	DrawDebugCrosshairs(GetWorld(), GetComponentLocation(), FRotator::ZeroRotator, 300.f, FColor::Red);
}

void UProjectileHomingBase::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
	)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateProjectileHomingLocation(DeltaTime);
}
