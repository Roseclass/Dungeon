#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

/**
 * 각종 투사체의 기본 베이스 ex)스킬,화살...
 */

class UProjectileMovementComponent;

UCLASS()
class DUNGEON_API AProjectile : public AActor
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

	//actor
	UPROPERTY(VisibleDefaultsOnly)
		UProjectileMovementComponent* ProjectileMovement;

protected:
public:

	//function
private:
	void FindCollision();
protected:
public:
};
