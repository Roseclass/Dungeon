#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillComponent.generated.h"

/**
 * ������ �����¿� ��ų �����͸� ������ ����
 * ĳ���Ϳ��� ��ų ���� �ʿ��� �����͸� �޾ư�
 */

class ASkillActor;
struct FSkillData;

DECLARE_MULTICAST_DELEGATE_TwoParams(FQuickSlotDataChanged, int32, ASkillActor*);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USkillComponent();
protected:
	virtual void BeginPlay() override;
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//property
private:
	UPROPERTY(EditDefaultsOnly, Category = "Init", meta = (ToolTip = "If SkillActors using with another comp, control generate order in OwnerActor"))
		bool bAutoGenerate = 1;
		
	UPROPERTY(EditDefaultsOnly, Category = "Init", meta = (ToolTip = "array order : left, right, q, w, e, r....."))
		TArray<TSubclassOf<ASkillActor>> SkillActorClasses;
		
	ASkillActor* CurrentSkill;
	TArray<ASkillActor*> SkillActors;
	TArray<ASkillActor*> QuickSlotSkillActors;
protected:
public:
	FQuickSlotDataChanged OnQuickSlotDataChanged;

	//function
private:
	UFUNCTION(NetMulticast, Reliable)void Multicast_SetSkillActorDatas(const TArray<ASkillActor*>& Array);
protected:
public:
	void SpawnSkillActors();
	void UseSkill(int32 Idx);
	void SpawnProjectile();
	void ChangeQuickSlotData(int32 Index, ASkillActor* InSkillActor);
	void GetQuickSlotCoolTime(int32 Index, float& Current, float& Max);
	bool IsQuickSlotCoolTime(int32 Index);

	FORCEINLINE const TArray<ASkillActor*>& GetSkillActors() const { return SkillActors; };
	FORCEINLINE ASkillActor* GetSkillActor(int32 Idx) const { return SkillActors.IsValidIndex(Idx) ? SkillActors[Idx] : nullptr; };
	FORCEINLINE ASkillActor* GetLeft() const { return SkillActors.IsValidIndex(0) ? SkillActors[0] : nullptr; };
	FORCEINLINE ASkillActor* GetRight() const { return SkillActors.IsValidIndex(1) ? SkillActors[1] : nullptr; };
	FORCEINLINE ASkillActor* GetQ() const { return SkillActors.IsValidIndex(2) ? SkillActors[2] : nullptr; };
	FORCEINLINE ASkillActor* GetW() const { return SkillActors.IsValidIndex(3) ? SkillActors[3] : nullptr; };
	FORCEINLINE ASkillActor* GetE() const { return SkillActors.IsValidIndex(4) ? SkillActors[4] : nullptr; };
	FORCEINLINE ASkillActor* GetR() const { return SkillActors.IsValidIndex(5) ? SkillActors[5] : nullptr; };
	FORCEINLINE ASkillActor* GetCurrentSkill() const { return CurrentSkill; };
};
