#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericTeamAgentInterface.h"
#include "Projectile.generated.h"

/**
 * 각종 투사체의 기본 베이스 ex)스킬,화살...
 */

class UProjectileMovementComponent;
class UShapeComponent;

UCLASS()
class DUNGEON_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();
protected:
	virtual void BeginPlay() override;
	virtual void OnProjectileDestroyed(AActor* DestroyedActor);
public:	
	virtual void Tick(float DeltaTime) override;
	virtual FGenericTeamId GetGenericTeamId() const;

	//property
private:
	TArray<UShapeComponent*> CollisionComponents;

	//actor
	UPROPERTY(VisibleDefaultsOnly)
		UProjectileMovementComponent* ProjectileMovement;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true))
		uint8 TeamID = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true))
		float Damage;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
		AActor* Target;
public:

	//function
private:
	void FindCollision();
protected:
	UFUNCTION()void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void SendDamage(float InDamage, AActor* OtherActor, const FHitResult& SweepResult);
public:
	//void Init(AActor* InOwner, float InDamgeRate = 1.0, float InLifeSpan = -1);

	FORCEINLINE void SetTeamID(uint8 InID) { TeamID = InID; }
	FORCEINLINE void SetDamage(float InDamage) { Damage = InDamage; }
	FORCEINLINE void SetTarget(AActor* InActor) { Target = InActor; }

};
