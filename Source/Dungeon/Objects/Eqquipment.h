#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "Particles/ParticleSystemComponent.h"
#include "Characters/CharcterAppearanceData.h"
#include "Eqquipment.generated.h"

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

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Weapon,
	Helms,
	UpperBody,
	LowerBody,
	Boots,
	Gloves,
	Max
};

UCLASS()
class DUNGEON_API AEqquipment : public AActor
{
	GENERATED_BODY()
	
public:	
	AEqquipment();
protected:
	virtual void BeginPlay() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
public:	
	virtual void Tick(float DeltaTime) override;

	//property
private:
	AItemManager* Manager;
	TArray<UShapeComponent*> InteractCollisionComponents;
	TArray<UMeshComponent*> MeshComponents;
	UPROPERTY()UItemObject* ItemObject;
	ACharacter* OwnerCharacter;
	bool bPickable;

	UPROPERTY(ReplicatedUsing = "OnRep_Mode")EItemMode Mode = EItemMode::Max;

	UNiagaraComponent* NiagaraPickEffect;
	UParticleSystemComponent* ParticlePickEffect;

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

	UPROPERTY(EditDefaultsOnly, Category = "Data")
		EItemType ItemType;

	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (EditCondition = "EItemType == EAppearancePart::UpperBody", EditConditionHides, DisplayName = "Index"))
		int32 UpperBodyIndex;

	TArray<EAppearancePart> HelmsTypes;
	EAppearancePart UpperBodyTypes = EAppearancePart::ChestAttachment;
	TArray<EAppearancePart> LowerBodyTypes;
	TArray<EAppearancePart> BootsTypes;
	TArray<EAppearancePart> GlovesTypes;
protected:
public:

	//function
private:
protected:
	UFUNCTION()virtual void OnRep_Mode();
	virtual void FindComponents();
	virtual void SpawnLootEffects();
	virtual void SetEffectLocation();
	virtual void SortMesh();
	virtual void ActivateEffect();
	virtual void DeactivateEffect();
	virtual void SetPickableMode();
	virtual void SetInventoryMode();
	virtual void SetEquipMode();

public:
	virtual void SetOwnerCharacter(ACharacter* InCharacter);

	virtual void SetItemLocation(const FVector& NewLocation, bool bSweep = false, FHitResult* OutSweepHitResult = nullptr, ETeleportType Teleport = ETeleportType::None);
	virtual void SetItemRotation(FRotator NewRotation, ETeleportType Teleport = ETeleportType::None);
	virtual void AttachItemToComponent(USceneComponent* Parent, const FAttachmentTransformRules& AttachmentRules, FName InSocketName = NAME_None);
	virtual void ChangeVisibility(EItemMode InMode);
	virtual void SetMode(EItemMode InMode);

	FORCEINLINE void SetManager(AItemManager* InManager) { Manager = InManager; OnRep_Mode(); }

	FORCEINLINE UItemObject* GetItemObject() { return ItemObject; }
	FORCEINLINE EItemType GetType() { return ItemType; }
	//FORCEINLINE EItemType GetType() { return ItemType; }
	
};
