// Copyright Dominik Forkert. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Teleporter.generated.h"

class UCapsuleComponent;
class USoundBase;

UCLASS()
class TOONTANKS_API ATeleporter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATeleporter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Actor Components", meta=(AllowPrivateAccess="true"))
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Actor Components", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Teleportation", meta=(AllowPrivateAccess="true"))
	ATeleporter* TargetTeleporter;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Teleportation", meta=(AllowPrivateAccess="true"))
	bool bTeleporterIsBlocked{false};
	
	UPROPERTY(EditAnywhere, Category="Sound")
	USoundBase* TeleportSound;

	FVector GetTeleportationLocation(const AActor* Actor) const;
	
	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& Hit
	);
	
	UFUNCTION()
	void OnEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
	

public:
	// TODO: most likely to be deleted
	// Called every frame
	virtual void Tick(float DeltaTime) override;




};
