#include "Objects/QuestPoster.h"
#include "Global.h"

#include "DungeonPlayerController.h"
#include "Characters/PlayerCharacter.h"

#include "Objects/QuestBoard.h"
#include "Components/QuestComponent.h"
#include "Widgets/UW_QuestPopup.h"

AQuestPoster::AQuestPoster()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AQuestPoster::BeginPlay()
{
	Super::BeginPlay();
	
	OnBeginCursorOver.AddDynamic(this, &AQuestPoster::OnActorBeginCursorOver);
	OnEndCursorOver.AddDynamic(this, &AQuestPoster::OnActorEndCursorOver);
	OnClicked.AddDynamic(this, &AQuestPoster::OnActorClicked);
}

void AQuestPoster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AQuestPoster::OnActorBeginCursorOver(AActor* TouchedActor)
{
	CheckFalse(bActive);
	CheckTrue(Board->IsClicked());
	
	// Glow
	OnGlow();

	// preview Quest
	APawn* pawn = UGameplayStatics::GetPlayerControllerFromID(GetWorld(), 0)->GetPawn();

	UQuestComponent* quest = CHelpers::GetComponent<UQuestComponent>(pawn);
	if (quest)
	{
		quest->SetPreview(Quest);
		quest->OnWidget();
	}
}

void AQuestPoster::OnActorEndCursorOver(AActor* TouchedActor)
{
	CheckFalse(bActive);
	CheckTrue(Board->IsClicked());

	// off Glow
	OffGlow();

	// off widget
	APawn* pawn = UGameplayStatics::GetPlayerControllerFromID(GetWorld(), 0)->GetPawn();

	UQuestComponent* quest = CHelpers::GetComponent<UQuestComponent>(pawn);
	if (quest)quest->OffWidget();
}

void AQuestPoster::OnActorClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	CheckFalse(bActive);
	CheckTrue(Board->IsClicked());

	// create popup
	CreatePopup();

	// update state
	Board->SetClicked(1);
}

void AQuestPoster::CreatePopup()
{
	// create widget
	if (PopupWidgetClass)
	{
		PopupWidget = CreateWidget<UUW_QuestPopup, APlayerController>(UGameplayStatics::GetPlayerControllerFromID(GetWorld(), 0), PopupWidgetClass);
		PopupWidget->AddToViewport();
	}else CLog::Print("AQuestPoster::CreatePopup PopupWidgetClass is null");

	// bind button events, Set Description
	if (PopupWidget)
	{
		PopupWidget->OnConfirm.BindUFunction(this, "Confirm");
		PopupWidget->OnCancel.BindUFunction(this, "Cancel");
		PopupWidget->Init(PopupWidgetDescription);
	}else CLog::Print("AQuestPoster::CreatePopup PopupWidget is nullptr");
}

void AQuestPoster::Confirm()
{
	// set Quest to player
	APawn* pawn = UGameplayStatics::GetPlayerControllerFromID(GetWorld(), 0)->GetPawn();

	UQuestComponent* quest = CHelpers::GetComponent<UQuestComponent>(pawn);
	if (quest)quest->SetQuest(Quest);

	// update state
	Board->SetClicked(0);

	// remove popup
	if (PopupWidget)PopupWidget->RemoveFromParent();

	// end quest seqeunce
	Board->EndSequence();
}

void AQuestPoster::Cancel()
{
	// update state
	Board->SetClicked(0);

	// remove popup
	if (PopupWidget)PopupWidget->RemoveFromParent();
}

void AQuestPoster::SetQuest(AQuest* NewQuest)
{
	Quest = NewQuest;
}  

void AQuestPoster::SetBoard(AQuestBoard* NewBoard)
{
	Board = NewBoard;
}
