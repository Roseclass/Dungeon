#pragma once

#include "CoreMinimal.h"
#include "Objects/Projectile.h"
#include "Projectile_ChainLightning.generated.h"

/**
 * 
 */

class USphereComponent;
class UCapsuleComponent;
class UNiagaraComponent;


UCLASS()
class DUNGEON_API AProjectile_ChainLightning : public AProjectile
{
	GENERATED_BODY()
public:
	AProjectile_ChainLightning();
protected:
	virtual void BeginPlay() override;
public:

	//property
private:
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Datas")
		float LifeTime = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Datas")
		float DestoryDelay = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Datas")
		float Count = 4;

	UPROPERTY(VisibleDefaultsOnly)
		UCapsuleComponent* OverlapCollision;

	UPROPERTY(VisibleDefaultsOnly)
		UNiagaraComponent* Niagara;
public:

	//function
private:
protected:
	virtual void SendDamage(TSubclassOf<UGameplayEffect> EffectClass, float InForce, float InDamage, AActor* Target, const FHitResult& SweepResult)override;
public:
};
