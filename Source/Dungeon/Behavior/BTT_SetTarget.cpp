#include "Behavior/BTT_SetTarget.h"
#include "Global.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Characters/DungeonCharacter.h"
#include "Characters/EnemyAIController.h"
#include "Characters/Enemy.h"
#include "Components/BehaviorComponent.h"

UBTT_SetTarget::UBTT_SetTarget()
{
	NodeName = "Target";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_SetTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)return EBTNodeResult::Failed;

    AEnemyAIController* controller = Cast<AEnemyAIController>(OwnerComp.GetOwner());
    if (!controller)return EBTNodeResult::Failed;

    AEnemy* aiPawn = Cast<AEnemy>(controller->GetPawn());
    if (!aiPawn)return EBTNodeResult::Failed;

    UBlackBoardPlayerArrayObject* obj = Cast<UBlackBoardPlayerArrayObject>(BlackboardComp->GetValueAsObject(PerceptedPlayerObject.SelectedKeyName));
    if (!obj)return EBTNodeResult::Failed;
    if (!obj->GetPlayers().Num())return EBTNodeResult::Failed;

    ADungeonCharacter* NewTarget = nullptr;

    switch (Option)
    {
    case ESetTargetOption::Closest:NewTarget = FindClosest(aiPawn, obj); break;
    case ESetTargetOption::Farthest:NewTarget = FindFarthest(aiPawn, obj); break;
    case ESetTargetOption::Random:NewTarget = FindRandom(aiPawn, obj); break;
    default:break;
    }
    if (!NewTarget)return EBTNodeResult::Failed;

    BlackboardComp->SetValueAsObject(Target.SelectedKeyName, NewTarget);
	return EBTNodeResult::Succeeded;
}

ADungeonCharacter* UBTT_SetTarget::FindClosest(AEnemy* Origin, UBlackBoardPlayerArrayObject* Others)
{
    ADungeonCharacter* result = nullptr;
    const TArray<ADungeonCharacter*>& arr = Others->GetPlayers();
    int32 idx = 0; float dist = 1e9;
    for (int32 i = 0; i < arr.Num(); ++i)
    {
        float temp = FVector::Distance(Origin->GetActorLocation(),arr[i]->GetActorLocation());
        if (dist < temp)continue;
        dist = temp; idx = i;
    }
    if (arr.IsValidIndex(idx))result = arr[idx];
    return result;
}

ADungeonCharacter* UBTT_SetTarget::FindFarthest(AEnemy* Origin, UBlackBoardPlayerArrayObject* Others)
{
    ADungeonCharacter* result = nullptr;
    const TArray<ADungeonCharacter*>& arr = Others->GetPlayers();
    int32 idx = 0; float dist = -1e9;
    for (int32 i = 0; i < arr.Num(); ++i)
    {
        float temp = FVector::Distance(Origin->GetActorLocation(), arr[i]->GetActorLocation());
        if (dist > temp)continue;
        dist = temp; idx = i;
    }
    if (arr.IsValidIndex(idx))result = arr[idx];
    return result;
}

ADungeonCharacter* UBTT_SetTarget::FindRandom(AEnemy* Origin, UBlackBoardPlayerArrayObject* Others)
{
    ADungeonCharacter* result = nullptr;
    const TArray<ADungeonCharacter*>& arr = Others->GetPlayers();
    int32 idx = UKismetMathLibrary::RandomIntegerInRange(0, arr.Num() - 1);
    if (arr.IsValidIndex(idx))result = arr[idx];
    return result;
}
