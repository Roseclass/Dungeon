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
	UPROPERTY(EditDefaultsOnly, meta = (ToolTip = "array order : left, right, q, w, e, r....."))
		TArray<TSubclassOf<ASkillActor>> SkillActorClasses;
		
	ASkillActor* CurrentSkill;
	TArray<ASkillActor*> SkillActors;
protected:
public:

	//function
private:
	void SpawnSkillActors();
protected:
public:
	void UseSkill(int32 Idx);
	void SpawnProjectile();

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
