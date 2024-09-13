#pragma once

#include "CoreMinimal.h"
#include "Objects/Projectile.h"
#include "Projectile_ThunderStorm.generated.h"

/**
 * 
 */

class USphereComponent;
class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class DUNGEON_API AProjectile_ThunderStorm : public AProjectile
{
	GENERATED_BODY()
public:
	AProjectile_ThunderStorm();
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;

	//property
private:
	float CurrentDelay;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Datas")
		float LifeTime = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Datas")
		float DestoryDelay = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Datas")
		float DotDamageDelay = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Datas")
		UNiagaraSystem* ThunderFX;

	UPROPERTY(VisibleDefaultsOnly)
		USphereComponent* RootCollision;

	UPROPERTY(VisibleDefaultsOnly)
		UNiagaraComponent* Niagara;

public:

	//function
private:
	void SpawnThunderAndSendDamage();
protected:
public:
};

//
// 데미지 즉발
// 큐로 이펙트 재생
//
