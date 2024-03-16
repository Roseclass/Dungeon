#include "Behavior/BTS_OrcElite.h"
#include "Global.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Characters/PlayerCharacter.h"
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

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)return;

	AAIController* controller = Cast<AAIController>(OwnerComp.GetOwner());
	UBehaviorComponent* behavior = CHelpers::GetComponent<UBehaviorComponent>(controller);

	AEnemy* aiPawn = Cast<AEnemy>(controller->GetPawn());
	UStateComponent* state = CHelpers::GetComponent<UStateComponent>(aiPawn);

	UBlackBoardPlayerArrayObject* obj = Cast<UBlackBoardPlayerArrayObject>(BlackboardComp->GetValueAsObject(behavior->GetPerceptedPlayersKey()));
	if (!obj)CLog::Print("obj null", 223, 10, FColor::Blue);
	else if (!obj->GetPlayers().Num())CLog::Print("obj empty", 223, 1, FColor::Blue);
	else if (obj->GetPlayers().Num())
	{
		for (int32 i = 0; i < obj->GetPlayers().Num(); ++i)
			CLog::Print(obj->GetPlayers()[i]->GetName(), 223 + i, 1, FColor::Blue);
	}

	CheckTrue(state->IsDeadMode());	

	if (state->IsHittedMode())		
	{
		behavior->SetHittedMode();	
		return;
	}

	APlayerCharacter* target = behavior->GetTarget();

	if (target == nullptr)
	{
		CLog::Print("target null", 222, 1, FColor::Black);
		behavior->SetWaitMode();
		return;
	}
	else
	{
		CLog::Print("target on", 222, 1, FColor::Black);
		UStateComponent* targetState = CHelpers::GetComponent<UStateComponent>(target);
		if (targetState->IsDeadMode())
		{
			behavior->SetWaitMode();
			return;
		}
	}

	float distance = aiPawn->GetDistanceTo(target);

}