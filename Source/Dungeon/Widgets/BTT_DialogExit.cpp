#include "Widgets/BTT_DialogExit.h"
#include "Global.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTFunctionLibrary.h"

#include "DungeonPlayerController.h"
#include "Characters/PlayerCharacter.h"

#include "Widgets/UW_Dialog.h"

UBTT_DialogExit::UBTT_DialogExit()
{
	NodeName = "Speak";

	DialogWidget.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_DialogExit, DialogWidget), UUW_Dialog::StaticClass());
	Reward.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_DialogExit, Reward));
}

EBTNodeResult::Type UBTT_DialogExit::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UUW_Dialog* widget = nullptr;
	widget = Cast<UUW_Dialog>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(DialogWidget.SelectedKeyName));

	if (widget)
	{
		widget->Exit();
		ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(widget->GetOwningPlayer());
		//controller->OnHUD();
		//controller->EnableInput(controller);

		APlayerCharacter* player = Cast<APlayerCharacter>(controller->GetPawn());
		player->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}

	Reset(OwnerComp);
	return EBTNodeResult::Succeeded;
}

void UBTT_DialogExit::Reset(UBehaviorTreeComponent& OwnerComp)
{
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(Reward.SelectedKeyName, bReward);
}
