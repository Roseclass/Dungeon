﻿#include "Widgets/UW_Quest.h"
#include "Global.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/TreeView.h"
#include "Components/VerticalBox.h"

#include "Objects/Quest.h"

/////////////////////////
// UUW_QuestEntry
/////////////////////////

void UUW_QuestEntry::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Expand->OnClicked.AddDynamic(this, &UUW_QuestEntry::OnExpandButtonClicked);
}

void UUW_QuestEntry::NativeOnItemExpansionChanged(bool bIsExpanded)
{
	for (int32 i = 1; i < Vertical->GetChildrenCount(); ++i)
		Vertical->GetChildAt(i)->SetVisibility(bIsExpanded ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UUW_QuestEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UQuest_Objective* obj = Cast<UQuest_Objective>(ListItemObject);
	CheckNull(obj);
	Objective = obj;
	for (auto i : Objective->GetQuestConditions())
	{
		UTextBlock* temp = NewObject<UTextBlock>(UTextBlock::StaticClass());
		FString txt = i.Summary + "(" + FString::FromInt(i.CurrentCount) + "/" + FString::FromInt(i.TargetCount) + ")";

		temp->SetText(FText::FromString(txt));
		Vertical->AddChild(temp);
	}

	FString str = "";

	switch (Objective->GetState())
	{		
	case EQuestObjectiveState::Main:str = TEXT("목표"); break;
	case EQuestObjectiveState::Additive:str = TEXT("추가목표"); break;
	case EQuestObjectiveState::Max:break;
	default:break;
	}

	str += "(0/" + FString::FromInt(Objective->GetQuestConditions().Num()) + ")";

	Object->SetText(FText::FromString(str));
}

void UUW_QuestEntry::OnExpandButtonClicked()
{
	UTreeView* temp = Cast<UTreeView>(GetOwningListView());
	CheckNull(temp);
	temp->SetItemExpansion(Objective, !IsListItemExpanded());
}

/////////////////////////
// UUW_Quest
/////////////////////////

void UUW_Quest::SetQuest(AQuest* NewQuest)
{
	Quest = NewQuest;
	ObjectTreeView->ClearListItems();
	ObjectTreeView->AddItem(Quest->GetMainQuestOjbective());
	ObjectTreeView->AddItem(Quest->GetAdditiveQuestOjbective());
}