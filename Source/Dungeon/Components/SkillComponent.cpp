#include "Components/SkillComponent.h"
#include "Global.h"

USkillComponent::USkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USkillComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USkillComponent::UseSkill(int32 Idx)
{
	CheckFalse(SkillDatas.IsValidIndex(Idx));
}

void USkillComponent::UseLeft()
{
	UseSkill(0);
}

void USkillComponent::UseRight()
{
	UseSkill(1);
}

void USkillComponent::UseQ()
{
	UseSkill(2);
}

void USkillComponent::UseW()
{
	UseSkill(3);
}

void USkillComponent::UseE()
{
	UseSkill(4);
}

void USkillComponent::UseR()
{
	UseSkill(5);
}
