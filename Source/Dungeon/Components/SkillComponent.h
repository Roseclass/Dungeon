#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Engine/DataTable.h"
#include "Characters/GameplayEnums.h"
#include "SkillComponent.generated.h"

/**
 * 
 */

class UDataTable;
class UMediaSource;
class UGABase;
class USaveGameData;

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
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Skills|Datas")
		UDataTable* DataTable;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Skills|Tags")
		FGameplayTagContainer BlockUseTags;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Skills|Tatas")
		FGameplayTagContainer BlockMoveTags;

public:
	FQuickSlotDataChanged OnQuickSlotDataChanged;
	FQuickSlotCoolDown OnQuickSlotCoolDown;

	//function
private:
	void GiveDefaultAbilities();
protected:
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
	bool CanUse()const;
	bool CanMove()const;

	// save
	void SaveData(USaveGameData* SaveData);
	void LoadData(USaveGameData* const ReadData);

	//TODO:: change to ref
	int32 GetSkillLevel(int32 InSkillID) const;
	const TArray<const FSkillData*> GetSkillDatas() const;
	const FSkillData* GetSkillData(int32 InSkillID)const;
};
