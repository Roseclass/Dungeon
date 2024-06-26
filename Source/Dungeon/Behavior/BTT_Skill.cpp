#include "Behavior/BTT_Skill.h"
#include "Global.h"
#include "GameplayTagContainer.h"

#include "Characters/EnemyAIController.h"
#include "Characters/Enemy.h"
#include "Components/SkillComponent.h"

UBTT_Skill::UBTT_Skill()
{
	NodeName = "Skill";
	bNotifyTick = true;

	SkillTags.AddTag(FGameplayTag::RequestGameplayTag("State.Skill"));
}

EBTNodeResult::Type UBTT_Skill::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// use skill by index in enemy
	AEnemyAIController* controller = Cast<AEnemyAIController>(OwnerComp.GetOwner());
	AEnemy* aiPawn = Cast<AEnemy>(controller->GetPawn());
	aiPawn->UseSkill(SkillIndex);

	return EBTNodeResult::InProgress;
}

void UBTT_Skill::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// is enemy's state IDLE?
	// that means skill sequence's end

	AAIController* controller = Cast<AAIController>(OwnerComp.GetOwner());
	AEnemy* aiPawn = Cast<AEnemy>(controller->GetPawn());

	USkillComponent* skill = CHelpers::GetComponent<USkillComponent>(aiPawn);

	if (!skill->HasAnyMatchingGameplayTags(SkillTags))
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}