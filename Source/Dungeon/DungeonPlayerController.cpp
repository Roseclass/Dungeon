// Copyright Epic Games, Inc. All Rights Reserved.

#include "DungeonPlayerController.h"
#include "Global.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/DungeonCharacter.h"
#include "Engine/World.h"

ADungeonPlayerController::ADungeonPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void ADungeonPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if(bInputPressed)
	{
		FollowTime += DeltaTime;

		// Look for the touch location
		FVector HitLocation = FVector::ZeroVector;
		FHitResult Hit;
		if(bIsTouch)
		{
			GetHitResultUnderFinger(ETouchIndex::Touch1, ECC_Visibility, true, Hit);
		}
		else
		{
			GetHitResultUnderCursor(ECC_Visibility, true, Hit);
		}
		HitLocation = Hit.Location;

		// Direct the Pawn towards that location
		ADungeonCharacter* const MyPawn = Cast<ADungeonCharacter>(GetPawn());
		if(MyPawn)
		{
			FVector WorldDirection = (HitLocation - MyPawn->GetActorLocation()).GetSafeNormal();
			MyPawn->AddMovementInput(WorldDirection, 1.f, false);

			ADungeonCharacter* const other = Cast<ADungeonCharacter>(Hit.GetActor());

			if (other && MyPawn->GetGenericTeamId() != other->GetGenericTeamId())
			{
				CLog::Print("IN");
			}
		}
	}
	else
	{
		FollowTime = 0.f;
	}
}

void ADungeonPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ADungeonPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ADungeonPlayerController::OnSetDestinationReleased);

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ADungeonPlayerController::OnTouchPressed);
	InputComponent->BindTouch(EInputEvent::IE_Released, this, &ADungeonPlayerController::OnTouchReleased);

}

void ADungeonPlayerController::OnSetDestinationPressed()
{
	// We flag that the input is being pressed
	bInputPressed = true;
	// Just in case the character was moving because of a previous short press we stop it
	StopMovement();
}

void ADungeonPlayerController::OnSetDestinationReleased()
{
	// Player is no longer pressing the input
	bInputPressed = false;

	// If it was a short press
	if(FollowTime <= ShortPressThreshold)
	{
		// We look for the location in the world where the player has pressed the input
		FVector HitLocation = FVector::ZeroVector;
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, true, Hit);
		HitLocation = Hit.Location;

		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, HitLocation);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, HitLocation, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}
}

void ADungeonPlayerController::OnTouchPressed(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	bIsTouch = true;
	OnSetDestinationPressed();
}

void ADungeonPlayerController::OnTouchReleased(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	bIsTouch = false;
	OnSetDestinationReleased();
}
