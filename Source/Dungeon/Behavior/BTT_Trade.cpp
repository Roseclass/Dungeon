#include "Behavior/BTT_Trade.h"
#include "Global.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "DungeonPlayerController.h"
#include "Behavior/PlayerDialogDatas.h"

#include "Components/InventoryComponent.h"

UBTT_Trade::UBTT_Trade()
{
	NodeName = "Trade";

	InteractingPlayer.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_Trade, InteractingPlayer), ADungeonPlayerController::StaticClass());
}

EBTNodeResult::Type UBTT_Trade::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// get interacting player controller
	ADungeonPlayerController* player = nullptr;
	player = Cast<ADungeonPlayerController>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(InteractingPlayer.SelectedKeyName));
	if (!player)return EBTNodeResult::Failed;

	// start trade
	UInventoryComponent* inv  = CHelpers::GetComponent<UInventoryComponent>(player->GetPawn());
	if (!inv)return EBTNodeResult::Failed;
	AActor* npc = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("SelfActor"));
	if(!npc)return EBTNodeResult::Failed;
	//inv->Trade(npc);

	return EBTNodeResult::Succeeded;
}
