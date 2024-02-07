#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillComponent.generated.h"

/**
 * ĳ���Ͱ� ��ų�� ����� �� �ְ� ���ִ� ������Ʈ
 * ������ �����¿� ��ų �����͸� �־� ��ų ����� ���
 * ex)skill01(whirlwind)->use->if(canuse)->PlayAnimMontage
 * �÷��̾�� ���� �������� ����
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
