#include "Behavior/BTT_ChangePhase.h"
#include "Global.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_ChangePhase::UBTT_ChangePhase()
{
	NodeName = "ChangePhase";
	bNotifyTick = true;

	Phase.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_ChangePhase, Phase));
}

EBTNodeResult::Type UBTT_ChangePhase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)return EBTNodeResult::Failed;
	BlackboardComp->SetValueAsInt(Phase.SelectedKeyName, NextPhase);

	return EBTNodeResult::Succeeded;
}
