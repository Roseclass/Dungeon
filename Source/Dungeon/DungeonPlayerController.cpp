#include "DungeonPlayerController.h"
#include "Global.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/World.h"
#include "AIController.h"

#include "Characters/DungeonCharacter.h"
#include "Objects/Weapon.h"
#include "Widgets/UW_Main.h"

ADungeonPlayerController::ADungeonPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void ADungeonPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (MainWidgetClass)
	{
		MainWidget = CreateWidget<UUW_Main, ADungeonPlayerController>(this, MainWidgetClass);
		MainWidget->AddToViewport();
	}

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

	if (Item)
	{
		if (myPawn->IsOverlappingActor(Item))
		{
			StopMovement();
			myPawn->TryAddItem(Item->GetItemObject());
			Item = nullptr;
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

		if (other && dist < 500 && myPawn->CanUse() && other->GetGenericTeamId() != myPawn->GetGenericTeamId())
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
	InputComponent->BindAction("RightClick", IE_Pressed, this, &ADungeonPlayerController::OnRightClick);
	InputComponent->BindAction("QuickSlot0", IE_Pressed, this, &ADungeonPlayerController::OnQuickSlot0);
	InputComponent->BindAction("QuickSlot1", IE_Pressed, this, &ADungeonPlayerController::OnQuickSlot1);
	InputComponent->BindAction("QuickSlot2", IE_Pressed, this, &ADungeonPlayerController::OnQuickSlot2);
	InputComponent->BindAction("QuickSlot3", IE_Pressed, this, &ADungeonPlayerController::OnQuickSlot3);

	InputComponent->BindAction("SkillTree", IE_Pressed, this, &ADungeonPlayerController::OnSkillTree);
	InputComponent->BindAction("Inventory", IE_Pressed, this, &ADungeonPlayerController::OnInventory);
}

void ADungeonPlayerController::OnSetDestinationPressed()
{
	Target = nullptr;
	Item = nullptr;
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
		AWeapon* const otherWeapon = Cast<AWeapon>(Hit.GetActor());

		if (other && other->GetGenericTeamId() != myPawn->GetGenericTeamId())
		{
			UAIBlueprintHelperLibrary::SimpleMoveToActor(this, other);
			Target = other;
		}
		else if (otherWeapon)
		{
			UAIBlueprintHelperLibrary::SimpleMoveToActor(this, otherWeapon);
			Item = otherWeapon;
		}
		else
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, Hit.Location);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, Hit.Location, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
		}
	}
}

void ADungeonPlayerController::OnRightClick()
{
	ADungeonCharacter* const myPawn = Cast<ADungeonCharacter>(GetPawn());
	CheckNull(myPawn);
	myPawn->UseRight();
}

void ADungeonPlayerController::OnQuickSlot0()
{
	ADungeonCharacter* const myPawn = Cast<ADungeonCharacter>(GetPawn());
	CheckNull(myPawn);
	myPawn->UseQ();
}

void ADungeonPlayerController::OnQuickSlot1()
{
	ADungeonCharacter* const myPawn = Cast<ADungeonCharacter>(GetPawn());
	CheckNull(myPawn);
	myPawn->UseW();
}

void ADungeonPlayerController::OnQuickSlot2()
{
	ADungeonCharacter* const myPawn = Cast<ADungeonCharacter>(GetPawn());
	CheckNull(myPawn);
	myPawn->UseE();
}

void ADungeonPlayerController::OnQuickSlot3()
{
	ADungeonCharacter* const myPawn = Cast<ADungeonCharacter>(GetPawn());
	CheckNull(myPawn);
	myPawn->UseR();
}

void ADungeonPlayerController::OnSkillTree()
{
	ADungeonCharacter* const myPawn = Cast<ADungeonCharacter>(GetPawn());
	CheckNull(myPawn);
	myPawn->ToggleSkillTree();
}

void ADungeonPlayerController::OnInventory()
{
	ADungeonCharacter* const myPawn = Cast<ADungeonCharacter>(GetPawn());
	CheckNull(myPawn);
	myPawn->ToggleInventory();
}
