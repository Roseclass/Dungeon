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

USTRUCT(BlueprintType)
struct FItemAppearanceData
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleDefaultsOnly)
		EAppearancePart PartType;

	UPROPERTY(EditAnywhere)
		int32 Index;
};

USTRUCT(BlueprintType)
struct FItemStatusData
{
	GENERATED_BODY()

	// this data will be saved
private:
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
		float BaseDamage;

	UPROPERTY()
		float FinalDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
		float BaseMaxHealth;
	
	UPROPERTY()
		float FinalMaxHealth;

	UPROPERTY(EditDefaultsOnly, Category = "Mana")
		float BaseMaxMana;

	UPROPERTY()
		float FinalMaxMana;

public:
	UPROPERTY()
		bool bRandomize;
	
	FORCEINLINE float GetFinalDamage()const { return FinalDamage; }
	FORCEINLINE float GetFinalMaxHealth()const { return FinalMaxHealth; }
	FORCEINLINE float GetFinalMaxMana()const { return FinalMaxMana; }
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

	UPROPERTY(EditDefaultsOnly, Category = "Data")
		FItemStatusData ItemStatus;

	UPROPERTY(EditDefaultsOnly, EditFixedSize, Category = "Data", meta = (EditCondition = "ItemType == EItemType::Helms", EditConditionHides,DisplayName = "Datas"))
		TArray<FItemAppearanceData> HelmsAppearanceDatas;

	UPROPERTY(EditDefaultsOnly, EditFixedSize, Category = "Data", meta = (EditCondition = "ItemType == EItemType::UpperBody", EditConditionHides,DisplayName = "Datas"))
		TArray<FItemAppearanceData> UpperBodyAppearanceDatas;

	UPROPERTY(EditDefaultsOnly, EditFixedSize, Category = "Data", meta = (EditCondition = "ItemType == EItemType::LowerBody", EditConditionHides,DisplayName = "Datas"))
		TArray<FItemAppearanceData> LowerBodyAppearanceDatas;

	UPROPERTY(EditDefaultsOnly, EditFixedSize, Category = "Data", meta = (EditCondition = "ItemType == EItemType::Boots", EditConditionHides,DisplayName = "Datas"))
		TArray<FItemAppearanceData> BootsAppearanceDatas;

	UPROPERTY(EditDefaultsOnly, EditFixedSize, Category = "Data", meta = (EditCondition = "ItemType == EItemType::Gloves", EditConditionHides,DisplayName = "Datas"))
		TArray<FItemAppearanceData> GlovesAppearanceDatas;
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

	FORCEINLINE UItemObject* GetItemObject() const { return ItemObject; }
	FORCEINLINE EItemType GetType() const { return ItemType; }
	FORCEINLINE FItemStatusData GetItemStatus() const { return ItemStatus; }
	const TArray<FItemAppearanceData>& GetAppearanceDatas()const;
	
};
