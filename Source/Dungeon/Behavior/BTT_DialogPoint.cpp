#include "Behavior/BTT_DialogPoint.h"
#include "Global.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "DungeonPlayerController.h"
#include "Behavior/PlayerDialogDatas.h"

UBTT_DialogPoint::UBTT_DialogPoint()
{
	NodeName = "LoadPoint";

	PlayerDatas.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_DialogPoint, PlayerDatas), UPlayerDialogDatas::StaticClass());
	InteractingPlayer.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_DialogPoint, InteractingPlayer), ADungeonPlayerController::StaticClass());
	Point.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_DialogPoint, Point));
}

EBTNodeResult::Type UBTT_DialogPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// get interacting player controller
	ADungeonPlayerController* player = nullptr;
	player = Cast<ADungeonPlayerController>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(InteractingPlayer.SelectedKeyName));
	if (!player)return EBTNodeResult::Failed;

	// load dialog point
	UPlayerDialogDatas* datas = Cast<UPlayerDialogDatas>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(PlayerDatas.SelectedKeyName));
	if (!datas)
	{
		CLog::Print("UBTT_DialogPoint datas is nullptr", -1, 10, FColor::Red);
		return EBTNodeResult::Failed;
	}
	int32 point = datas->PointMap.FindOrAdd(player);
	OwnerComp.GetBlackboardComponent()->SetValueAsInt(Point.SelectedKeyName, point);

	return EBTNodeResult::Succeeded;
}
