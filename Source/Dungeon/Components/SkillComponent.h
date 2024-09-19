#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayEnums.h"
#include "Abilities/AbilityTaskTypes.h"
#include "Engine/NetSerialization.h"
#include "SkillComponent.generated.h"

/**
 * 
 */

class UDataTable;
class UMediaSource;
class UGABase;
class USaveGameData;

class AWarningSign;
class UNiagaraSystem;

enum class ESkillTreeSkillState : uint8;

USTRUCT(BlueprintType)
struct FSkillData : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Class")
		TSubclassOf<UGABase> SkillClass;

	UPROPERTY(EditAnywhere, Category = "Class")
		ESkillTreeSkillState BaseState;

	UPROPERTY(EditAnywhere, Category = "Widget", meta = (DisplayThumbnail = "true", DisplayName = "Image", AllowedClasses = "Texture,MaterialInterface,SlateTextureAtlasInterface", DisallowedClasses = "MediaTexture"))
		TObjectPtr<UObject> SkillImage;

	UPROPERTY(EditAnywhere, Category = "Widget")
		FVector2D PannelPosition;

	UPROPERTY(EditAnywhere, Category = "Widget")
		FVector2D ParentPosition = FVector2D(-1, -1);

	UPROPERTY(EditAnywhere, Category = "Widget")
		UMediaSource* PreviewSource;

	UPROPERTY(EditAnywhere, Category = "Widget")
		FText Name;

	UPROPERTY(EditAnywhere, Category = "Widget")
		FText Description;
};

USTRUCT()
struct FSkillEhancementData : public FFastArraySerializerItem
{
	GENERATED_BODY()
public:
	UPROPERTY()float CostAdditive = 0;
	UPROPERTY()float CostMultiplicitive = 100;
	UPROPERTY()float CooldownAdditive = 0;
	UPROPERTY()float CooldownMultiplicitive = 100;
	UPROPERTY()float DamageAdditive = 0;
	UPROPERTY()float DamageMultiplicitive = 100;
};

USTRUCT()
struct FSkillEnhancementArray : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()TArray<FSkillEhancementData> Items;

	void AddSkillEnhancement(const FSkillEhancementData& NewSkill)
	{
		Items.Add(NewSkill);
		MarkItemDirty(Items.Last());
	}

	void RemoveSkillEnhancement(int32 Index)
	{
		if (Items.IsValidIndex(Index))
		{
			Items.RemoveAt(Index);
			MarkArrayDirty();
		}
	}

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FSkillEhancementData, FSkillEnhancementArray>(Items, DeltaParms, *this);
	}
};

template<>
struct TStructOpsTypeTraits<FSkillEnhancementArray> : public TStructOpsTypeTraitsBase2<FSkillEnhancementArray>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FQuickSlotDataChanged, int32, const FSkillData&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FQuickSlotCoolDown, int32, float);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API USkillComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	USkillComponent();
protected:
	virtual void BeginPlay() override;	
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//property
private:
	bool bLoad;
	int32 QuickSlotData[EQuickSlotPosition::Max];
	TArray<FSkillData*> SkillDatas;
	UPROPERTY(Replicated)FSkillEnhancementArray EnhancementDatas;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Skills|Datas")
		UDataTable* DataTable;

	UPROPERTY(EditDefaultsOnly, Category = "Skills|Datas")
		TArray<TSubclassOf<UGABase>> DefaultGAs;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Skills|Tags")
		FGameplayTagContainer BlockUseTags;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Skills|Tags")
		FGameplayTagContainer BlockMoveTags;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Skills|Tags")
		FGameplayTagContainer DeadTags;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Skills|Tags")
		FGameplayTagContainer HitTags;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Skills|Tags")
		FGameplayTag CostBaseTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Skills|Tags")
		FGameplayTag CostAdditiveTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Skills|Tags")
		FGameplayTag CostMultiplicitiveTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Skills|Tags")
		FGameplayTag CooldownBaseTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Skills|Tags")
		FGameplayTag CooldownAdditiveTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Skills|Tags")
		FGameplayTag CooldownMultiplicitiveTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Skills|Tags")
		FGameplayTag DamageAdditiveTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Skills|Tags")
		FGameplayTag DamageMultiplicitiveTag;

public:
	FQuickSlotDataChanged OnQuickSlotDataChanged;
	FQuickSlotCoolDown OnQuickSlotCoolDown;

	//function
private:
	void GiveDefaultAbilities();
protected:
	UFUNCTION()virtual void HitReaction(UAbilitySystemComponent* InComponent, const FGameplayEffectSpec& InSpec, FActiveGameplayEffectHandle InHandle);
	UFUNCTION()virtual void OnCannotMoveTagChanged(FGameplayTag Tag, int32 NewCount);
public:
	void UseSkill(int32 InSkillID);
	void UseQuickSlotSkill(int32 InQuickSlotIndex);
	void ChangeQuickSlotData(int32 InQuickSlotIndex, int32 InSkillID);
	void SetCoolDown(int32 InSkillID);
	bool GetQuickSlotCoolDown(int32 InQuickSlotIndex, float& Result);
	bool IsQuickSlotCoolDown(int32 InQuickSlotIndex);
	float GetQuickSlotManaCost(int32 InQuickSlotIndex);
	bool GetSkillRange(int32 InSkillID, float& Range);
	bool GetQuickSlotSkillRange(int32 InQuickSlotIndex, float& Range);
	
	// ability
	void EnhanceAbility(const TArray<FSkillEnhancement>& InDatas, float Rate = 1.0f);
	FSkillEhancementData GetEhancementData(FGameplayTag AbilityTag);

	// Reliable cue
	UFUNCTION(Client, Reliable)void Cient_DamageText(float InDamage, bool IsCritical, FVector InLocation);
	UFUNCTION(NetMulticast, Reliable)void Multicast_WarningSign(TSubclassOf<AWarningSign> Class, FTransform const& Transform, AActor* InOwner, APawn* Instigator, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride,float Duration, float ExtraDuration);
	UFUNCTION(NetMulticast, Reliable)void Multicast_FXEffect_Transform(UNiagaraSystem* NiagaraFX, FTransform const& Transform);
	UFUNCTION(NetMulticast, Reliable)void Multicast_FXEffect_Attached(UNiagaraSystem* NiagaraFX, AActor* AttachTarget);

	// teleport
	UFUNCTION(Reliable, Server)void Server_Teleport(const FHitResult& HitResult, const float MaxDist);

	//state
	bool CanUse()const;
	bool CanMove()const;
	bool IsDead()const;
	bool IsHit()const;

	// save
	void SaveData(USaveGameData* SaveData);
	void LoadData(USaveGameData* const ReadData);

	//TODO:: change to ref
	int32 GetSkillLevel(int32 InSkillID) const;
	const TArray<const FSkillData*> GetSkillDatas() const;
	const FSkillData* GetSkillData(int32 InSkillID)const;
	float GetDefense() const;
	float GetPower() const;
};
