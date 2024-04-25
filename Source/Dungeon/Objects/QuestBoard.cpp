#include "Objects/QuestBoard.h"
#include "Global.h"
#include "Components/SceneComponent.h"

#include "Characters/PlayerCharacter.h"
#include "Components/QuestComponent.h"
#include "Components/QuestListComponent.h"
#include "Objects/Quest.h"
#include "Objects/QuestPoster.h"

AQuestBoard::AQuestBoard()
{
	PrimaryActorTick.bCanEverTick = true;

	//scene
	CHelpers::CreateComponent(this, &RootScene, "RootScene");
}

void AQuestBoard::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		TArray<USceneComponent*> shapeComponents;
		GetComponents<USceneComponent>(shapeComponents);
		for (USceneComponent* component : shapeComponents)
		{
			for (auto i : component->ComponentTags)
			{
				if (i == ComponentTag)
				{
					PosterPinRoots.Add(component);
					break;
				}
			}
		}
	}
}

void AQuestBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AQuestBoard::GeneratePosters()
{
	// only in server

	// spawn posters
	int32 rootMax = PosterPinRoots.Num();

	for (int32 i = 0; i < PosterPinRoots.Num(); ++i)
	{
		AQuestPoster* poster = GetWorld()->SpawnActorDeferred<AQuestPoster>(PosterClass, PosterPinRoots[i]->GetComponentTransform(), this);
		UGameplayStatics::FinishSpawningActor(poster, PosterPinRoots[i]->GetComponentTransform());
		if (!poster)CLog::Print("AQuestBoard::GeneratePosters poster is nullptr");
		else
		{
			poster->SetBoard(this);
			QuestPosters.Add(poster);
		}
	}	
}

void AQuestBoard::ActivateQuestPosters()
{
	APawn* pawn = UGameplayStatics::GetPlayerControllerFromID(GetWorld(), 0)->GetPawn();

	UQuestComponent* quest = CHelpers::GetComponent<UQuestComponent>(pawn);
	if (quest)
	{
		quest->SetCannotToggle();
		quest->OffWidget();
	}

	for (auto i : QuestPosters)
		i->IsHidden() ? i->Deactivate() : i->Activate();
}

void AQuestBoard::DeactivateQuestPosters()
{
	APawn* pawn = UGameplayStatics::GetPlayerControllerFromID(GetWorld(), 0)->GetPawn();

	UQuestComponent* quest = CHelpers::GetComponent<UQuestComponent>(pawn);
	if (quest)
		quest->SetCanToggle();

	for (auto i : QuestPosters)
		i->Deactivate();
}

void AQuestBoard::GenerateQuests()
{
	// only in server

	// get server's pawn
	APawn* pawn = UGameplayStatics::GetPlayerControllerFromID(GetWorld(), 0)->GetPawn();

	TArray<FQuestTreeData> treeDatas;
	TArray<TSubclassOf<AQuest>> questClasses;
	if (List)
	{
		List->FindAvailableQuest(treeDatas);
		if (treeDatas.IsEmpty())
			CLog::Print("AQuestBoard::GenerateQuests arr empty", -1, 10, FColor::Red);
		else
		{
			for (auto i : treeDatas)
				questClasses.Add(i.QuestClass);
		}
	}
	else CLog::Print("AQuestBoard::GenerateQuests list is nullptr", -1, 10, FColor::Red);

	// destroy quests
	for (auto i : Quests)
		i->Destroy();

	// respawn random quests
	int32 posterMax = QuestPosters.Num();

	if (!posterMax)
	{
		GeneratePosters();
		posterMax = QuestPosters.Num();
	}

	// root >= Quests
	if (questClasses.Num() <= posterMax)
	{
		for (int32 i = 0; i < questClasses.Num(); ++i)
		{
			AQuest* quest = GetWorld()->SpawnActorDeferred<AQuest>(questClasses[i], QuestPosters[i]->GetActorTransform(), this);
			UGameplayStatics::FinishSpawningActor(quest, QuestPosters[i]->GetActorTransform());
			QuestPosters[i]->SetQuest(quest);
			QuestPosters[i]->SetActorHiddenInGame(0);
		}
		for (int32 i = questClasses.Num(); i < posterMax; ++i)
		{
			CLog::Print(i);
			QuestPosters[i]->SetActorHiddenInGame(1);
		}
		return;
	}

	// root < Quests
	for (int32 i = 0; i < posterMax; ++i)
	{
		int32 idx = UKismetMathLibrary::RandomIntegerInRange(0, questClasses.Num());
		AQuest* quest = GetWorld()->SpawnActorDeferred<AQuest>(questClasses[idx], QuestPosters[i]->GetActorTransform(), this);
		UGameplayStatics::FinishSpawningActor(quest, QuestPosters[i]->GetActorTransform());
		QuestPosters[i]->SetQuest(quest);
	}
}
