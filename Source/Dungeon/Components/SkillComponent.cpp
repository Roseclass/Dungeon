#include "Components/SkillComponent.h"
#include "Global.h"

#include "SaveManager.h"
#include "Characters/DungeonCharacterBase.h"
#include "Characters/GABase.h"
#include "Components/SkillTreeComponent.h"

USkillComponent::USkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	HitTags.AddTag(FGameplayTag::RequestGameplayTag("State.Hit"));
	HitTags.AddTag(FGameplayTag::RequestGameplayTag("State.Knockback"));
	HitTags.AddTag(FGameplayTag::RequestGameplayTag("State.Knockdown"));

	DeadTags.AddTag(FGameplayTag::RequestGameplayTag("State.Dead"));
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
	CheckNull(DataTable);
	DataTable->GetAllRows<FSkillData>("", SkillDatas);
	for (int32 i = 0; i < int32(EQuickSlotPosition::Max); ++i)
		QuickSlotData[i] = -1;

	CheckTrue(GetOwner()->GetLocalRole() != ROLE_Authority);

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
}

void USkillComponent::UseSkill(int32 InSkillID)
{
	for (auto& i : ActivatableAbilities.Items)
		if (i.InputID == InSkillID)
		{
			//UEnum* temp = FindObject<UEnum>(ANY_PACKAGE, TEXT("EGameplayAbility_Mage"), true);
			//if (temp)CLog::Print(temp->GetNameStringByIndex(InSkillID));
			TryActivateAbility(i.Handle);
		}
}

void USkillComponent::UseQuickSlotSkill(int32 InQuickSlotIndex)
{
	CheckFalse(InQuickSlotIndex < int32(EQuickSlotPosition::Max));
	UseSkill(QuickSlotData[InQuickSlotIndex]);
}

void USkillComponent::ChangeQuickSlotData(int32 InQuickSlotIndex, int32 InSkillID)
{
	CheckFalse(InQuickSlotIndex < int32(EQuickSlotPosition::Max));
	QuickSlotData[InQuickSlotIndex] = InSkillID;

	// Is Empty
	if (QuickSlotData[InQuickSlotIndex]<0)
	{
		OnQuickSlotDataChanged.Broadcast(InQuickSlotIndex, FSkillData());
		return;
	}

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
			ChangeQuickSlotData(i, -1);
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

bool USkillComponent::CanUse() const
{
	return !HasAnyMatchingGameplayTags(BlockUseTags);
}

bool USkillComponent::CanMove() const
{
	return !HasAnyMatchingGameplayTags(BlockMoveTags);
}

bool USkillComponent::IsDead()const
{
	return HasAnyMatchingGameplayTags(DeadTags);
}

bool USkillComponent::IsHit()const
{
	return HasAnyMatchingGameplayTags(HitTags);
}

void USkillComponent::SaveData(USaveGameData* SaveData)
{
	// reset Datas
	//SaveData->PlayerData.SkillPoints;
	SaveData->PlayerData.SlotSkills.Empty();

	// save Datas
	for (int32 i : QuickSlotData)
		SaveData->PlayerData.SlotSkills.Add(i);
}

void USkillComponent::LoadData(USaveGameData* const ReadData)
{
	// load Datas
	for (int32 i = 0; i < ReadData->PlayerData.SlotSkills.Num(); ++i)
		ChangeQuickSlotData(i, ReadData->PlayerData.SlotSkills[i]);

	bLoad = 1;
}

int32 USkillComponent::GetSkillLevel(int32 InSkillID) const
{
	//if(!SkillState.Items.IsValidIndex(InSkillID))return 0;
	//return (int32)SkillState.Items[InSkillID].SkillState - (int32)ESkillTreeSkillState::Unlocked;
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
