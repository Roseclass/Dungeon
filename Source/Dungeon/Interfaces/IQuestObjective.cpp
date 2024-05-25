#include "Interfaces/IQuestObjective.h"
#include "Global.h"

#include "DungeonPlayerController.h"
#include "Objects/Quest.h"
#include "Components/QuestComponent.h"

void IIQuestObjective::QuestObjective_Init()
{
	AActor* actor = Cast<AActor>(_getUObject());
	ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(UGameplayStatics::GetPlayerController(actor->GetWorld(), 0));
	LinkedComponent = CHelpers::GetComponent<UQuestComponent>(controller->GetPawn());

	if(LinkedComponent && actor->HasAuthority())
			LinkedComponent->AddToQuestPool(actor);
}