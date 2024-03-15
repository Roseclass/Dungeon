#include "Behavior/BTS_OrcElite.h"
#include "Global.h"

#include "Characters/DungeonCharacter.h"
#include "Characters/Enemy.h"
#include "Characters/EnemyAIController.h"
#include "Components/StateComponent.h"
#include "Components/BehaviorComponent.h"

UBTS_OrcElite::UBTS_OrcElite()
{
	NodeName = "OrcElite";
}

void UBTS_OrcElite::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* controller = Cast<AAIController>(OwnerComp.GetOwner());
	UBehaviorComponent* behavior = CHelpers::GetComponent<UBehaviorComponent>(controller);

	AEnemy* aiPawn = Cast<AEnemy>(controller->GetPawn());
	UStateComponent* state = CHelpers::GetComponent<UStateComponent>(aiPawn);

	CheckTrue(state->IsDeadMode());	

	if (state->IsHittedMode())		
	{
		behavior->SetHittedMode();	
		return;
	}

	ADungeonCharacter* target = behavior->GetTarget();

	if (target == nullptr)
	{
		CLog::Print("target null", 222, 10, FColor::Black);
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

	float distance = aiPawn->GetDistanceTo(target);

	CLog::Print("target on", 222, 10, FColor::Black);
}