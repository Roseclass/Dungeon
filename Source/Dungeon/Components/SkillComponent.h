#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillComponent.generated.h"

/**
 * 정해진 프리셋에 스킬 데이터를 저장해 보관
 * 캐릭터에서 스킬 사용시 필요한 데이터를 받아감
 */

class ASkillActor;
struct FSkillData;

DECLARE_MULTICAST_DELEGATE_TwoParams(FQuickSlotDataChanged, int32, ASkillActor*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FQuickSlotCoolDown, int32, float);

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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//property
private:
	UPROPERTY(EditDefaultsOnly, Category = "Init", meta = (ToolTip = "If SkillActors using with another comp, control generate order in OwnerActor"))
		bool bAutoGenerate = 1;
		
	UPROPERTY(EditDefaultsOnly, Category = "Init", meta = (ToolTip = "array order : left, right, q, w, e, r....."))
		TArray<TSubclassOf<ASkillActor>> SkillActorClasses;
		
	ASkillActor* CurrentSkill;
	UPROPERTY(Replicated, ReplicatedUsing = "OnRep_SkillActors")TArray<ASkillActor*> SkillActors;
	TArray<ASkillActor*> QuickSlotSkillActors;
protected:
public:
	FQuickSlotDataChanged OnQuickSlotDataChanged;
	FQuickSlotCoolDown OnQuickSlotCoolDown;

	//function
private:
	UFUNCTION() void OnRep_SkillActors();
protected:
public:
	void SpawnSkillActors();
	void UseSkill(int32 Idx);
	void UseQuickSlotSkill(int32 Idx);
	void SpawnProjectile();
	void ChangeQuickSlotData(int32 Index, ASkillActor* InSkillActor);
	void SetCoolDown(ASkillActor* InSkillActor);
	bool GetQuickSlotCoolDown(int32 Index, float& Result);
	bool IsQuickSlotCoolDown(int32 Index);
	bool GetSkillRange(int32 InIndex, float& Range);
	bool GetQuickSlotSkillRange(int32 InIndex, float& Range);


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
