#include "Behavior/BTS_Quest.h"
#include "Global.h"

#include "Characters/PlayerCharacter.h"

#include "Behavior/PlayerDialogDatas.h"

UBTS_Quest::UBTS_Quest()
{
	NodeName = "Quest";

	Data.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_Quest, Data), UPlayerDialogDatas::StaticClass());
	InteractingPlayer.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_Quest, InteractingPlayer), APlayerCharacter::StaticClass());
}

void UBTS_Quest::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}
