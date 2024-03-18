#include "Behavior/BTT_Approach.h"
#include "Global.h"

#include "Characters/EnemyAIController.h"
#include "Components/BehaviorComponent.h"
#include "Components/SkillComponent.h"

UBTT_Approach::UBTT_Approach()
{
	NodeName = "Approach";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_Approach::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* controller = Cast<AAIController>(OwnerComp.GetOwner());
	UBehaviorComponent* behavior = CHelpers::GetComponent<UBehaviorComponent>(controller);
	behavior->TaskStart();

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

void UBTT_Approach::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* controller = Cast<AAIController>(OwnerComp.GetOwner());
	UBehaviorComponent* behavior = CHelpers::GetComponent<UBehaviorComponent>(controller);

	// time over?
	float elapsed = GetWorld()->GetTimeSeconds() - behavior->GetTaskStartTime();
	if(elapsed > Duration)
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

EBTNodeResult::Type UBTT_Approach::PerformMoveTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// save origin AcceptableRadius
	float origin = AcceptableRadius;

	// if RangeType is skill?
	// change AcceptableRadius to SkillRange
	// execute super::movetotask
	if (RangeType == ESuccessRange::Skill)
	{
		AAIController* controller = Cast<AAIController>(OwnerComp.GetOwner());
		USkillComponent* skill = CHelpers::GetComponent<USkillComponent>(controller->GetPawn());
		
		float range = 0;
		if (skill->GetSkillRange(SkillIndex, range))
			AcceptableRadius = range;		
	}
	EBTNodeResult::Type NodeResult = Super::PerformMoveTask(OwnerComp, NodeMemory);

	// undo change
	if (RangeType == ESuccessRange::Skill)
		AcceptableRadius = origin;

	return NodeResult;
}
