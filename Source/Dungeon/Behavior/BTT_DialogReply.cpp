#include "Behavior/BTT_DialogReply.h"
#include "Global.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "DungeonPlayerController.h"
#include "Behavior/PlayerDialogDatas.h"

UBTT_DialogReply::UBTT_DialogReply()
{
	NodeName = "Reply";

	PlayerDatas.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_DialogReply, PlayerDatas), UPlayerDialogDatas::StaticClass());
	InteractingPlayer.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_DialogReply, InteractingPlayer), ADungeonPlayerController::StaticClass());
}

EBTNodeResult::Type UBTT_DialogReply::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// get interacting player controller
	ADungeonPlayerController* player = nullptr;
	player = Cast<ADungeonPlayerController>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(InteractingPlayer.SelectedKeyName));
	if (!player)return EBTNodeResult::Failed;

	// setting Replies
	TArray<FText> arr;
	for (auto i : Replies)arr.Add(i.Reply);
	player->Client_DialogReply(arr);

	// release interacting player
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(InteractingPlayer.SelectedKeyName, nullptr);

	return EBTNodeResult::Succeeded;
}

FString UBTT_DialogReply::GetStaticDescription() const
{
	Super::GetStaticDescription();
	FString result = "Replies...";
	for (int32 i = 0; i < Replies.Num(); ++i)
		result += FString::Printf(TEXT("\nReply:%s, NextPoint:%d"), *Replies[i].Reply.ToString(), Replies[i].NextPoint);
	return result;
}
