#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "Particles/ParticleSystemComponent.h"
#include "Characters/CharcterAppearanceData.h"
#include "Components/TimeLineComponent.h"
#include "Eqquipment.generated.h"

class UFXSystemAsset;
class UShapeComponent;
class UBoxComponent;
class UMeshComponent;
class USplineComponent;
class UWidgetComponent;
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

friend class AEqquipment;

	// this data will be saved
private:
	UPROPERTY(EditDefaultsOnly, Category = "WidgetInfo")
		FString Name;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
		float BaseDamage;

	UPROPERTY()
		float FinalDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
		float BaseMaxHealth;
	
	UPROPERTY()
		float FinalMaxHealth;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
		float BaseHealthRegen;

	UPROPERTY()
		float FinalHealthRegen;

	UPROPERTY(EditDefaultsOnly, Category = "Mana")
		float BaseMaxMana;

	UPROPERTY()
		float FinalMaxMana;

	UPROPERTY(EditDefaultsOnly, Category = "Mana")
		float BaseManaRegen;

	UPROPERTY()
		float FinalManaRegen;

	UPROPERTY()
		bool bRandomize;

public:
	FORCEINLINE FString GetName()const { return Name; }
	FORCEINLINE float GetFinalDamage()const { return BaseDamage; }
	FORCEINLINE float GetFinalMaxHealth()const { return FinalMaxHealth; }
	FORCEINLINE float GetFinalHealthRegen()const { return FinalHealthRegen; }
	FORCEINLINE float GetFinalMaxMana()const { return FinalMaxMana; }
	FORCEINLINE float GetFinalManaRegen()const { return FinalManaRegen; }
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
	virtual void PostNetReceiveLocationAndRotation()override;
public:	
	virtual void Tick(float DeltaTime) override;

	//property
private:
	bool bUpdateLocationAndRotation = 1;
	AItemManager* Manager;
	UShapeComponent* InteractCollisionComponent;
	TArray<UMeshComponent*> MeshComponents;
	UPROPERTY()UItemObject* ItemObject;
	UPROPERTY(Replicated)ACharacter* OwnerCharacter;
	bool bPickable;

	UPROPERTY(ReplicatedUsing = "OnRep_Mode")EItemMode Mode = EItemMode::Max;

	UPROPERTY()UNiagaraComponent* NiagaraPickEffect;
	UPROPERTY()UParticleSystemComponent* ParticlePickEffect;

	UPROPERTY(VisibleDefaultsOnly)
		USceneComponent* Root;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, meta = (AllowPrivateAccess = true))
		USplineComponent* DropSpline;

	UPROPERTY(VisibleDefaultsOnly)
		UWidgetComponent* NameWidget;

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

	UPROPERTY(EditDefaultsOnly, Category = "Drop")
		float Speed = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Drop")
		uint8 RotationCount = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Drop", meta = (ClampMax = 90.00, ClampMin = 0.00, DisplayPriority = 0))
		uint8 RotationAngle = 45;

	UPROPERTY(EditDefaultsOnly, Category = "Drop")
		UCurveFloat* DropCurve;
	FTimeline DropTimeLine;
	FOnTimelineFloat DropTimelineFloat;

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

	UFUNCTION()void DropTickFunction(float Value);
public:
	virtual void SetOwnerCharacter(ACharacter* InCharacter);

	virtual void SetItemLocation(const FVector& NewLocation, bool bSweep = false, FHitResult* OutSweepHitResult = nullptr, ETeleportType Teleport = ETeleportType::None);
	virtual void SetItemRotation(FRotator NewRotation, ETeleportType Teleport = ETeleportType::None);
	virtual void AttachItemToComponent(USceneComponent* Parent, const FAttachmentTransformRules& AttachmentRules, FName InSocketName = NAME_None);
	virtual void ChangeVisibility(EItemMode InMode);
	virtual void SetMode(EItemMode InMode);
	virtual void PlayDropSequence(FVector Start,FVector End);

	void StartCursorOver();
	void EndCursorOver();

	FORCEINLINE void SetManager(AItemManager* InManager) { Manager = InManager; OnRep_Mode(); }

	FORCEINLINE bool HasOwnerCharacter() const { return OwnerCharacter != nullptr; }
	FORCEINLINE ACharacter* GetOwnerCharacter() const { return OwnerCharacter; }
	FORCEINLINE UItemObject* GetItemObject() const { return ItemObject; }
	FORCEINLINE EItemType GetType() const { return ItemType; }
	FORCEINLINE FItemStatusData GetItemStatus() const { return ItemStatus; }
	const TArray<FItemAppearanceData>& GetAppearanceDatas()const;
	
};
