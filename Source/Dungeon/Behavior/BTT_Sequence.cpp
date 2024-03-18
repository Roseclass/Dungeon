#include "Behavior/BTT_Sequence.h"
#include "Global.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Characters/EnemyAIController.h"
#include "Characters/Enemy.h"
#include "Components/StateComponent.h"

UBTT_Sequence::UBTT_Sequence()
{
	NodeName = "Sequence";

	Sequence.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_Sequence, Sequence));
}

EBTNodeResult::Type UBTT_Sequence::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)return EBTNodeResult::Failed;

	AEnemyAIController* controller = Cast<AEnemyAIController>(OwnerComp.GetOwner());
	if (!controller)return EBTNodeResult::Failed;

	AEnemy* aiPawn = Cast<AEnemy>(controller->GetPawn());
	if (!aiPawn)return EBTNodeResult::Failed;

	aiPawn->PlaySequence(BlackboardComp->GetValueAsInt(Sequence.SelectedKeyName));

	return EBTNodeResult::InProgress;
}

void UBTT_Sequence::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// is enemy's state IDLE?
	// that means skill sequence's end

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	AAIController* controller = Cast<AAIController>(OwnerComp.GetOwner());
	AEnemy* aiPawn = Cast<AEnemy>(controller->GetPawn());

	UStateComponent* state = CHelpers::GetComponent<UStateComponent>(aiPawn);

	if (state->IsIdleMode())
	{
		int32 sequence = BlackboardComp->GetValueAsInt(Sequence.SelectedKeyName);
		BlackboardComp->SetValueAsInt(Sequence.SelectedKeyName,++sequence);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
