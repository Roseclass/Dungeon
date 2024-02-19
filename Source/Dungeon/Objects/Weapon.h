#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

/**
 *
 */

class UShapeComponent;
class UMeshComponent;
class UMaterialInstance;
class ACharacter;
class UItemObject;

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
	TArray<UMeshComponent*> MeshComponents;
	TArray<AActor*> HittedActors;
	UItemObject* ItemObject;
	ACharacter* OwnerCharacter;
	bool bPickable;

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
	void FindComponents();
	void SetEffectLocation();
	void SortMesh();
	void ActivateEffect();
	void DeactivateEffect();

protected:	
	UFUNCTION()void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void SendDamage(float InDamage, AActor* OtherActor, const FHitResult& SweepResult);
public:
	void SetOwnerCharacter(ACharacter* InCharacter);
	void OnCollision();
	void OffCollision();
	void ResetHittedActors();

	void SetEquipMode();
	void SetInventoryMode();
	void SetPickableMode();

	FORCEINLINE void SetTeamID(uint8 InID) { TeamID = InID; }
	FORCEINLINE void SetDamage(float InDamage) { Damage = InDamage; }

	FORCEINLINE FTransform GetAttachTransform() { return AttachTransform; }
	FORCEINLINE FName GetSocketName() { return SocketName; }
	FORCEINLINE UItemObject* GetItemObject() { return ItemObject; }

};
