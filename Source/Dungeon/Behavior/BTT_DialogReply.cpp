#include "Behavior/BTT_DialogReply.h"
#include "Global.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

//#include "Widgets/UW_Dialog.h"

UBTT_DialogReply::UBTT_DialogReply()
{
	NodeName = "Reply";

	//DialogWidget.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_DialogReply, DialogWidget), UUW_Dialog::StaticClass());
	ReplyIndex.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_DialogReply, ReplyIndex));
	Point.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_DialogReply, Point));
}

EBTNodeResult::Type UBTT_DialogReply::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	//UUW_Dialog* widget = nullptr;
	//widget = Cast<UUW_Dialog>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(DialogWidget.SelectedKeyName));

	//if (widget)
	//{
	//	widget->OnReplyFinished.AddDynamic(this, &UBTT_DialogReply::OnReplyFinished);
	//	TArray<FText> arr;
	//	for (auto i : Replies)arr.Add(i.Reply);
	//	widget->Reply(arr);
	//}

	return EBTNodeResult::InProgress;
}

void UBTT_DialogReply::OnReplyFinished(int32 InReplyIndex, UBehaviorTreeComponent* OwnerComp)
{
	//UUW_Dialog* widget = nullptr;
	//widget = Cast<UUW_Dialog>(OwnerComp->GetBlackboardComponent()->GetValueAsObject(DialogWidget.SelectedKeyName));
	//if (widget)	widget->OnReplyFinished.Remove(this, "OnReplyFinished");
	//OwnerComp->GetBlackboardComponent()->SetValueAsInt(ReplyIndex.SelectedKeyName, InReplyIndex);
	//OwnerComp->GetBlackboardComponent()->SetValueAsInt(Point.SelectedKeyName, Replies[InReplyIndex].NextPoint);
	//FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}