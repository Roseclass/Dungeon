#include "Components/SkillComponent.h"
#include "Global.h"

#include "SaveManager.h"
#include "Characters/DungeonCharacterBase.h"

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

void USkillComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void USkillComponent::SpawnSkillActors()
{
	for(auto i : ActivatableAbilities.Items)
		i.InputID
}

void USkillComponent::UseSkill(int32 Idx)
{

}

void USkillComponent::UseQuickSlotSkill(int32 Idx)
{

}

void USkillComponent::Abort()
{

}

void USkillComponent::SpawnProjectile()
{

}

void USkillComponent::SpawnWarningSign(int32 InIndex)
{

}

void USkillComponent::ChangeQuickSlotData(int32 Index, ASkillActor* InSkillActor)
{

}

void USkillComponent::SetCoolDown(ASkillActor* InSkillActor)
{

}

bool USkillComponent::GetQuickSlotCoolDown(int32 Index,float& Result)
{
	return 1;
}

bool USkillComponent::IsQuickSlotCoolDown(int32 Index)
{
	return 1;
}

float USkillComponent::GetQuickSlotManaCost(int32 Index)
{
	return 1;
}

bool USkillComponent::GetSkillRange(int32 InIndex, float& Range)
{
	return 1;
}

bool USkillComponent::GetQuickSlotSkillRange(int32 InIndex, float& Range)
{
	return 1;
}

void USkillComponent::SaveData(USaveGameData* SaveData)
{
	// reset Datas
	//SaveData->PlayerData.SlotSkills.Empty();

	//for (auto i : QuickSlotSkillActors)
	//	if (i)SaveData->PlayerData.SlotSkills.Add(i->GetClass());
	//	else SaveData->PlayerData.SlotSkills.Add(nullptr);



	//QuickSlotData[EQuickSlotPosition::Max];
}

void USkillComponent::LoadData(USaveGameData* const ReadData)
{
	//for (int32 i = 0; i < ReadData->PlayerData.SlotSkills.Num(); ++i)
	//{
	//	for(auto skill : SkillActors)
	//		if(skill->GetClass() == ReadData->PlayerData.SlotSkills[i])
	//			ChangeQuickSlotData(i, skill);
	//}

	//bLoad = 1;
	
	//QuickSlotData[EQuickSlotPosition::Max];
}
