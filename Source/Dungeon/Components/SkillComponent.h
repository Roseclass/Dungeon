#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillComponent.generated.h"

/**
 * 정해진 프리셋에 스킬 데이터를 저장해 보관
 * 캐릭터에서 스킬 사용시 필요한 데이터를 받아감
 */

USTRUCT(BlueprintType)
struct FSkillData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		UAnimMontage* Montage;

	UPROPERTY(EditAnywhere)
		float PlayRate = 1.0f;

	UPROPERTY(EditAnywhere)
		FName StartSection;

	UPROPERTY(EditAnywhere)
		FName RepeatSection;

	UPROPERTY(EditAnywhere)
		bool bCanMove = false;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()
	//property
private:
	UPROPERTY(EditDefaultsOnly, meta = (ToolTip = "array order : left, right, q, w, e, r....."))
		TArray<FSkillData> SkillDatas;

	FSkillData* CurrentSkill;
protected:
public:

	//engine
public:
	USkillComponent();
protected:
	virtual void BeginPlay() override;
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//function
private:
protected:
public:
	void SetSkill(FSkillData* Data);
	void UnsetSkill();

	FORCEINLINE FSkillData* GetSkillData(int32 Idx) { return SkillDatas.IsValidIndex(Idx) ? &SkillDatas[Idx] : nullptr; };
	FORCEINLINE FSkillData* GetLeft() { return SkillDatas.IsValidIndex(0) ? &SkillDatas[0] : nullptr; };
	FORCEINLINE FSkillData* GetRight() { return SkillDatas.IsValidIndex(1) ? &SkillDatas[1] : nullptr; };
	FORCEINLINE FSkillData* GetQ() { return SkillDatas.IsValidIndex(2) ? &SkillDatas[2] : nullptr; };
	FORCEINLINE FSkillData* GetW() { return SkillDatas.IsValidIndex(3) ? &SkillDatas[3] : nullptr; };
	FORCEINLINE FSkillData* GetE() { return SkillDatas.IsValidIndex(4) ? &SkillDatas[4] : nullptr; };
	FORCEINLINE FSkillData* GetR() { return SkillDatas.IsValidIndex(5) ? &SkillDatas[5] : nullptr; };
};
