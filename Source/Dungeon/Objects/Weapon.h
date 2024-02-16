#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

/**
 *
 */

class UItemObject;
class UMaterialInstance;

UCLASS()
class DUNGEON_API AWeapon : public AActor
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
	TArray<AActor*> HittedActors;
	UItemObject* ItemObject;

	UPROPERTY(EditDefaultsOnly, Category = "Attach")
		FTransform AttachTransform;

	UPROPERTY(EditDefaultsOnly, Category = "Attach")
		FName SocketName;

	UPROPERTY(EditDefaultsOnly, Category = "InventroyData")
		int32 DimensionX;

	UPROPERTY(EditDefaultsOnly, Category = "InventroyData")
		int32 DimensionY;

	UPROPERTY(EditDefaultsOnly, Category = "InventroyData")
		UMaterialInstance* Icon;

	UPROPERTY(EditDefaultsOnly, Category = "InventroyData")
		UMaterialInstance* IconRotated;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true))
		uint8 TeamID = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true))
		float Damage;

public:

	//function
private:
	void FindCollision();
protected:	
	UFUNCTION()void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void SendDamage(float InDamage, AActor* OtherActor, const FHitResult& SweepResult);
public:
	void OnCollision();
	void OffCollision();
	void ResetHittedActors();

	FORCEINLINE void SetTeamID(uint8 InID) { TeamID = InID; }
	FORCEINLINE void SetDamage(float InDamage) { Damage = InDamage; }

	FORCEINLINE FTransform GetAttachTransform() { return AttachTransform; }
	FORCEINLINE FName GetSocketName() { return SocketName; }
	FORCEINLINE UItemObject* GetItemObject() { return ItemObject; }

};
