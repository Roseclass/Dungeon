#include "Behavior/BTT_DialogExit.h"
#include "Global.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTFunctionLibrary.h"

#include "DungeonPlayerController.h"
#include "Characters/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "DungeonPlayerController.h"
#include "Behavior/PlayerDialogDatas.h"

UBTT_DialogExit::UBTT_DialogExit()
{
	NodeName = "Exit";

	PlayerDatas.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_DialogExit, PlayerDatas), UPlayerDialogDatas::StaticClass());
	InteractingPlayer.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_DialogExit, InteractingPlayer), ADungeonPlayerController::StaticClass());
}

EBTNodeResult::Type UBTT_DialogExit::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// get interacting player controller
	ADungeonPlayerController* player = nullptr;
	player = Cast<ADungeonPlayerController>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(InteractingPlayer.SelectedKeyName));
	if (!player)return EBTNodeResult::Failed;

	// shut down dialog widget
	player->Client_DialogExit();

	// Enable Movement
	ACharacter* pawn = Cast<ACharacter>(player->GetPawn());
	if (pawn)pawn->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	// save dialog point
	UPlayerDialogDatas* datas = Cast<UPlayerDialogDatas>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(PlayerDatas.SelectedKeyName));
	if (!datas)
	{
		CLog::Print("UBTT_DialogExit datas is nullptr", -1, 10, FColor::Red);
		return EBTNodeResult::Failed;
	}
	int32& point = datas->PointMap.FindOrAdd(player);
	point = NextPoint;
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(PlayerDatas.SelectedKeyName, datas);

	// release interacting player
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(InteractingPlayer.SelectedKeyName, nullptr);

	return EBTNodeResult::Succeeded;
}

FString UBTT_DialogExit::GetStaticDescription() const
{
	Super::GetStaticDescription();
	FString result = FString::Printf(TEXT("NextPoint : %d"), NextPoint);
	return result;
}
