#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericTeamAgentInterface.h"
#include "Projectile.generated.h"

/**
 * ���� ����ü�� �⺻ ���̽� ex)��ų,ȭ��...
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
	bool bCanCollide;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Collision", meta = (ExposeOnSpawn = true))
		uint8 TeamID = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Collision", meta = (ExposeOnSpawn = true))
		float Damage;

	UPROPERTY(BlueprintReadWrite, Category = "Collision", meta = (ExposeOnSpawn = true))
		AActor* Target;

	UPROPERTY(BlueprintReadWrite, Category = "Collision", meta = (ExposeOnSpawn = true))
		bool bAOE;
public:

	//function
private:
	void FindCollision();
protected:
	UFUNCTION()virtual void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()virtual void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()virtual void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void SendDamage(float InDamage, AActor* OtherActor, const FHitResult& SweepResult);
public:
	//void Init(AActor* InOwner, float InDamgeRate = 1.0, float InLifeSpan = -1);
	virtual void SetCollide(bool InState);
	virtual void Activate();
	virtual void Deactivate();

	FORCEINLINE void SetTeamID(uint8 InID) { TeamID = InID; }
	FORCEINLINE void SetDamage(float InDamage) { Damage = InDamage; }
	FORCEINLINE void SetTarget(AActor* InActor) { Target = InActor; }

};
