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
public:
	virtual void Activate()override;
	virtual void Deactivate()override;
};
