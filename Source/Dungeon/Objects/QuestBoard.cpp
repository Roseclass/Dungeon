#include "Objects/QuestBoard.h"
#include "Global.h"
#include "Components/SceneComponent.h"

AQuestBoard::AQuestBoard()
{
	PrimaryActorTick.bCanEverTick = true;

	//scene
	CHelpers::CreateComponent(this, &RootScene, "RootScene");
}

void AQuestBoard::BeginPlay()
{
	Super::BeginPlay();

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

void AQuestBoard::GenerateQuests(TArray<TSubclassOf<AQuest>> InQuests)
{
	// Spawn Posters on PosterPinRoots
}