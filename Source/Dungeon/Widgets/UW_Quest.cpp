#include "Widgets/UW_Quest.h"
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
	Objective->OnQuestCountChanged.BindUFunction(this, "UpdateObject");

	for (auto i : Objective->GetQuestConditions())
	{
		UTextBlock* temp = NewObject<UTextBlock>(UTextBlock::StaticClass());
		Vertical->AddChild(temp);
	}
	UpdateObject();
}

void UUW_QuestEntry::OnExpandButtonClicked()
{
	UTreeView* temp = Cast<UTreeView>(GetOwningListView());
	CheckNull(temp);
	temp->SetItemExpansion(Objective, !IsListItemExpanded());
}

void UUW_QuestEntry::UpdateObject()
{
	//update main category
	{
		FString str = "";

		switch (Objective->GetState())
		{
		case EQuestObjectiveState::Main:str = TEXT("목표"); break;
		case EQuestObjectiveState::Additive:str = TEXT("추가목표"); break;
		case EQuestObjectiveState::Max:break;
		default:break;
		}

		//TODO:: count success
		int32 success = 0;
		str += FString::Printf(TEXT("(%i/%i)"), success, Objective->GetQuestConditions().Num());
		MainCategory->SetText(FText::FromString(str));
	}

	//update child
	int arrIdx = 0;
	for (int32 i = 0; i < Vertical->GetChildrenCount(); ++i)
	{
		UTextBlock* txt = Cast<UTextBlock>(Vertical->GetChildAt(i));
		if (!txt)continue;

		if (!Objective->GetQuestCounts().IsValidIndex(arrIdx))continue;
		const int32* count = Objective->GetQuestCounts()[arrIdx];

		if (!Objective->GetQuestConditions().IsValidIndex(arrIdx))continue;
		const FQuestCondition* condition = Objective->GetQuestConditions()[arrIdx++];
		FString result = condition->GetSummary();
		EQuestConditionType type = condition->GetType();

		if (type == EQuestConditionType::DestroyingEnemy)
			result += FString::Printf(TEXT("(%i/%i)"), *count, condition->GetTargetEnemyCount());
		else if (type == EQuestConditionType::Survive)
			result += FString::Printf(TEXT("(%i초)"), *count);
		else if (type == EQuestConditionType::Interact)
			result += FString::Printf(TEXT("(%i/%i)"), *count, condition->GetTargetInteractCount());

		txt->SetText(FText::FromString(result));
	}
}

/////////////////////////
// UUW_Quest
/////////////////////////

void UUW_Quest::SetQuest(AQuest* NewQuest)
{
	Quest = NewQuest;

	ObjectTreeView->ClearListItems();
	if (!Quest)
	{
		// TODO:: Set default
		return;
	}

	ObjectTreeView->AddItem(Quest->GetMainQuestOjbective());
	ObjectTreeView->AddItem(Quest->GetAdditiveQuestOjbective());
}