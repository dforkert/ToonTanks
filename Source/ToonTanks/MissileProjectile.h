// Copyright Dominik Forkert. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "MissileProjectile.generated.h"

class UProjectileHomingBase;

UCLASS()
class TOONTANKS_API AMissileProjectile final : public AProjectile
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMissileProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// TODO: do we need this anymore?
	/**
	 * @brief Destroys the corresponding ProjectileHoming Actor, before destroying the Projectile itself
	 */
	virtual void DestroyProjectile() override;

private:
	UPROPERTY(EditAnywhere, DisplayName="Projectile Homing Type", Category="Combat")
	TSubclassOf<UProjectileHomingBase> ProjectileHomingClass;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	UProjectileHomingBase* ProjectileHoming;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
	

};
