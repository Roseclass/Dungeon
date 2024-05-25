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

	MainCounts.Init(0, MainConditions.Num());
	MainQuestOjbectivePool.Init(TSet<AActor*>(),MainConditions.Num());
	MainQuestOjbective = NewObject<UQuest_Objective>(this, UQuest_Objective::StaticClass());
	MainQuestOjbective->SetQuestConditions(MainConditions);
	MainQuestOjbective->SetQuestCounts(MainCounts);

	AdditiveCounts.Init(0, AdditiveConditions.Num());
	AdditiveQuestOjbectivePool.Init(TSet<AActor*>(), AdditiveConditions.Num());
	AdditiveQuestOjbective = NewObject<UQuest_Objective>(this, UQuest_Objective::StaticClass());
	AdditiveQuestOjbective->SetQuestConditions(AdditiveConditions);
	AdditiveQuestOjbective->SetQuestCounts(AdditiveCounts);
}

void AQuest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AQuest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicated 변수를 여기에 추가
	DOREPLIFETIME_CONDITION(AQuest, MainCounts, COND_None);
	DOREPLIFETIME_CONDITION(AQuest, AdditiveCounts, COND_None);
}

void AQuest::OnQuestDestroyed(AActor* DestroyedActor)
{
	//if (OnCompleted.IsBound())
	//	OnCompleted.Broadcast(RootNumber, NodeNumber);
}

void AQuest::OnRep_MainCounts()
{
	//TODO::UpdateWidget
	MainQuestOjbective->OnQuestCountChanged.ExecuteIfBound();
}

void AQuest::OnRep_AdditiveCounts()
{
	//TODO::UpdateWidget
	AdditiveQuestOjbective->OnQuestCountChanged.ExecuteIfBound();
}

void AQuest::CheckCondition(AActor* InObject)
{
	CheckNull(InObject);

	for (int32 i = 0; i < MainQuestOjbectivePool.Num(); ++i)
	{
		if (MainQuestOjbectivePool[i].Find(InObject))
		{
			++MainCounts[i];
			MainQuestOjbectivePool[i].Remove(InObject);
		}
	}

	for (int32 i = 0; i < AdditiveQuestOjbectivePool.Num(); ++i)
	{
		if (AdditiveQuestOjbectivePool[i].Find(InObject))
		{
			++AdditiveCounts[i];
			AdditiveQuestOjbectivePool[i].Remove(InObject);
		}
	}
}

void AQuest::AddToQuestPool(AActor* InObject)
{
	CheckNull(InObject);

	for (int32 i = 0; i < MainConditions.Num(); ++i)
	{
		EQuestConditionType type = MainConditions[i].GetType();
		if (type == EQuestConditionType::DestroyingEnemy)
		{
			for (auto j : MainConditions[i].GetTargetEnemyClasses())
				if (InObject->GetClass()->IsChildOf(j))
				{
					MainQuestOjbectivePool[i].Add(InObject);
					break;
				}
		}
		else if (type == EQuestConditionType::Interact)
		{
			for (auto j : MainConditions[i].GetTargetInteractClasses())
				if (InObject->GetClass()->IsChildOf(j))
				{
					MainQuestOjbectivePool[i].Add(InObject);
					break;
				}
		}
	}
	for (int32 i = 0; i < AdditiveConditions.Num(); ++i)
	{
		EQuestConditionType type = AdditiveConditions[i].GetType();
		if (type == EQuestConditionType::DestroyingEnemy)
		{
			for (auto j : AdditiveConditions[i].GetTargetEnemyClasses())
				if (InObject->GetClass()->IsChildOf(j))
				{
					AdditiveQuestOjbectivePool[i].Add(InObject);
					break;
				}
		}
		else if (type == EQuestConditionType::Interact)
		{
			for (auto j : AdditiveConditions[i].GetTargetInteractClasses())
				if (InObject->GetClass()->IsChildOf(j))
				{
					AdditiveQuestOjbectivePool[i].Add(InObject);
					break;
				}
		}
	}

}
