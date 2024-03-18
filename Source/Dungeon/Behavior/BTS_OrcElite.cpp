#include "Behavior/BTS_OrcElite.h"
#include "Global.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Characters/PlayerCharacter.h"
#include "Characters/Enemy.h"
#include "Characters/EnemyAIController.h"
#include "Components/StateComponent.h"
#include "Components/StatusComponent.h"
#include "Components/BehaviorComponent.h"

UBTS_OrcElite::UBTS_OrcElite()
{
	NodeName = "OrcElite";

	PerceptedPlayerObject.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_OrcElite, PerceptedPlayerObject), UBlackBoardPlayerArrayObject::StaticClass());
	Target.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_OrcElite, Target), APlayerCharacter::StaticClass());
	Phase.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_OrcElite, Phase));
	Sequence.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_OrcElite, Sequence));
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
	UStatusComponent* status = CHelpers::GetComponent<UStatusComponent>(aiPawn);

	// print percepted players
	{
		UBlackBoardPlayerArrayObject* obj = Cast<UBlackBoardPlayerArrayObject>(BlackboardComp->GetValueAsObject(PerceptedPlayerObject.SelectedKeyName));
		if (!obj)CLog::Print("obj null", 223, 10, FColor::Blue);
		else if (!obj->GetPlayers().Num())CLog::Print("obj empty", 223, 1, FColor::Blue);
		else if (obj->GetPlayers().Num())
		{
			for (int32 i = 0; i < obj->GetPlayers().Num(); ++i)
				CLog::Print(obj->GetPlayers()[i]->GetName(), 223 + i, 1, FColor::Blue);
		}
	}

	// print target
	{
		APlayerCharacter* target = Cast<APlayerCharacter>(BlackboardComp->GetValueAsObject(Target.SelectedKeyName));
		if (target == nullptr)CLog::Print("target null", 222, 1, FColor::Black);
		else CLog::Print("target on", 222, 1, FColor::Black);
	}

	CheckTrue(state->IsDeadMode());	
	CheckTrue(state->IsSequenceMode());

	int32 prevPhase = BlackboardComp->GetValueAsInt(Phase.SelectedKeyName);
	int32 phase = CheckPhase(status->GetCurrentHealth() / status->GetMaxHealth(), prevPhase);
	CLog::Print(phase, 224, 1, FColor::Purple);

	if (phase != prevPhase)
	{
		behavior->SetSequenceMode();
		return;
	}

	if (state->IsHittedMode())		
	{
		behavior->SetHittedMode();	
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

int32 UBTS_OrcElite::CheckPhase(float HealthRate, int32 PrevPhase)
{
	int32 result = 0;
	if (HealthRate <= 0.5 && PrevPhase == 0)
		result = 1;

	return result;
}
