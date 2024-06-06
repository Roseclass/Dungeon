#include "Components/SkillComponent.h"
#include "Global.h"

#include "SaveManager.h"
#include "Characters/DungeonCharacterBase.h"
#include "Characters/GABase.h"

USkillComponent::USkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USkillComponent::BeginPlay()
{
	Super::BeginPlay();
	GiveDefaultAbilities();
}

void USkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USkillComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void USkillComponent::GiveDefaultAbilities()
{
	CheckTrue(GetOwner()->GetLocalRole() != ROLE_Authority);

	CheckNull(DataTable);
	DataTable->GetAllRows<FSkillData>("", SkillDatas);

	for (auto i : SkillDatas)
	{
		if (!i->SkillClass)continue;
		GiveAbility(FGameplayAbilitySpec(
			i->SkillClass,
			GetSkillLevel(i->SkillClass.GetDefaultObject()->GetSkillID()),
			i->SkillClass.GetDefaultObject()->GetSkillID(),
			this
		));
	}

	for (int32 i = 0; i < int32(EQuickSlotPosition::Max); ++i)
		QuickSlotData[i] = -1;

}

void USkillComponent::UseSkill(int32 InSkillID)
{
	AbilityLocalInputPressed(InSkillID);
}

void USkillComponent::UseQuickSlotSkill(int32 InQuickSlotIndex)
{
	CheckFalse(InQuickSlotIndex < int32(EQuickSlotPosition::Max));
	UseSkill(QuickSlotData[InQuickSlotIndex]);
}

void USkillComponent::ChangeQuickSlotData(int32 InQuickSlotIndex, int32 InSkillID)
{
	CheckFalse(InQuickSlotIndex < int32(EQuickSlotPosition::Max));
	int32 prevID = QuickSlotData[InQuickSlotIndex];
	QuickSlotData[InQuickSlotIndex] = InSkillID;

	for (auto i : SkillDatas)
	{
		if (!i->SkillClass)continue;
		if (i->SkillClass.GetDefaultObject()->GetSkillID() == InSkillID)
		{
			OnQuickSlotDataChanged.Broadcast(InQuickSlotIndex, *i);
			break;
		}
	}

	for (int32 i = 0; i < int32(EQuickSlotPosition::Max); ++i)
	{
		if (i == InQuickSlotIndex)continue;
		if (QuickSlotData[i] == InSkillID)
		{
			OnQuickSlotDataChanged.Broadcast(i, FSkillData());
			QuickSlotData[i] = -1;
		}
	}
}

void USkillComponent::SetCoolDown(int32 InSkillID)
{

}

bool USkillComponent::GetQuickSlotCoolDown(int32 InQuickSlotIndex,float& Result)
{
	return 1;
}

bool USkillComponent::IsQuickSlotCoolDown(int32 InQuickSlotIndex)
{
	return 1;
}

float USkillComponent::GetQuickSlotManaCost(int32 InQuickSlotIndex)
{
	return 1;
}

bool USkillComponent::GetSkillRange(int32 InSkillID, float& Range)
{
	return 1;
}

bool USkillComponent::GetQuickSlotSkillRange(int32 InQuickSlotIndex, float& Range)
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

int32 USkillComponent::GetSkillLevel(int32 InSkillID) const
{
	return 1;
}

const TArray<const FSkillData*> USkillComponent::GetSkillDatas() const
{
	TArray<const FSkillData*> result;
	for (auto i : SkillDatas)
	{
		const FSkillData* temp = i;
		result.Add(temp);
	}
	return result;
}

const FSkillData* USkillComponent::GetSkillData(int32 InSkillID)const
{
	for (auto i : SkillDatas)
	{
		if (!i->SkillClass)continue;
		if (i->SkillClass.GetDefaultObject()->GetSkillID() == InSkillID)
			return i;
	}
	return nullptr;
}
