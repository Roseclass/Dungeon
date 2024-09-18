#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "Particles/ParticleSystemComponent.h"
#include "Characters/CharcterAppearanceData.h"
#include "Abilities/AbilityTaskTypes.h"
#include "Components/TimeLineComponent.h"
#include "Interfaces/IInteractable.h"
#include "AttributeSet.h"
#include "Eqquipment.generated.h"

class UGameplayEffect;
class UFXSystemAsset;
class UShapeComponent;
class UBoxComponent;
class UMeshComponent;
class USplineComponent;
class UWidgetComponent;
class UMaterialInstance;
class ADungeonCharacterBase;
class UItemObject;
class UNiagaraComponent;
class USceneComponent;
class UParticleSystemComponent;

UENUM()
enum class EEquipmentInitType : uint8
{
	Spawned, Placed, Max
};

UENUM()
enum class EItemMode : uint8
{
	Loot /*Visible,Interactable,NoOwner*/,
	Inventory /*Invisible,NotInteractable,HasOwner*/,
	Equip /*Visible,NotInteractable,HasOwner*/,
	Drop /*Visible,Interactable After DropSequence,NoOwner*/,
	Max
};

UENUM(BlueprintType)
enum class EItemGrade : uint8
{
	Common,
	Uncommon,
	Rare,
	Unique,
	Max
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

	UPROPERTY()
		bool bRandomize;

	UPROPERTY()
		TArray<FSkillEnhancement> EnhancementDatas;

	UPROPERTY()
		TArray<EItemGrade> EnhancementGrades;

	UPROPERTY()
		TArray<FGameplayAttribute> TargetAttributes;

	UPROPERTY()
		TArray<EItemGrade> TargetAttributeGrades;

	UPROPERTY()
		TArray<float> TargetAttributeValues;

public:
	FORCEINLINE FString GetName()const { return Name; }
	FORCEINLINE const TArray<FSkillEnhancement>& GetEnhancementDatas()const { return EnhancementDatas; }
	FORCEINLINE const TArray<EItemGrade>& GetEnhancementGrades()const { return EnhancementGrades; }
	FORCEINLINE const TArray<FGameplayAttribute>& GetTargetAttributes()const { return TargetAttributes; }
	FORCEINLINE const TArray<EItemGrade>& GetTargetAttributeGrades()const { return TargetAttributeGrades; }
	FORCEINLINE const TArray<float>& GetTargetAttributeValues()const { return TargetAttributeValues; }
};

USTRUCT()
struct FEquipmentStateUpdateParameters
{
	GENERATED_BODY()
public:
	UPROPERTY()EItemMode State;
	UPROPERTY()ADungeonCharacterBase* NewOwner;
	UPROPERTY()FVector Location;
	UPROPERTY()FVector DropStart;
	UPROPERTY()FVector DropEnd;
};

UCLASS()
class DUNGEON_API AEqquipment : public AActor, public IIInteractable
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

	virtual void StartInteract(ADungeonPlayerController* InPlayer)override;
	virtual void EndInteract(ADungeonPlayerController* InPlayer)override;
	virtual void StartCursorOver(ADungeonPlayerController* InPlayer)override;
	virtual void EndCursorOver(ADungeonPlayerController* InPlayer)override;
	virtual bool IsInteractable()override;

	//property
