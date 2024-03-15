#include "Behavior/BTT_Approach.h"
#include "Global.h"

UBTT_Approach::UBTT_Approach()
{
	NodeName = "Approach";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_Approach::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	StartTime = GetWorld()->GetTimeSeconds();
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

void UBTT_Approach::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// time over?
	float elapsed = GetWorld()->GetTimeSeconds() - StartTime;
	if(elapsed> Duration)
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}