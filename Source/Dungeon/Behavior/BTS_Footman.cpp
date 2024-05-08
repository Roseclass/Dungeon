#include "Behavior/BTS_Footman.h"
#include "Global.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Characters/PlayerCharacter.h"
#include "Characters/Enemy.h"
#include "Characters/EnemyAIController.h"
#include "Components/StateComponent.h"
#include "Components/StatusComponent.h"
#include "Components/BehaviorComponent.h"

UBTS_Footman::UBTS_Footman()
{
	NodeName = "Footman";

	PerceptedPlayerObject.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_Footman, PerceptedPlayerObject), UBlackBoardPlayerArrayObject::StaticClass());
	Target.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_Footman, Target), APlayerCharacter::StaticClass());
}

void UBTS_Footman::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)return;

	AAIController* controller = Cast<AAIController>(OwnerComp.GetOwner());
	UBehaviorComponent* behavior = CHelpers::GetComponent<UBehaviorComponent>(controller);

	AEnemy* aiPawn = Cast<AEnemy>(controller->GetPawn());
	UStateComponent* state = CHelpers::GetComponent<UStateComponent>(aiPawn);
	UStatusComponent* status = CHelpers::GetComponent<UStatusComponent>(aiPawn);

	CheckTrue(state->IsDeadMode());
	CheckTrue(state->IsSequenceMode());

	if (state->IsHitMode())
	{
		behavior->SetHitMode();
		return;
	}

	APlayerCharacter* target = Cast<APlayerCharacter>(BlackboardComp->GetValueAsObject(Target.SelectedKeyName));

	if (target == nullptr)
	{
		behavior->SetWaitMode();
		return;
	}
	else
	{
		UStateComponent* targetState = CHelpers::GetComponent<UStateComponent>(target);
		if (targetState->IsDeadMode())
		{
			behavior->SetWaitMode();
			return;
		}
	}
}
