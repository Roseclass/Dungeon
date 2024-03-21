#include "Behavior/BTT_DialogPoint.h"
#include "Global.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_DialogPoint::UBTT_DialogPoint()
{
	NodeName = "Point";

	Point.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_DialogPoint, Point));
}

EBTNodeResult::Type UBTT_DialogPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)return EBTNodeResult::Failed;

	BlackboardComp->SetValueAsInt(Point.SelectedKeyName, NextPoint);

	return EBTNodeResult::Succeeded;
}
