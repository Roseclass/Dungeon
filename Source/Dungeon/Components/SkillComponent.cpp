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

void USkillComponent::SetSkill(FSkillData* Data)
{
	CurrentSkill = Data;
}

void USkillComponent::UnsetSkill()
{
	CurrentSkill = nullptr;
}
