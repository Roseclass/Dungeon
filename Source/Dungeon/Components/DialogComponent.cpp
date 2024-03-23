#include "Components/DialogComponent.h"
#include "Global.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

#include "DungeonPlayerController.h"
#include "Characters/NPC.h"
#include "Characters/PlayerCharacter.h"
#include "Behavior/PlayerDialogDatas.h"

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
	CheckFalse(owner->HasAuthority());
	Controller = Cast<AAIController>(owner->GetController());
	UBlackboardComponent* bbComp = Controller->GetBlackboardComponent();
	PlayerDialogDatas = NewObject<UPlayerDialogDatas>(owner, UPlayerDialogDatas::StaticClass());
	if (!bbComp)
	{
		Controller->UseBlackboard(Blackboard, bbComp);
		bbComp->SetValueAsObject(PlayerDialogDatasObjectBBName, PlayerDialogDatas);
		Controller->RunBehaviorTree(DialogTree);
	}
}

void UDialogComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UDialogComponent::OnInteraction(ADungeonPlayerController* InPlayerController)
{
	if (InPlayerController)
	{
		InPlayerController->Client_DialogInit(Cast<ANPC>(GetOwner()));
		UBlackboardComponent* bbComp = Controller->GetBlackboardComponent();
		if (!bbComp)Controller->UseBlackboard(Blackboard, bbComp);
		bbComp->SetValueAsObject(PlayerControllerObjectBBName, InPlayerController);
		bbComp->SetValueAsBool(RewardBoolBBName, 1);
	}
}

void UDialogComponent::OnReply(ADungeonPlayerController* InPlayerController, int32 NextPoint)
{
	if (InPlayerController)
	{
		int32& point = PlayerDialogDatas->PointMap.FindOrAdd(InPlayerController);
		point = NextPoint;

		UBlackboardComponent* bbComp = Controller->GetBlackboardComponent();
		if (!bbComp)Controller->UseBlackboard(Blackboard, bbComp);
		bbComp->SetValueAsObject(PlayerControllerObjectBBName, InPlayerController);
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
	bbComp->SetValueAsObject(PlayerControllerObjectBBName, nullptr);
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
