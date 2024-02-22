#pragma once

#include "CoreMinimal.h"
#include "Objects/Projectile.h"
#include "Projectile_Mage21.generated.h"

/**
 * 
 */

class USceneComponent;
class UStaticMeshComponent;
class UNiagaraComponent;

UCLASS()
class DUNGEON_API AProjectile_Mage21 : public AProjectile
{
	GENERATED_BODY()
public:
	AProjectile_Mage21();
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;

	//property
private:
	bool bAct;
	TArray<AActor*> OverlappedActors;
protected:
	UPROPERTY(VisibleDefaultsOnly)
		USceneComponent* Root;

	UPROPERTY(VisibleDefaultsOnly)
		UStaticMeshComponent* CollisionMesh;

	UPROPERTY(VisibleDefaultsOnly)
		UNiagaraComponent* NiagaraEffect;
public:

	//function
private:
protected:
	virtual void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)override;
	virtual void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)override;
	virtual void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)override;
public:
	virtual void Activate()override;
	virtual void Deactivate()override;
};
