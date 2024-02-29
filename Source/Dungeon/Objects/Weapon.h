#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "Particles/ParticleSystemComponent.h"
#include "Weapon.generated.h"

/**
 *
 */

class UFXSystemAsset;
class UShapeComponent;
class UBoxComponent;
class UMeshComponent;
class UMaterialInstance;
class ACharacter;
class UItemObject;
class UNiagaraComponent;
class USceneComponent;
class UParticleSystemComponent;
class AItemManager;

UENUM()
enum class EItemMode : uint8
{
	Loot, Inventory, Equip, Max
};

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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//property
private:
	AItemManager* Manager;
	TArray<UShapeComponent*> CollisionComponents;
	TArray<UShapeComponent*> InteractCollisionComponents;
	TArray<UMeshComponent*> MeshComponents;
	TArray<AActor*> HittedActors;
	UPROPERTY()UItemObject* ItemObject;
	ACharacter* OwnerCharacter;
	bool bPickable;

	UPROPERTY(ReplicatedUsing = "OnRep_Mode")EItemMode Mode = EItemMode::Max;

	UNiagaraComponent* NiagaraPickEffect;
	UParticleSystemComponent* ParticlePickEffect;

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

	UPROPERTY(EditDefaultsOnly, Category = "Loot")
		UFXSystemAsset* LootEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Loot")
		TArray<FParticleSysParam> LootEffectParams;

	UPROPERTY(EditDefaultsOnly, Category = "Loot")
		FQuat LootEffectWorldRotation;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true))
		uint8 TeamID = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true))
		float Damage;

public:

	//function
private:
	UFUNCTION() void OnRep_Mode();

	void FindComponents();
	void SpawnLootEffects();
	void SetEffectLocation();
	void SortMesh();
	void ActivateEffect();
	void DeactivateEffect();
	void SetPickableMode();
	void SetInventoryMode();
	void SetEquipMode();

protected:	
	UFUNCTION()void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void SendDamage(float InDamage, AActor* OtherActor, const FHitResult& SweepResult);
public:
	void SetOwnerCharacter(ACharacter* InCharacter);
	void OnCollision();
	void OffCollision();
	void ResetHittedActors();

	void SetItemLocation(const FVector& NewLocation, bool bSweep = false, FHitResult* OutSweepHitResult = nullptr, ETeleportType Teleport = ETeleportType::None);
	void SetItemRotation(FRotator NewRotation, ETeleportType Teleport = ETeleportType::None);
	void AttachItemToComponent(USceneComponent* Parent, const FAttachmentTransformRules& AttachmentRules, FName InSocketName = NAME_None);
	void ChangeVisibility(EItemMode InMode);
	void SetMode(EItemMode InMode);

	FORCEINLINE void SetTeamID(uint8 InID) { TeamID = InID; }
	FORCEINLINE void SetDamage(float InDamage) { Damage = InDamage; }
	FORCEINLINE void SetManager(AItemManager* InManager) {Manager = InManager; OnRep_Mode();}

	FORCEINLINE FTransform GetAttachTransform() { return AttachTransform; }
	FORCEINLINE FName GetSocketName() { return SocketName; }
	FORCEINLINE UItemObject* GetItemObject() { return ItemObject; }

};
