#include "Behavior/BTS_Footman.h"
#include "Global.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Characters/PlayerCharacter.h"
#include "Characters/Enemy.h"
#include "Characters/EnemyAIController.h"
#include "Components/BehaviorComponent.h"
#include "Components/SkillComponent.h"

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
	USkillComponent* skill = CHelpers::GetComponent<USkillComponent>(aiPawn);

	CheckTrue(skill->IsDead());
	//CheckTrue(skill->IsSequenceMode());

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
