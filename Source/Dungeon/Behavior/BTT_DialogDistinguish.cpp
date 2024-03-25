#include "Behavior/BTT_DialogDistinguish.h"
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

UBTT_DialogDistinguish::UBTT_DialogDistinguish()
{
	NodeName = "DistinguishSpeak";

	PlayerDatas.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_DialogDistinguish, PlayerDatas), UPlayerDialogDatas::StaticClass());
	InteractingPlayer.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_DialogDistinguish, InteractingPlayer), ADungeonPlayerController::StaticClass());
}

EBTNodeResult::Type UBTT_DialogDistinguish::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// get interacting player controller
	ADungeonPlayerController* player = nullptr;
	player = Cast<ADungeonPlayerController>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(InteractingPlayer.SelectedKeyName));
	if (!player)return EBTNodeResult::Failed;

	int32 next = Client_NextPoint;
	FText text = Client_Text;

	// if server
	if (player->IsLocalController())
	{
		next = Server_NextPoint;
		text = Server_Text;
	}
	// change dialog text
	player->Client_DialogSpeak(text);

	// save dialog point
	UPlayerDialogDatas* datas = Cast<UPlayerDialogDatas>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(PlayerDatas.SelectedKeyName));
	if (!datas)
	{
		CLog::Print("UBTT_DialogDistinguish datas is nullptr", -1, 10, FColor::Red);
		return EBTNodeResult::Failed;
	}
	int32& point = datas->PointMap.FindOrAdd(player);
	point = next;
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(PlayerDatas.SelectedKeyName, datas);

	// release interacting player
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(InteractingPlayer.SelectedKeyName, nullptr);

	return EBTNodeResult::Succeeded;
}

FString UBTT_DialogDistinguish::GetStaticDescription() const
{
	Super::GetStaticDescription();
	FString result = FString::Printf(TEXT("Server_NextPoint : %d"), Server_NextPoint);
	result += FString::Printf(TEXT("\nServer_Text : %s"), *Server_Text.ToString());
	result += FString::Printf(TEXT("\nClient_NextPoint : %d"), Client_NextPoint);
	result += FString::Printf(TEXT("\nClient_Text : %s"), *Client_Text.ToString());
	return result;
}
