#pragma once

#include "CoreMinimal.h"
#include "Objects/DamageDealer.h"
#include "GenericTeamAgentInterface.h"
#include "Projectile.generated.h"

/**
 * base of every projectile ex)arrow, missile
 */

class UProjectileMovementComponent;
class UNiagaraSystem;
class UShapeComponent;

UENUM(BlueprintType)
enum class EProjectileType : uint8
{
	AOE_Normal = 0b00 UMETA(DisplayName = "Multiple Hits, Not Neutralized by Environment"),
	Once_Normal = 0b01 UMETA(DisplayName = "Single Hit, Not Neutralized by Environment"),
	AOE_Neutralizable = 0b10 UMETA(DisplayName = "Multiple Hits, Neutralized by Environment"),
	Once_Neutralizable = 0b11 UMETA(DisplayName = "Single Hit, Neutralized by Environment")
};

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
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Datas")
		EProjectileType Type;

	UPROPERTY(EditAnywhere, Category = "Datas")
		UNiagaraSystem* MuzzleFX;

	UPROPERTY(EditAnywhere, Category = "Datas")
		UNiagaraSystem* DestroyFX;

	//actor
	UPROPERTY(VisibleDefaultsOnly)
		UProjectileMovementComponent* ProjectileMovement;
public:

	//function
private:
protected:
	virtual void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)override;
	virtual void FindCollision()override;

	UFUNCTION()virtual void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
	virtual void SpawnMuzzleFX();
	virtual void SpawnDestroyFX(const FHitResult& SweepResult);
public:
};
