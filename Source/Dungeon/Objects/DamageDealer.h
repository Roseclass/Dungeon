#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "DamageDealer.generated.h"

/**
 *	
 */

class AController;
class UCustomDamageType;
class UShapeComponent;
class UGameplayEffect;

UCLASS()
class DUNGEON_API ADamageDealer : public AActor
{
	GENERATED_BODY()
	
public:	
	ADamageDealer();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	//property
private:
	TArray<UShapeComponent*> CollisionComponents;

	TArray<AActor*> OverlappedActors;
	AActor* CurrentOverlappedActor;

	TArray<AActor*> DamagedActors;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Datas")
		TSubclassOf<UCustomDamageType> DamageTypeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Datas")
		TSubclassOf<UGameplayEffect> GamePlayEffectClass;

	UPROPERTY(EditAnywhere, Category = "Datas")
		float Damage;

	UPROPERTY(EditAnywhere, Category = "Datas")
		float Force;

	UPROPERTY(EditAnywhere, Category = "Datas")
		uint8 TeamID = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Datas")
		FName OverlapComponentTag = FName("OverlapCollision");
public:

	//function
private:
protected:
	UFUNCTION()virtual void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()virtual void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	virtual void ResetDamagedActors();
	virtual void FindCollision();

	FORCEINLINE const TArray<AActor*>& GetOverlappedActors()const { return OverlappedActors; }
	FORCEINLINE const AActor* GetCurrentOverlappedActor()const { return CurrentOverlappedActor; }
	FORCEINLINE const TArray<AActor*>& GetDamagedActors()const { return DamagedActors; }
public:
	FORCEINLINE void SetTeamID(uint8 InID) { TeamID = InID; }
	FORCEINLINE void SetDamage(float InDamage) { Damage = InDamage; }
};
