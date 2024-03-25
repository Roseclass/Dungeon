#include "Behavior/BTT_DialogSpeak.h"
#include "Global.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "DungeonPlayerController.h"
#include "Behavior/PlayerDialogDatas.h"

UBTT_DialogSpeak::UBTT_DialogSpeak()
{
	NodeName = "Speak";

	PlayerDatas.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_DialogSpeak, PlayerDatas), UPlayerDialogDatas::StaticClass());
	InteractingPlayer.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_DialogSpeak, InteractingPlayer), ADungeonPlayerController::StaticClass());
}

EBTNodeResult::Type UBTT_DialogSpeak::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// get interacting player controller
	ADungeonPlayerController* player = nullptr;
	player = Cast<ADungeonPlayerController>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(InteractingPlayer.SelectedKeyName));
	if (!player)return EBTNodeResult::Failed;

	// change dialog text
	player->Client_DialogSpeak(Text);

	// save dialog point
	UPlayerDialogDatas* datas = Cast<UPlayerDialogDatas>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(PlayerDatas.SelectedKeyName));
	if (!datas)
	{
		CLog::Print("UBTT_DialogSpeak datas is nullptr", -1, 10, FColor::Red);
		return EBTNodeResult::Failed;
	}
	int32& point = datas->PointMap.FindOrAdd(player);
	point = NextPoint;
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(PlayerDatas.SelectedKeyName, datas);

	// release interacting player
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(InteractingPlayer.SelectedKeyName, nullptr);

	
	return EBTNodeResult::Succeeded;
}

FString UBTT_DialogSpeak::GetStaticDescription() const
{
	Super::GetStaticDescription();
	FString result = FString::Printf(TEXT("NextPoint : %d"), NextPoint);
	result += FString::Printf(TEXT("\nText : %s"), *Text.ToString());
	return result;
}
