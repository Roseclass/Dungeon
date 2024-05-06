#include "Behavior/BTS_Ghost.h"
#include "Global.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Characters/PlayerCharacter.h"
#include "Characters/Enemy.h"
#include "Characters/EnemyAIController.h"
#include "Components/StateComponent.h"
#include "Components/StatusComponent.h"
#include "Components/BehaviorComponent.h"

UBTS_Ghost::UBTS_Ghost()
{
	NodeName = "Ghost";

	PerceptedPlayerObject.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_Ghost, PerceptedPlayerObject), UBlackBoardPlayerArrayObject::StaticClass());
	Target.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_Ghost, Target), APlayerCharacter::StaticClass());
}

void UBTS_Ghost::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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

	//// print percepted players
	//{
	//	UBlackBoardPlayerArrayObject* obj = Cast<UBlackBoardPlayerArrayObject>(BlackboardComp->GetValueAsObject(PerceptedPlayerObject.SelectedKeyName));
	//	if (!obj)CLog::Print("obj null", 223, 10, FColor::Blue);
	//	else if (!obj->GetPlayers().Num())CLog::Print("obj empty", 223, 1, FColor::Blue);
	//	else if (obj->GetPlayers().Num())
	//	{
	//		for (int32 i = 0; i < obj->GetPlayers().Num(); ++i)
	//			CLog::Print(obj->GetPlayers()[i]->GetName(), 223 + i, 1, FColor::Blue);
	//	}
	//}

	//// print target
	//{
	//	APlayerCharacter* target = Cast<APlayerCharacter>(BlackboardComp->GetValueAsObject(Target.SelectedKeyName));
	//	if (target == nullptr)CLog::Print("target null", 222, 1, FColor::Black);
	//	else CLog::Print("target on", 222, 1, FColor::Black);
	//}

	{
		UBlackBoardPlayerArrayObject* obj = Cast<UBlackBoardPlayerArrayObject>(BlackboardComp->GetValueAsObject(PerceptedPlayerObject.SelectedKeyName));
		if (obj)
		{
			for (auto i : obj->GetPlayers())
			{
				float dist = UKismetMathLibrary::Vector_Distance(i->GetActorLocation(), aiPawn->GetActorLocation());
				if (behavior->GetAvoidRange() < dist)
				{
					behavior->SetAvoidMode();
					return;
				}
			}
		}
	}

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
