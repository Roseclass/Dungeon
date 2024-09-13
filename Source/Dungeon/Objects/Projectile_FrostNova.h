#pragma once

#include "CoreMinimal.h"
#include "Objects/Projectile.h"
#include "Components/TimeLineComponent.h"
#include "Projectile_FrostNova.generated.h"

/**
 * 
 */

class USphereComponent;
class UNiagaraComponent;

UCLASS()
class DUNGEON_API AProjectile_FrostNova : public AProjectile
{
	GENERATED_BODY()

public:
	AProjectile_FrostNova();
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;

	//property
private:
protected:
	UPROPERTY(VisibleDefaultsOnly)
		USphereComponent* RootCollision;

	UPROPERTY(VisibleDefaultsOnly)
		UNiagaraComponent* Niagara;

	UPROPERTY(EditAnywhere, Category = "SequenceSettings")
		UCurveFloat* SizeCurve;
	FTimeline SizeTimeLine;
	FOnTimelineFloat SizeTimelineFloat;

	UPROPERTY(EditAnywhere, Category = "SequenceSettings")
		float MaxSize = 300;

	UPROPERTY(EditAnywhere, Category = "SequenceSettings")
		float LifeTime = 0.5;

	UPROPERTY(EditAnywhere, Category = "SequenceSettings")
		float DestoryDelay = 0.5;

public:

	//function
private:
	UFUNCTION()void SizeSequenceTickFunction(float Value);
protected:
public:
};
