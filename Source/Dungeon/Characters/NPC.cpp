#include "Characters/NPC.h"
#include "Global.h"
#include "Components/ShapeComponent.h"
#include "Components/MeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "DungeonPlayerController.h"
#include "Characters/PlayerCharacter.h"
#include "Components/DialogComponent.h"

ANPC::ANPC()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateActorComponent(this, &Dialog, "Dialog");
}

void ANPC::BeginPlay()
{
	Super::BeginPlay();	
	FindInteractionCollision();
	FindMesh();
	Init();
}

void ANPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANPC::StartInteract(ADungeonPlayerController* InPlayer)
{
	CheckNull(InPlayer);
	Dialog->OnInteraction(InPlayer);

	APlayerCharacter* playerPawn = Cast<APlayerCharacter>(InPlayer->GetPawn());
	CheckNull(playerPawn);
	playerPawn->GetCharacterMovement()->DisableMovement();
}

void ANPC::EndInteract(ADungeonPlayerController* InPlayer)
{
	IsInteracting = 0;
}

void ANPC::StartCursorOver(ADungeonPlayerController* InPlayer)
{
	for (auto i : MeshComponents)
		i->SetRenderCustomDepth(1);
}

void ANPC::EndCursorOver(ADungeonPlayerController* InPlayer)
{
	for (auto i : MeshComponents)
		i->SetRenderCustomDepth(0);
}

bool ANPC::IsInteractable()
{
	return !IsInteracting;
}

void ANPC::Init()
{

}

void ANPC::FindInteractionCollision()
{
	//Find InteractionCollision
	TArray<UShapeComponent*> shapeComponents;
	GetComponents<UShapeComponent>(shapeComponents);
	for (UShapeComponent* component : shapeComponents)
	{
		for (auto i : component->ComponentTags)
		{
			if (i == InteractionCollisionTag)
			{
				InteractionCollisionComponents.Add(component);
				break;
			}
		}
	}
}

void ANPC::FindMesh()
{
	// find mesh
	GetComponents<UMeshComponent>(MeshComponents);
}