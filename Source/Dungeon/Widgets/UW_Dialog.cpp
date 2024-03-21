#include "Widgets/UW_Dialog.h"
#include "Global.h"
#include "Components/ListView.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Widgets/UW_DialogEntry.h"

FReply UUW_Dialog::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) && DialogState == EDialogState::Speak)
	{
		if (OnSpeakFinished.IsBound())
			OnSpeakFinished.Broadcast(BTComponent);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void UUW_Dialog::OnReplyClicked(UDialogReplyObject* ClickedObject)
{
	int32 numb = RepliesListView->GetIndexForItem(ClickedObject);
	if (OnReplyFinished.IsBound())
		OnReplyFinished.Broadcast(numb, BTComponent);
}

void UUW_Dialog::OnQuestClicked(UDialogReplyObject* ClickedObject)
{
	if (OnQuestReplyFinished.IsBound())
		OnQuestReplyFinished.Broadcast(BTComponent, ClickedObject);
}

void UUW_Dialog::OnPointClicked(UDialogReplyObject* ClickedObject)
{
	if (OnPointReplyFinished.IsBound())
		OnPointReplyFinished.Broadcast(BTComponent);
}

void UUW_Dialog::HideReplySection()
{
	ReplyBorder->SetVisibility(ESlateVisibility::Collapsed);
}

void UUW_Dialog::RevealReplySection()
{
	ReplyBorder->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UUW_Dialog::HideSpeakSection()
{
	NPCPortrait->SetVisibility(ESlateVisibility::Collapsed);
	NPCName->SetVisibility(ESlateVisibility::Collapsed);
	SpeakBorder->SetVisibility(ESlateVisibility::Collapsed);
}

void UUW_Dialog::RevealSpeakSection()
{
	NPCPortrait->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	NPCName->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	SpeakBorder->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UUW_Dialog::Init(UObject* Portrait, FText Name)
{
	NPCPortrait->SetBrushResourceObject(Portrait);
	NPCName->SetText(Name);
}

void UUW_Dialog::SetBTComponent(UBehaviorTreeComponent* InComponent)
{
	BTComponent = InComponent;
}

void UUW_Dialog::SetDialogState(EDialogState InState)
{
	DialogState = InState;
	switch (DialogState)
	{
	case EDialogState::Speak:HideReplySection(); RevealSpeakSection(); break;
	case EDialogState::Reply:RevealReplySection(); HideSpeakSection(); break;
	default:break;
	}
}

void UUW_Dialog::Speak(FText InText)
{
	SpeakText->SetText(InText);
	SetDialogState(EDialogState::Speak);
}

void UUW_Dialog::Reply(TArray<FText> Replies)
{
	RepliesListView->ClearListItems();
	for (auto i : Replies)
	{
		UDialogReplyObject* obj = NewObject<UDialogReplyObject>(this);
		obj->SetText(i);
		RepliesListView->AddItem(obj);
		//obj->OnClicked.AddUFunction(this, "OnReplyClicked");
	}
	SetDialogState(EDialogState::Reply);
}

void UUW_Dialog::Quest(const TArray<FQuestTreeData>& Quests)
{
	RepliesListView->ClearListItems();
	for (auto i : Quests)
	{
		UDialogReplyObject* obj = NewObject<UDialogReplyObject>(this);
		obj->SetText(i.QuestName);
		obj->SetData(i);
		RepliesListView->AddItem(obj);
		//obj->OnClicked.AddDynamic(this, &UUW_Dialog::OnQuestClicked);
	}
	SetDialogState(EDialogState::Reply);
}

void UUW_Dialog::Point(FText InText, bool Clear)
{
	if (Clear)RepliesListView->ClearListItems();
	UDialogReplyObject* obj = NewObject<UDialogReplyObject>(this);
	obj->SetText(InText);
	RepliesListView->AddItem(obj);
	obj->OnClicked.AddUFunction(this, "OnPointClicked");
	SetDialogState(EDialogState::Reply);
}

void UUW_Dialog::Exit()
{
	if (OnExit.IsBound())
		OnExit.Broadcast();
}
