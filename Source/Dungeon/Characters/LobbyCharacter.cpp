#include "Characters/LobbyCharacter.h"

ALobbyCharacter::ALobbyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ALobbyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALobbyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALobbyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

