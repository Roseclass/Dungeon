#include "Characters/NPC.h"
#include "Global.h"
#include "Components/ShapeComponent.h"
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
	Init();
}

void ANPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANPC::Interact(APlayerCharacter* InPlayer)
{
	IsInteracting = 1;
	Dialog->OnInteraction(InPlayer);
	CheckNull(InPlayer);
	ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(InPlayer->GetController());
	CheckNull(controller);
	InPlayer->GetCharacterMovement()->DisableMovement();
}

void ANPC::PreInteractEvent(APlayerCharacter* InPlayer)
{

}

void ANPC::InInteractEvent(APlayerCharacter* InPlayer)
{

}

void ANPC::PostInteractEvent(APlayerCharacter* InPlayer)
{

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

void ANPC::EndInteract()
{
	IsInteracting = 0;
}