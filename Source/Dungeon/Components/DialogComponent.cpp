#include "Components/DialogComponent.h"
#include "Global.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameFramework/PlayerController.h"

#include "Characters/NPC.h"
#include "Characters/PlayerCharacter.h"

#include "Widgets/UW_Dialog.h"

UDialogComponent::UDialogComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDialogComponent::BeginPlay()
{
	Super::BeginPlay();

	ANPC* owner = Cast<ANPC>(GetOwner());
	CheckNull(owner);
	Controller = Cast<AAIController>(owner->GetController());
	UBlackboardComponent* bbComp = Controller->GetBlackboardComponent();
	if (!bbComp)Controller->UseBlackboard(Blackboard, bbComp);
}

void UDialogComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UDialogComponent::OnInteraction(APlayerCharacter* InPlayer)
{
	APlayerController* controller = Cast<APlayerController>(InPlayer->GetController());
	if (DialogWidgetClass)
	{
		DialogWidget = CreateWidget<UUW_Dialog, APlayerController>(controller, DialogWidgetClass);
		DialogWidget->OnExit.AddUFunction(this, "OnExit");
		DialogWidget->Init(Portrait,Name);
		DialogWidget->AddToViewport();
		controller->SetInputMode(FInputModeUIOnly());
		controller->SetShowMouseCursor(1);
		UBlackboardComponent* bbComp = Controller->GetBlackboardComponent();
		if (!bbComp)Controller->UseBlackboard(Blackboard, bbComp);
		bbComp->SetValueAsObject(WidgetObjectBBName, DialogWidget);
		bbComp->SetValueAsObject(PlayerObjectBBName, InPlayer);
		bbComp->SetValueAsBool(RewardBoolBBName, 1);
		Controller->RunBehaviorTree(DialogTree);
		DialogWidget->SetBTComponent(Cast<UBehaviorTreeComponent>(Controller->GetBrainComponent()));
	}
	else
	{
		CLog::Print("DialogWidgetClass Is Null");
		return;
	}
}

void UDialogComponent::OnExit()
{
	CheckNull(DialogWidget);
	APlayerController* controller = Cast<APlayerController>(DialogWidget->GetOwningPlayer());
	CheckNull(controller);
	DialogWidget->RemoveFromParent();
	Controller->GetBrainComponent()->StopLogic(FString());
	UBlackboardComponent* bbComp = Controller->GetBlackboardComponent();
	bbComp->SetValueAsObject(PlayerObjectBBName, nullptr);
	ANPC* npc = Cast<ANPC>(GetOwner());
	if (npc)npc->EndInteract();
}

void UDialogComponent::SetValueAsBool(FName InName, bool Value)
{
	UBlackboardComponent* bbComp = Controller->GetBlackboardComponent();
	if (!bbComp) { CLog::Print("UDialogComponent::SetValueAsBool, BlackboardComponent is nullptr"); return; }
	if (!bbComp->IsValidKey(bbComp->GetKeyID(InName)))
	{
		CLog::Print("UDialogComponent::SetValueAsBool,Key Name -" + InName.ToString() + "- is Wrong"); return;
	}
	bbComp->SetValueAsBool(InName, Value);
}

void UDialogComponent::SetValueAsInt(FName InName, int32 Value)
{
	UBlackboardComponent* bbComp = Controller->GetBlackboardComponent();
	if (!bbComp) { CLog::Print("UDialogComponent::SetValueAsInt, BlackboardComponent is nullptr"); return; }
	if (!bbComp->IsValidKey(bbComp->GetKeyID(InName)))
	{
		CLog::Print("UDialogComponent::SetValueAsInt,Key Name -" + InName.ToString() + "- is Wrong"); return;
	}
	bbComp->SetValueAsInt(InName, Value);
}

bool UDialogComponent::GetValueAsBool(FName InName)
{
	bool result = 0;
	if (!Controller)return result;

	UBlackboardComponent* bbComp = Controller->GetBlackboardComponent();
	if (!bbComp) { CLog::Print("UDialogComponent::GetValueAsBool, BlackboardComponent is nullptr"); return result; }
	if (!bbComp->IsValidKey(bbComp->GetKeyID(InName)))
	{
		CLog::Print("UDialogComponent::GetValueAsBool,Key Name -" + InName.ToString() + "- is Wrong"); return result;
	}

	result = bbComp->GetValueAsBool(InName);
	return result;
}

int32 UDialogComponent::GetValueAsInt(FName InName)
{
	int32 result = 0;
	if (!Controller)return result;

	UBlackboardComponent* bbComp = Controller->GetBlackboardComponent();
	if (!bbComp) { CLog::Print("UDialogComponent::GetValueAsInt, BlackboardComponent is nullptr"); return result; }
	if (!bbComp->IsValidKey(bbComp->GetKeyID(InName)))
	{
		CLog::Print("UDialogComponent::GetValueAsInt,Key Name -" + InName.ToString() + "- is Wrong"); return result;
	}

	result = bbComp->GetValueAsInt(InName);
	return result;
}