private:
	FString UniqueID;
	TArray<UMeshComponent*> MeshComponents;
	UPROPERTY()UShapeComponent* InteractCollisionComponent;
	UPROPERTY()UItemObject* ItemObject;
	ADungeonCharacterBase* OwnerCharacter;
	bool bInteractable;

	EItemMode Mode = EItemMode::Max;

	UPROPERTY()UNiagaraComponent* NiagaraPickEffect;
	UPROPERTY()UParticleSystemComponent* ParticlePickEffect;

	UPROPERTY(VisibleDefaultsOnly)
		USceneComponent* Root;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, meta = (AllowPrivateAccess = true))
		USplineComponent* DropSpline;

	UPROPERTY(VisibleDefaultsOnly)
		UWidgetComponent* NameWidget;

	//InventroyData
	UPROPERTY(EditDefaultsOnly, Category = "InventroyData")
		int32 DimensionX;

	UPROPERTY(EditDefaultsOnly, Category = "InventroyData")
		int32 DimensionY;

	UPROPERTY(EditDefaultsOnly, Category = "InventroyData")
		UMaterialInstance* Icon;

	UPROPERTY(EditDefaultsOnly, Category = "InventroyData")
		UMaterialInstance* IconRotated;

	//Loot
	UPROPERTY(EditDefaultsOnly, Category = "Loot")
		UFXSystemAsset* LootEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Loot")
		TArray<FParticleSysParam> LootEffectParams;

	UPROPERTY(EditDefaultsOnly, Category = "Loot")
		FQuat LootEffectWorldRotation;

	//Drop
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
	FVector DropStart;
	FVector DropEnd;

	//Data
	UPROPERTY(EditDefaultsOnly, Category = "Data")
		EItemType ItemType;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
		FItemStatusData ItemStatus;

	UPROPERTY(EditAnywhere, Category = "Data")
		EEquipmentInitType InitType;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
		UDataTable* EnhancementDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
		TMap<FGameplayAttribute,float> TargetAttributes;

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
	UPROPERTY(EditAnywhere, Category = "GameplayEffect")
		TSubclassOf<UGameplayEffect> CommonEffectClass;

public:

	//function
private:
	EItemGrade CheckGrade(float Rate);
protected:
	virtual void FindComponents();
	virtual void SpawnLootEffects();
	virtual void SetEffectLocation();
	virtual void SortMesh();
	virtual void ActivateEffect();
	virtual void DeactivateEffect();

	virtual void SetLootMode(const FEquipmentStateUpdateParameters& UpdateParameters);
	virtual void SetInventoryMode(const FEquipmentStateUpdateParameters& UpdateParameters);
	virtual void SetEquipMode(const FEquipmentStateUpdateParameters& UpdateParameters);
	virtual void SetDropMode(const FEquipmentStateUpdateParameters& UpdateParameters);

	virtual void SetOwnerCharacter(ADungeonCharacterBase* InCharacter);

	virtual void PlayDropSequence(FVector Start,FVector End);
	UFUNCTION()void DropTickFunction(float Value);
public:
	void AssignUniqueID(FString NewUniqueID);
	virtual void UpdateState(const FEquipmentStateUpdateParameters& UpdateParameters);
	virtual void UpdateStatus(const FItemStatusData& UpdateData);

	void Load(const FItemStatusData& InData);

	FORCEINLINE bool HasOwnerCharacter() const { return OwnerCharacter != nullptr; }
	FORCEINLINE ADungeonCharacterBase* GetOwnerCharacter() const { return OwnerCharacter; }
	FORCEINLINE UItemObject* GetItemObject() const { return ItemObject; }
	FORCEINLINE EItemType GetType() const { return ItemType; }
	FORCEINLINE EEquipmentInitType GetInitType() const { return InitType; }
	FORCEINLINE const FItemStatusData& GetItemStatus() const { return ItemStatus; }
	FORCEINLINE FString GetUniqueID() const { return UniqueID; }
	FORCEINLINE virtual FName GetSocketName() const { return NAME_None; }
	FORCEINLINE const TMap<FGameplayAttribute, float>& GetTargetAttributes() const { return TargetAttributes; }
	const TArray<FItemAppearanceData>& GetAppearanceDatas()const;
	FEquipmentStateUpdateParameters GetEquipmentState() const;
	void GetDimensions(int32& X, int32& Y);
	void GetAllEffectClasses(TArray<TSubclassOf<UGameplayEffect>>& Classes)const;
};
