#include "Objects/Quest.h"
#include "Global.h"

#include "DungeonPlayerController.h"
#include "Characters/Enemy.h"

AQuest::AQuest()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = 1;
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

void AQuest::CheckCondition(AActor* InObject)
{
	/*for (auto& i : MainConditions)
	{
		EQuestConditionType type = i.GetType();
		if (type == EQuestConditionType::DestroyingEnemy)
		{
			for (auto j : i.GetTargetEnemyClasses())
				if (InObject->GetClass()->IsChildOf(j))
					i.IncreaseEnemyCount();
		}
		else if (type == EQuestConditionType::Interact)
		{
			for (auto j : i.GetTargetInteractClasses())
				if (InObject->GetClass()->IsChildOf(j))
					i.IncreaseInteractCount();
		}
	}

	for (auto& i : AdditiveConditions)
	{
		EQuestConditionType type = i.GetType();
		if (type == EQuestConditionType::DestroyingEnemy)
		{
			for (auto j : i.GetTargetEnemyClasses())
				if (InObject->GetClass()->IsChildOf(j))
					i.IncreaseEnemyCount();
		}
		else if (type == EQuestConditionType::Interact)
		{
			for (auto j : i.GetTargetInteractClasses())
				if (InObject->GetClass()->IsChildOf(j))
					i.IncreaseInteractCount();
		}
	}*/

	//TODO:: find object from map increase count
}

void AQuest::AddToQuestPool(AActor* InObject)
{
	//TODO:: Addtoobjectpool
	// find class, add to map
}
