#include "Components/SkillTreeComponent.h"
#include "Global.h"

#include "SaveManager.h"
#include "Characters/PlayerCharacter.h"
#include "DungeonPlayerController.h"
#include "Components/SkillComponent.h"
#include "Widgets/UW_SkillTree.h"
#include "Abilities/GABase.h"

USkillTreeComponent::USkillTreeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(1);
}

void USkillTreeComponent::BeginPlay()
{
	Super::BeginPlay();

}

void USkillTreeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!Widget)
		for (int32 i = 0; i < SkillStates.Items.Num(); ++i)
			if (SkillStates.Items[i].SkillState == ESkillTreeSkillState::Locked)CLog::Print(FString::Printf(TEXT("%i : Lock"), i), -1, 0, FColor::Purple);
			else if (SkillStates.Items[i].SkillState == ESkillTreeSkillState::Unlocked)CLog::Print(FString::Printf(TEXT("%i : Unlock"), i), -1, 0, FColor::Purple);
			else CLog::Print(FString::Printf(TEXT("%i : Else"), i), -1, 0, FColor::Purple);
}

void USkillTreeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(USkillTreeComponent, SkillStates, COND_None);
}

void USkillTreeComponent::MakeTreeDatas(const TArray<const FSkillData*> InSkillDatas)
{
	//for (auto i : InSkillDatas)
	//	SkillWidgetStates.Add(i->BaseState);

	CheckFalse(GetOwner()->GetLocalRole() == ROLE_Authority);
	CLog::Print("IN", -1, 10, FColor::Black);

	SkillTreeDatas.Init(TArray<int32>(), InSkillDatas.Num());
	TMap<FVector2D, int32> locMap;
	for (auto i : InSkillDatas)locMap.Add(i->PannelPosition, i->SkillClass.GetDefaultObject()->GetSkillID());
	for (auto i : InSkillDatas)
	{
		if (!locMap.Contains(i->ParentPosition))continue;
		int32 skillID = locMap[i->ParentPosition];
		if (!SkillTreeDatas.IsValidIndex(skillID))
		{
			CLog::Print(FString::Printf(TEXT("USkillTreeComponent::MakeTreeDatas, %s IsNotValidIndex"), *i->SkillClass->GetName()), -1, 10, FColor::Red);
			continue;
		}
		SkillTreeDatas[skillID].Add(i->SkillClass.GetDefaultObject()->GetSkillID());
	}

	for (auto i : InSkillDatas)
	{
		FSkillStateItem temp;
		temp.SkillState = i->BaseState;
		SkillStates.Items.Add(temp);
	}
}

void USkillTreeComponent::OnRep_SkillStates()
{
	CLog::Print(SkillWidgetStates.Num(), -1, 10, FColor::Green);

	for (int32 i = 0; i < SkillWidgetStates.Num(); ++i)
	{
		if (!SkillStates.Items.IsValidIndex(i))continue;
		if (SkillWidgetStates[i] == SkillStates.Items[i].SkillState)continue;
		SkillWidgetStates[i] = SkillStates.Items[i].SkillState;
		if(Widget)Widget->UpdateButtonState(i, SkillWidgetStates[i]);
	}

	for (int32 i = SkillWidgetStates.Num(); i < SkillStates.Items.Num(); ++i)
	{
		SkillWidgetStates.Add(SkillStates.Items[i].SkillState);
		if(Widget)Widget->UpdateButtonState(i, SkillWidgetStates[i]);
	}
}

void USkillTreeComponent::Server_UpdateSkillState_Implementation(int32 InSkillID, ESkillTreeSkillState InSkillState)
{
	//this is for loading no need to change children state
	CheckFalse(SkillStates.Items.IsValidIndex(InSkillID));
	if (SkillComp->GetSkillDatas().IsValidIndex(InSkillID) && InSkillState < SkillComp->GetSkillDatas()[InSkillID]->BaseState)
		InSkillState = SkillComp->GetSkillDatas()[InSkillID]->BaseState;

	SkillStates.Items[InSkillID].SkillState = InSkillState;
}

void USkillTreeComponent::Server_LevelUpSkillState_Implementation(int32 InSkillID)
{
	// TODO::add point condition

	CheckFalse(SkillStates.Items.IsValidIndex(InSkillID));

	int32 temp = int32(SkillStates.Items[InSkillID].SkillState) + 1;
	temp %= int32(ESkillTreeSkillState::Max);

	SkillStates.Items[InSkillID].SkillState = ESkillTreeSkillState(temp);

	// update children state to unlock
	if (SkillStates.Items[InSkillID].SkillState == ESkillTreeSkillState::Acquired_1 && SkillComp->GetSkillDatas().IsValidIndex(InSkillID))
		for (auto i : SkillTreeDatas[InSkillID])
			Server_LevelUpSkillState(i);

	OnRep_SkillStates();
}

void USkillTreeComponent::Init(const TArray<const FSkillData*> Array, USkillComponent* InSkillComp, TFunction<void(int32, int32)> OnPopupClicked)
{
	SkillComp = InSkillComp;
	MakeTreeDatas(SkillComp->GetSkillDatas());

	for (auto i : Array)
	{
		if (i->ParentPosition == FVector2D(-1, -1))
		{
			RootDatas.Add(i->PannelPosition);
			continue;
		}
		TreeDatas.FindOrAdd(i->ParentPosition).Add(i->PannelPosition);
	}

	ACharacter* owner = Cast<ACharacter>(GetOwner());
	CheckFalse(owner && owner->IsLocallyControlled());
	if (WidgetClass && !Widget)
	{
		APlayerCharacter* character = Cast<APlayerCharacter>(GetOwner());
		CheckNull(character);
		ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(character->GetController());
		CheckNull(controller);

		Widget = CreateWidget<UUW_SkillTree, ADungeonPlayerController>(controller, WidgetClass);
		Widget->AddToViewport();
		Widget->Init(Array, this, InSkillComp, OnPopupClicked);
		for (int32 i = 0; i < SkillStates.Items.Num(); ++i)
			Widget->UpdateButtonState(i, SkillStates.Items[i].SkillState);
	}	
}

void USkillTreeComponent::AddPoints()
{

}

void USkillTreeComponent::LevelUp(int32 InSkillID)
{
	Server_LevelUpSkillState(InSkillID);
}

bool USkillTreeComponent::IsWidgetVisible()
{
	if (!Widget)return 0;
	return Widget->IsVisible();
}

void USkillTreeComponent::OnWidget()
{
	CheckNull(Widget);
	Widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void USkillTreeComponent::OffWidget()
{
	CheckNull(Widget);
	Widget->SetVisibility(ESlateVisibility::Collapsed);
}

void USkillTreeComponent::SaveData(USaveGameData* SaveData)
{
	SaveData->PlayerData.SkillState.Empty();

	for (auto i : SkillStates.Items)
		SaveData->PlayerData.SkillState.Add(i.SkillState);
}

void USkillTreeComponent::LoadData(USaveGameData* const ReadData)
{
	for (int32 i = 0; i < ReadData->PlayerData.SkillState.Num(); ++i)
		Server_UpdateSkillState(i, SkillStates.Items[i].SkillState);

	if (GetOwner()->GetLocalRole() == ROLE_Authority)
		OnRep_SkillStates();
}
