#pragma once

#include "CoreMinimal.h"
#include "Objects/Projectile.h"
#include "Projectile_FlameStrike.generated.h"

/**
 * 
 */
class USphereComponent;
class UNiagaraComponent;
class UGameplayEffect;

UCLASS()
class DUNGEON_API AProjectile_FlameStrike : public AProjectile
{
	GENERATED_BODY()
public:
	AProjectile_FlameStrike();
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;

	//property
private:
	TArray<AActor*> DotActors;

	UPROPERTY(EditDefaultsOnly, Category = "Datas")
		TSubclassOf<UGameplayEffect> DotGamePlayEffectClass;

	UPROPERTY(EditAnywhere, Category = "Datas")
		float BurstDuration = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Datas")
		float DotDamage;

	UPROPERTY(EditAnywhere, Category = "Datas")
		float DotDuration = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Datas")
		float DotDamageDelay = 0.2f;
	float CurrentDelay;
protected:
	UPROPERTY(VisibleDefaultsOnly)
		USphereComponent* RootCollision;

	UPROPERTY(VisibleDefaultsOnly)
		UNiagaraComponent* Niagara;

public:

	//function
private:
	void SendDotDamage();

	UFUNCTION()void OnDotBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()void OnDotEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
protected:
	virtual void FindCollision()override;
public:
};
