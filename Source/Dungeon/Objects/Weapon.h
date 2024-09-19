#pragma once

#include "CoreMinimal.h"
#include "Objects/Eqquipment.h"
#include "Weapon.generated.h"

/**
 *
 */

struct FDamageEhancementData;

UCLASS()
class DUNGEON_API AWeapon : public AEqquipment
{
	GENERATED_BODY()
	
public:	
	AWeapon();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	//property
private:
	TArray<UShapeComponent*> CollisionComponents;
	TArray<AActor*> HitActors;
	const FDamageEhancementData* DamageData;

	UPROPERTY(EditDefaultsOnly, Category = "Attach")
		FTransform AttachTransform;

	UPROPERTY(EditDefaultsOnly, Category = "Attach")
		FName SocketName;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "Essential")
		uint8 TeamID = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Essential")
		TSubclassOf<UGameplayEffect> OverlapEffectClass;

public:

	//function
private:
protected:	
	virtual void FindComponents()override;

	virtual void SetOwnerCharacter(ADungeonCharacterBase* InCharacter)override;

	UFUNCTION()void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
public:
	void OnCollision(const FDamageEhancementData* InDamageData);
	void OffCollision();
	void ResetHitActors();

	FORCEINLINE void SetTeamID(uint8 InID) { TeamID = InID; }

	FORCEINLINE FTransform GetAttachTransform() { return AttachTransform; }
	FORCEINLINE virtual FName GetSocketName() const override{ return SocketName; }
};
