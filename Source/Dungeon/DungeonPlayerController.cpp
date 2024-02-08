#include "DungeonPlayerController.h"
#include "Global.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/DungeonCharacter.h"
#include "Engine/World.h"
#include "AIController.h"

ADungeonPlayerController::ADungeonPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void ADungeonPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	ADungeonCharacter* const myPawn = Cast<ADungeonCharacter>(GetPawn());
	CheckNull(myPawn);

	if (Target)
	{		
		float dist = UKismetMathLibrary::Vector_Distance(myPawn->GetActorLocation(), Target->GetActorLocation());
		if (dist < 500)
		{
			StopMovement();
			if (myPawn->CanUse())
			{
				myPawn->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(myPawn->GetActorLocation(), Target->GetActorLocation()));
				myPawn->UseLeft();
			}
			Target = nullptr;
		}
		return;
	}

	if(bInputPressed)
	{
		FollowTime += DeltaTime;

		FVector HitLocation = FVector::ZeroVector;
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, true, Hit);
		HitLocation = Hit.Location;

		ADungeonCharacter* const other = Cast<ADungeonCharacter>(Hit.GetActor());
		float dist = 1e9;
		if (other)
		{
			dist = UKismetMathLibrary::Vector_Distance(myPawn->GetActorLocation(), other->GetActorLocation());
		}
		if (other && dist < 500 && myPawn->CanUse())
		{
			//dist는 좌클릭 스킬 데이터에서 사거리를 받아와야함
			myPawn->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(myPawn->GetActorLocation(), other->GetActorLocation()));
			myPawn->UseLeft();
		}
		else if(myPawn && myPawn->CanMove())
		{
			FVector WorldDirection = (HitLocation - myPawn->GetActorLocation()).GetSafeNormal();
			myPawn->AddMovementInput(WorldDirection, 1.f, false);
		}
	}
	else
	{
		FollowTime = 0.f;
	}
}

void ADungeonPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ADungeonPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ADungeonPlayerController::OnSetDestinationReleased);
}

void ADungeonPlayerController::OnSetDestinationPressed()
{
	Target = nullptr;
	bInputPressed = true;
	StopMovement();
}

void ADungeonPlayerController::OnSetDestinationReleased()
{
	// Player is no longer pressing the input
	bInputPressed = false;

	ADungeonCharacter* const myPawn = Cast<ADungeonCharacter>(GetPawn());
	CheckNull(myPawn);

	if(!myPawn->CanMove())return;

	if(FollowTime <= ShortPressThreshold)
	{
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, true, Hit);

		ADungeonCharacter* const other = Cast<ADungeonCharacter>(Hit.GetActor());
		if (other)
		{
			UAIBlueprintHelperLibrary::SimpleMoveToActor(this, other);
			Target = other;
		}
		else
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, Hit.Location);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, Hit.Location, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
		}
	}
}
