#pragma once

#include "CoreMinimal.h"
#include "Objects/DamageDealer.h"
#include "GenericTeamAgentInterface.h"
#include "Projectile.generated.h"

/**
 * base of every projectile ex)arrow, missile
 */

class UProjectileMovementComponent;
class UShapeComponent;

UCLASS()
class DUNGEON_API AProjectile : public ADamageDealer
{
	GENERATED_BODY()
	
public:	
	AProjectile();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	//property
private:
	UPROPERTY(EditDefaultsOnly, Category = "Datas")
		bool bAOE;
protected:
	//actor
	UPROPERTY(VisibleDefaultsOnly)
		UProjectileMovementComponent* ProjectileMovement;
public:

	//function
private:
protected:
	virtual void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)override;
public:
};
