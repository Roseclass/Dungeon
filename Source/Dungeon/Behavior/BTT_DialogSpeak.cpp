#include "Behavior/BTT_DialogSpeak.h"
#include "Global.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Widgets/UW_Dialog.h"

UBTT_DialogSpeak::UBTT_DialogSpeak()
{
	NodeName = "Speak";

	DialogWidget.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_DialogSpeak, DialogWidget), UUW_Dialog::StaticClass());
}

EBTNodeResult::Type UBTT_DialogSpeak::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UUW_Dialog* widget = nullptr;
	widget = Cast<UUW_Dialog>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(DialogWidget.SelectedKeyName));

	if (widget)
	{
		widget->OnSpeakFinished.AddUFunction(this, "OnSpeakFinished");
		widget->Speak(Text);
	}

	return EBTNodeResult::InProgress;
}

void UBTT_DialogSpeak::OnSpeakFinished(UBehaviorTreeComponent* OwnerComp)
{
	UUW_Dialog* widget = nullptr;
	widget = Cast<UUW_Dialog>(OwnerComp->GetBlackboardComponent()->GetValueAsObject(DialogWidget.SelectedKeyName));
	if (widget)	widget->OnSpeakFinished.Clear();
	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}
