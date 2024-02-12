#include "Components/SkillComponent.h"
#include "Global.h"

#include "Characters/DungeonCharacter.h"
#include "Objects/SkillActor.h"

USkillComponent::USkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USkillComponent::BeginPlay()
{
	Super::BeginPlay();
	if(bAutoGenerate)SpawnSkillActors();
}

void USkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USkillComponent::SpawnSkillActors()
{
	for (auto i : SkillActorClasses)
		SkillActors.Add(GetWorld()->SpawnActor<ASkillActor>(i));
	for (auto i : SkillActors)
		i->SetOwnerCharacter(Cast<ADungeonCharacter>(GetOwner()));

	for (auto i : SkillActors)
		for (auto j : SkillActors)
		{
			if (i->GetSkillData()->PannelPosition == j->GetSkillData()->ParentPosition)
			{
				i->AddChild(j);
				j->SetParent(i);
			}
		}
}

void USkillComponent::UseSkill(int32 Idx)
{
	CheckFalse(SkillActors.IsValidIndex(Idx));
	CurrentSkill = SkillActors[Idx];
	CheckNull(CurrentSkill);
	CurrentSkill->Use();
}

void USkillComponent::SpawnProjectile()
{
	CheckNull(CurrentSkill);
	CurrentSkill->SpawnProjectile();
}
