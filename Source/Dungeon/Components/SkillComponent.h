#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SkillComponent.generated.h"

/**
 * 
 */

class UDataTable;
class ASkillActor;
class USaveGameData;
struct FSkillData;

DECLARE_MULTICAST_DELEGATE_TwoParams(FQuickSlotDataChanged, int32, ASkillActor*);
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
	/*
	* 퀵슬롯 데이터 구분할건지?
	* give시에 설정한 inputid로 구분
	* 
	*/
	int32 QuickSlotData[EQuickSlotPosition::Max];
protected:
	/*
	* 스킬 데이터 테이블
	*/

	UPROPERTY(EditDefaultsOnly, Category = "Datas")
		UDataTable* DataTable;
public:
	FQuickSlotDataChanged OnQuickSlotDataChanged;
	FQuickSlotCoolDown OnQuickSlotCoolDown;

	//function
private:
protected:
public:
	void SpawnSkillActors();
	void UseSkill(int32 Idx);
	void UseQuickSlotSkill(int32 Idx);
	void Abort();
	void SpawnProjectile();
	void SpawnWarningSign(int32 InIndex);
	void ChangeQuickSlotData(int32 Index, ASkillActor* InSkillActor);
	void SetCoolDown(ASkillActor* InSkillActor);
	bool GetQuickSlotCoolDown(int32 Index, float& Result);
	bool IsQuickSlotCoolDown(int32 Index);
	float GetQuickSlotManaCost(int32 Index);
	bool GetSkillRange(int32 InIndex, float& Range);
	bool GetQuickSlotSkillRange(int32 InIndex, float& Range);

	// save
	void SaveData(USaveGameData* SaveData);
	void LoadData(USaveGameData* const ReadData);

	//TODO:: change to ref
	FORCEINLINE TArray<ASkillActor*> GetSkillActors() const { return TArray<ASkillActor*>(); };
	FORCEINLINE ASkillActor* GetSkillActor(int32 Idx) const { return nullptr; };
	FORCEINLINE ASkillActor* GetLeft() const { return nullptr; };
	FORCEINLINE ASkillActor* GetRight() const { return nullptr; };
	FORCEINLINE ASkillActor* GetQ() const { return nullptr; };
	FORCEINLINE ASkillActor* GetW() const { return nullptr; };
	FORCEINLINE ASkillActor* GetE() const { return nullptr; };
	FORCEINLINE ASkillActor* GetR() const { return nullptr; };
	FORCEINLINE ASkillActor* GetCurrentSkill() const { return nullptr; };
};
