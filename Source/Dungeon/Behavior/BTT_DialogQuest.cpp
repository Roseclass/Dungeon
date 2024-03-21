#include "Behavior/BTT_DialogQuest.h"
#include "Global.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Objects/Quest.h"
#include "Components/QuestComponent.h"
#include "Components/QuestListComponent.h"
#include "Widgets/UW_Dialog.h"
#include "Widgets/UW_DialogEntry.h"

EBTNodeResult::Type UBTT_DialogQuest::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* controller = Cast<AAIController>(OwnerComp.GetOwner());
	if (!controller)
	{
		CLog::Print("UBTT_DialogQuest::ExecuteTask, AIController Component is nullptr");
		return EBTNodeResult::Failed;
	}
	UQuestListComponent* list = CHelpers::GetComponent<UQuestListComponent>(controller->GetPawn());
	if (!list)
	{
		CLog::Print("UBTT_DialogQuest::ExecuteTask, List Component is nullptr");
		return EBTNodeResult::Failed;
	}

	// 1. 리스트 컴포넌트에서 현재 수락 가능한 퀘스트 목록 받아옴
	TArray<FQuestTreeData> arr;
	list->FindAvailableQuest(arr, 200);

	// 2. 목록에 따른 퀘스트 오브젝트 생성
	UUW_Dialog* widget = nullptr;
	widget = Cast<UUW_Dialog>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(DialogWidget.SelectedKeyName));

	if (widget)
	{
		// 3. 선택지 추가
		widget->Quest(arr);
		if (bPoint)widget->Point(PrevPointText);
		// 4. 선택지에 함수 바인딩
		//widget->OnQuestReplyFinished.AddDynamic(this, &UBTT_DialogQuest::OnQuestReplyFinished);
		//widget->OnPointReplyFinished.AddDynamic(this, &UBTT_DialogQuest::OnPointReplyFinished);
	}

	return EBTNodeResult::InProgress;
}

void UBTT_DialogQuest::OnQuestReplyFinished(UBehaviorTreeComponent* OwnerComp, UDialogReplyObject* InObject)
{
	UUW_Dialog* widget = nullptr;
	widget = Cast<UUW_Dialog>(OwnerComp->GetBlackboardComponent()->GetValueAsObject(DialogWidget.SelectedKeyName));
	if (widget)
	{
		//widget->OnQuestReplyFinished.Remove(this, "OnQuestReplyFinished");
		//widget->OnPointReplyFinished.Remove(this, "OnPointReplyFinished");
	}

	ACharacter* player = nullptr;
	player = Cast<ACharacter>(OwnerComp->GetBlackboardComponent()->GetValueAsObject(Player.SelectedKeyName));

	if (!player)
	{
		CLog::Print("UBTT_DialogQuest::OnQuestReplyFinished, Player is nullptr");
		FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
	}

	UQuestComponent* comp = CHelpers::GetComponent<UQuestComponent>(player);
	AQuest* quest = GetWorld()->SpawnActor<AQuest>(InObject->GetData().QuestClass);
	if (!quest)
	{
		CLog::Print("UBTT_DialogQuest::OnQuestReplyFinished, Spawn Failed");
		FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
	}

	AAIController* controller = Cast<AAIController>(OwnerComp->GetOwner());
	if (!controller)
	{
		CLog::Print("UBTT_DialogQuest::OnQuestReplyFinished, AIController Component is nullptr");
		FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
	}
	UQuestListComponent* list = CHelpers::GetComponent<UQuestListComponent>(controller->GetPawn());
	if (!list)
	{
		CLog::Print("UBTT_DialogQuest::OnQuestReplyFinished, List Component is nullptr");
		FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
	}

	comp->AddQuest(quest);
	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}

void UBTT_DialogQuest::OnPointReplyFinished(UBehaviorTreeComponent* OwnerComp)
{
	UUW_Dialog* widget = nullptr;
	widget = Cast<UUW_Dialog>(OwnerComp->GetBlackboardComponent()->GetValueAsObject(DialogWidget.SelectedKeyName));
	if (widget)
	{
		//widget->OnQuestReplyFinished.Remove(this, "OnQuestReplyFinished");
		//widget->OnPointReplyFinished.Remove(this, "OnPointReplyFinished");
	}
	OwnerComp->GetBlackboardComponent()->SetValueAsInt(Point.SelectedKeyName, PrevPoint);
	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}

