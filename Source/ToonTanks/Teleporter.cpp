// Copyright Dominik Forkert. All Rights Reserved.


#include "Teleporter.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATeleporter::ATeleporter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	RootComponent = CapsuleComponent;
	CapsuleComponent->SetGenerateOverlapEvents(true);

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(CapsuleComponent);

}

// Called when the game starts or when spawned
void ATeleporter::BeginPlay()
{
	Super::BeginPlay();

	// Bind functions to Component Hit Event
	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &ATeleporter::OnBeginOverlap);
	CapsuleComponent->OnComponentEndOverlap.AddDynamic(this, &ATeleporter::OnEndOverlap);
}


FVector ATeleporter::GetTeleportationLocation(const AActor* Actor) const
{
	const double ActorToTeleporterZOffset{Actor->GetActorLocation().Z - GetActorLocation().Z};
	FVector TargetLocation{TargetTeleporter->GetActorLocation()};
	TargetLocation.Z += ActorToTeleporterZOffset;
	return TargetLocation;
}

void ATeleporter::TrySetBasePawnMovementComponentToLocalGeometry(const AActor* Actor) const
{
	UBasePawnMovementComponent* OtherActorMovementComponent{
		Cast<UBasePawnMovementComponent>(Actor->GetComponentByClass(UBasePawnMovementComponent::StaticClass()))
	};
	if (OtherActorMovementComponent)
	{
		OtherActorMovementComponent->SetMovementGeometry(LocalGeometry, SurroundingHollowSphere);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ATeleporter::OnBeginOverlap(
	UPrimitiveComponent* /*OverlappedComp*/,
	AActor* OtherActor,
	UPrimitiveComponent* /*OtherComp*/,
	int32 /*OtherBodyIndex*/,
	bool /*bFromSweep*/,
	const FHitResult& /*Hit*/
) 
{
	if (OtherActor && TargetTeleporter && !bTeleporterIsBlocked && !TargetTeleporter->bTeleporterIsBlocked)
	{
		const FVector TargetLocation{GetTeleportationLocation(OtherActor)};
		TargetTeleporter->bTeleporterIsBlocked = true;

		const bool bTeleportSuccessful{
			OtherActor->TeleportTo(TargetLocation, FRotator::ZeroRotator)
		};
		if (bTeleportSuccessful)
		{
			TargetTeleporter->TrySetBasePawnMovementComponentToLocalGeometry(OtherActor);
			
			if (TeleportSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, TeleportSound, TargetLocation);
			}
		}
		else
		{
			TargetTeleporter->bTeleporterIsBlocked = false;
		}
	}
}

void ATeleporter::OnEndOverlap(
	UPrimitiveComponent* /*OverlappedComp*/,
	AActor* /*OtherActor*/,
	UPrimitiveComponent* /*OtherComp*/,
	int32 /*OtherBodyIndex*/
	)
{
	bTeleporterIsBlocked = false;
}
