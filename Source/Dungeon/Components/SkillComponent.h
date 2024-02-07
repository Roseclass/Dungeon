#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillComponent.generated.h"

/**
 * 캐릭터가 스킬을 사용할 수 있게 해주는 컴포넌트
 * 정해진 프리셋에 스킬 데이터를 넣어 스킬 사용을 명령
 * ex)skill01(whirlwind)->use->if(canuse)->PlayAnimMontage
 * 플레이어와 몬스터 공용으로 사용됨
 */

USTRUCT(BlueprintType)
struct FSkillData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
		UAnimMontage* Montage;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()
	//property
private:
	UPROPERTY(EditDefaultsOnly, meta = (ToolTip = "array order : left, right, q, w, e, r....."))
		TArray<FSkillData> SkillDatas;
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
	void UseSkill(int32 Idx);
	void UseLeft();
	void UseRight();
	void UseQ();
	void UseW();
	void UseE();
	void UseR();
};
