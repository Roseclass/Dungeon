#include "Components/SkillComponent.h"
#include "Global.h"

#include "SaveManager.h"
#include "Characters/DungeonCharacterBase.h"
#include "Characters/PlayerCharacter.h"
#include "Objects/SkillActor.h"

USkillComponent::USkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(1);
	QuickSlotSkillActors.Init(nullptr, 6);
}

void USkillComponent::BeginPlay()
{
	Super::BeginPlay();
	if (bAutoGenerate && GetOwner()->HasAuthority())SpawnSkillActors();
}

void USkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USkillComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicated 변수를 여기에 추가
	DOREPLIFETIME_CONDITION(USkillComponent, SkillActors, COND_None);
}

void USkillComponent::OnRep_SkillActors()
{
	// check replication sync
	int32 cnt = 0;
	for (auto i : SkillActors)
		if (i)++cnt;
	if (SkillActors.Num() == cnt)
	{
		for (auto i : SkillActors)
		{
			i->SetOwnerCharacter(Cast<ADungeonCharacterBase>(GetOwner()));
			for (auto j : SkillActors)
			{
				if (i->GetSkillData()->PannelPosition == j->GetSkillData()->ParentPosition)
				{
					i->AddChild(j);
					j->SetParent(i);
				}
			}
		}

		APlayerCharacter* const owner = Cast<APlayerCharacter>(GetOwner());
		CheckNull(owner);
		owner->InitClientWidget();

		// load widget datas
		USaveGameData* saveGameData = Cast<USaveGameData>(UGameplayStatics::LoadGameFromSlot(USaveManager::GetCurrentSaveSlot(), 0));
		if (!saveGameData) { CLog::Print("saveGameData is nullptr"); return; }
		owner->OnAfterLoad_ClientWidget(saveGameData);

		bLoad = 1;
	}
}

void USkillComponent::SetCurrent(ASkillActor* NewCurrent)
{
	CurrentSkill = NewCurrent;
}

void USkillComponent::SpawnSkillActors()
{
	if (!SkillActors.IsEmpty())
	{
		CLog::Print(GetOwner()->GetName() + "wtf", -1, 10, FColor::Yellow);
		return;
	}
	TArray<ASkillActor*> Array;
	for (auto i : SkillActorClasses)
		Array.Add(GetWorld()->SpawnActor<ASkillActor>(i));

	SkillActors = Array;

	for (auto i : SkillActors)
	{
		i->SetOwner(GetOwner());
		i->OnSkillUsing.AddUFunction(this, "SetCurrent");
	}

	OnRep_SkillActors();
}

void USkillComponent::UseSkill(int32 Idx)
{
	CheckFalse(SkillActors.IsValidIndex(Idx));
	CheckNull(SkillActors[Idx]);
	CurrentSkill = SkillActors[Idx];
	CurrentSkill->Client_Use();
}

void USkillComponent::UseQuickSlotSkill(int32 Idx)
{
	CheckFalse(QuickSlotSkillActors.IsValidIndex(Idx));
	CheckNull(QuickSlotSkillActors[Idx]);
	CurrentSkill = QuickSlotSkillActors[Idx];
	QuickSlotSkillActors[Idx]->Client_Use();
}

void USkillComponent::SpawnProjectile()
{
	CheckNull(CurrentSkill);
	CurrentSkill->SpawnProjectile();
}

void USkillComponent::SpawnWarningSign(int32 InIndex)
{
	CheckNull(CurrentSkill);
	CurrentSkill->SpawnWarningSign(InIndex);
}

void USkillComponent::ChangeQuickSlotData(int32 Index, ASkillActor* InSkillActor)
{
	CheckFalse(QuickSlotSkillActors.IsValidIndex(Index));

	if (!InSkillActor)
	{
		OnQuickSlotDataChanged.Broadcast(Index, InSkillActor);
		QuickSlotSkillActors[Index] = nullptr;
		return;
	}

	if (QuickSlotSkillActors.Contains(InSkillActor))
	{
		for (int32 i = 0; i < QuickSlotSkillActors.Num(); ++i)
			if (QuickSlotSkillActors[i] == InSkillActor)
				ChangeQuickSlotData(i, nullptr);
	}

	OnQuickSlotDataChanged.Broadcast(Index, InSkillActor);
	QuickSlotSkillActors[Index] = InSkillActor;
	SetCoolDown(InSkillActor);
}

void USkillComponent::SetCoolDown(ASkillActor* InSkillActor)
{
	CheckNull(InSkillActor);
	int32 idx = QuickSlotSkillActors.Find(InSkillActor);
	CheckTrue(idx == INDEX_NONE);
	OnQuickSlotCoolDown.Broadcast(idx, QuickSlotSkillActors[idx]->GetRemainingCoolDown());
}

bool USkillComponent::GetQuickSlotCoolDown(int32 Index,float& Result)
{
	if (!QuickSlotSkillActors.IsValidIndex(Index) || !QuickSlotSkillActors[Index])return 0;
	Result = QuickSlotSkillActors[Index]->GetCoolDown();
	return 1;
}

bool USkillComponent::IsQuickSlotCoolDown(int32 Index)
{
	if (!QuickSlotSkillActors.IsValidIndex(Index))return 1;
	if (!QuickSlotSkillActors[Index])return 1;
	return QuickSlotSkillActors[Index]->IsCoolDown();
}

float USkillComponent::GetQuickSlotManaCost(int32 Index)
{
	if (!QuickSlotSkillActors.IsValidIndex(Index))return 1e9;
	if (!QuickSlotSkillActors[Index])return 1e9;
	return QuickSlotSkillActors[Index]->GetManaCost();
}

bool USkillComponent::GetSkillRange(int32 InIndex, float& Range)
{
	if (!SkillActors.IsValidIndex(InIndex))return 0;
	if (!SkillActors[InIndex])return 0;
	Range = SkillActors[InIndex]->GetSkillData()->Range;
	return 1;
}

bool USkillComponent::GetQuickSlotSkillRange(int32 InIndex, float& Range)
{
	if (!QuickSlotSkillActors.IsValidIndex(InIndex))return 0;
	if (!QuickSlotSkillActors[InIndex])return 0;
	Range = QuickSlotSkillActors[InIndex]->GetSkillData()->Range;
	return 1;
}

void USkillComponent::SaveData(USaveGameData* SaveData)
{
	// reset Datas
	SaveData->PlayerData.SlotSkills.Empty();

	for (auto i : QuickSlotSkillActors)
		if (i)SaveData->PlayerData.SlotSkills.Add(i->GetClass());
		else SaveData->PlayerData.SlotSkills.Add(nullptr);
}

void USkillComponent::LoadData(USaveGameData* const ReadData)
{
	for (int32 i = 0; i < ReadData->PlayerData.SlotSkills.Num(); ++i)
	{
		for(auto skill : SkillActors)
			if(skill->GetClass() == ReadData->PlayerData.SlotSkills[i])
				ChangeQuickSlotData(i, skill);
	}

	bLoad = 1;
}
