#include "Behavior/BTT_DialogQuest.h"
#include "Global.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "DungeonPlayerController.h"
#include "Behavior/PlayerDialogDatas.h"

#include "Objects/Quest.h"
#include "Components/QuestComponent.h"
#include "Components/QuestListComponent.h"

UBTT_DialogQuest::UBTT_DialogQuest()
{
	NodeName = "Quest";

	InteractingPlayer.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_DialogQuest, InteractingPlayer), ADungeonPlayerController::StaticClass());
}

EBTNodeResult::Type UBTT_DialogQuest::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// get interacting player controller
	ADungeonPlayerController* player = nullptr;
	player = Cast<ADungeonPlayerController>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(InteractingPlayer.SelectedKeyName));
	if (!player)return EBTNodeResult::Failed;

	// if server
	if (!player->IsLocalController())
	{
		CLog::Print("UBTT_DialogQuest player isn't ServerController", -1, 10, FColor::Red);
		return EBTNodeResult::Failed;
	}

	// set veiw target to board actor
	AActor* npc = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("SelfActor"));
	if (!npc)return EBTNodeResult::Failed;
	UQuestListComponent* list = CHelpers::GetComponent<UQuestListComponent>(npc);
	if (!list)return EBTNodeResult::Failed;
	list->ShowList(player);

	return EBTNodeResult::Succeeded;
}
