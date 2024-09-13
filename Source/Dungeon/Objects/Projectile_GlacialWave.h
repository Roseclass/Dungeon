#pragma once

#include "CoreMinimal.h"
#include "Objects/Projectile.h"
#include "Projectile_GlacialWave.generated.h"

/**
 * 
 */

class USceneComponent;
class UNiagaraComponent;

UCLASS()
class DUNGEON_API AProjectile_GlacialWave : public AProjectile
{
	GENERATED_BODY()
public:
	AProjectile_GlacialWave();
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

	UPROPERTY(VisibleDefaultsOnly)
		USceneComponent* Root;

	UPROPERTY(VisibleDefaultsOnly)
		UNiagaraComponent* Niagara;
public:

	//function
private:
protected:
public:
};
