// Copyright Dominik Forkert. All Rights Reserved.


#include "HealthComponent.h"

#include "ToonTanksGameModeBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

	// Bind functions to Damage Event
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakeDamage);

	ToonTanksGameModeBase = Cast<AToonTanksGameModeBase>(UGameplayStatics::GetGameMode(this));
}


void UHealthComponent::TakeDamage(
	AActor* ActorToBeDamaged,
	const float DamageAmount,
	const UDamageType* DamageType,
	AController* Instigator,
	AActor* DamageCauser
)
{
	if (DamageAmount <= 0.f)
	{
		return;
	}

	Health -= DamageAmount;
	if (ToonTanksGameModeBase && Health <= 0.f)
	{
		ToonTanksGameModeBase->ActorDied(GetOwner());
	}
}

// Called every frame
void UHealthComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
	)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

