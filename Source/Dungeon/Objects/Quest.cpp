#include "Objects/Quest.h"
#include "Global.h"

#include "DungeonPlayerController.h"

AQuest::AQuest()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AQuest::BeginPlay()
{
	Super::BeginPlay();
	OnDestroyed.AddDynamic(this, &AQuest::OnQuestDestroyed);

	MainQuestOjbective = NewObject<UQuest_Objective>(this, UQuest_Objective::StaticClass());
	MainQuestOjbective->SetQuestConditions(MainConditions);
	AdditiveQuestOjbective = NewObject<UQuest_Objective>(this, UQuest_Objective::StaticClass());
	AdditiveQuestOjbective->SetQuestConditions(AdditiveConditions);
}

void AQuest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AQuest::OnQuestDestroyed(AActor* DestroyedActor)
{
	//if (OnCompleted.IsBound())
	//	OnCompleted.Broadcast(RootNumber, NodeNumber);
}
