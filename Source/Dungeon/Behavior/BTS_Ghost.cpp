#include "Behavior/BTS_Ghost.h"
#include "Global.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Characters/PlayerCharacter.h"
#include "Characters/Enemy.h"
#include "Characters/EnemyAIController.h"
#include "Components/SkillComponent.h"
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
	USkillComponent* skill = CHelpers::GetComponent<USkillComponent>(aiPawn);

	CheckTrue(skill->IsDead());
	//CheckTrue(state->IsSequenceMode());

	{
		UBlackBoardPlayerArrayObject* obj = Cast<UBlackBoardPlayerArrayObject>(BlackboardComp->GetValueAsObject(PerceptedPlayerObject.SelectedKeyName));
		if (obj)
		{
			for (auto i : obj->GetPlayers())
			{
				float dist = UKismetMathLibrary::Vector_Distance(i->GetActorLocation(), aiPawn->GetActorLocation());
				if (dist < behavior->GetAvoidRange())
				{
					behavior->SetAvoidMode();
					return;
				}
			}
			behavior->SetWaitMode();
		}
	}

	if (skill->IsHit())
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
		USkillComponent* targetSkill = CHelpers::GetComponent<USkillComponent>(target);
		if (targetSkill->IsDead())
		{
			behavior->SetWaitMode();
			return;
		}
	}
}
