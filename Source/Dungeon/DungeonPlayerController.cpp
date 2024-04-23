#include "DungeonPlayerController.h"
#include "Global.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/World.h"
#include "AIController.h"

#include "Characters/PlayerCharacter.h"
#include "Characters/Enemy.h"
#include "Characters/NPC.h"
#include "Components/DialogComponent.h"
#include "Components/ConfirmPopupComponent.h"
#include "Objects/Weapon.h"
#include "Objects/ItemManager.h"
#include "Widgets/UW_Main.h"
#include "Widgets/UW_Dialog.h"

#include "Interfaces/IInteractable.h"

ADungeonPlayerController::ADungeonPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	CHelpers::CreateActorComponent<UConfirmPopupComponent>(this, &ConfirmPopup, "ConfirmPopup");
}

void ADungeonPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		ItemManager = GetWorld()->SpawnActor<AItemManager>(AItemManager::StaticClass());
		ItemManager->SetOwner(this);
		if (IsLocalController())OnRep_ItemManager();
	}
}

void ADungeonPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	APlayerCharacter* const viewTarget = Cast<APlayerCharacter>(GetViewTarget());
	CheckNull(viewTarget);

	APlayerCharacter* const myPawn = Cast<APlayerCharacter>(GetPawn());
	CheckNull(myPawn);

	if (Target)
	{		
		float dist = UKismetMathLibrary::Vector_Distance(myPawn->GetActorLocation(), Target->GetActorLocation());
		if (dist < 500)
		{
			StopPawnImmediately();
			if (myPawn->CanUse())
			{
				Client_ReplicateRotation(UKismetMathLibrary::FindLookAtRotation(myPawn->GetActorLocation(), Target->GetActorLocation()));
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
			StopPawnImmediately();
			myPawn->TryAddItem(Item);
			Item = nullptr;
		}
		return;
	}

	if (Iteractable)
	{
		AActor* inter = Cast<AActor>(Iteractable->_getUObject());
		if (myPawn->IsOverlappingActor(inter))
		{
			StopPawnImmediately();
			Server_Interaction(inter);
			Iteractable = nullptr;
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

		AEnemy* const other = Cast<AEnemy>(Hit.GetActor());
		float dist = 1e9;
		if (other)
		{
			dist = UKismetMathLibrary::Vector_Distance(myPawn->GetActorLocation(), other->GetActorLocation());
		}

		if (other && dist < 500 && myPawn->CanUse() && other->GetGenericTeamId() != myPawn->GetGenericTeamId())
		{
			//dist는 좌클릭 스킬 데이터에서 사거리를 받아와야함
			Client_ReplicateRotation(UKismetMathLibrary::FindLookAtRotation(myPawn->GetActorLocation(), other->GetActorLocation()));
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

void ADungeonPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicated 변수를 여기에 추가
	DOREPLIFETIME_CONDITION(ADungeonPlayerController, ItemManager, COND_None);
	DOREPLIFETIME_CONDITION(ADungeonPlayerController, Index, COND_None);
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
	InputComponent->BindAction("Chat", IE_Pressed, this, &ADungeonPlayerController::OnChat);
}

void ADungeonPlayerController::PostNetInit()
{
	Super::PostNetInit();
}

void ADungeonPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	Client_CreateMainWidget();
}

void ADungeonPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	// OnPossess of Client Returns Pawn as Nullptr
	if (GetPawn())
	{
		MainWidget->Init(GetPawn());
	}
	else CLog::Print("ADungeonPlayerController::OnRep_Pawn is nullptr", -1, 10, FColor::Red);
}

void ADungeonPlayerController::OnRep_ItemManager()
{
	//클라이언트 동기화 완료 체크

	if (ItemManager)
	{
		CLog::Print("load");

		TArray<AActor*>arr;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWeapon::StaticClass(), arr);
		for (auto i : arr)
		{
			AWeapon* item = Cast<AWeapon>(i);
			if (!item)continue;
			item->SetManager(ItemManager);
		}
	}
}

void ADungeonPlayerController::Client_CreateMainWidget_Implementation()
{
	if (MainWidgetClass)
	{
		MainWidget = CreateWidget<UUW_Main, ADungeonPlayerController>(this, MainWidgetClass);
		MainWidget->AddToViewport();
		if (HasAuthority())
		{
			APawn* pawn = GetPawn();
			if (pawn)MainWidget->Init(pawn);
			else CLog::Print("ADungeonPlayerController::Client_CreateMainWidget_Implementation sever pawn is nulltpr", -1, 10, FColor::Red);

		}
	}
}

void ADungeonPlayerController::Server_ReplicateRotation_Implementation(FRotator NewRotation, ADungeonPlayerController* Exception)
{
	Multicast_ReplicateRotation(NewRotation, Exception);
}

void ADungeonPlayerController::Multicast_ReplicateRotation_Implementation(FRotator NewRotation, ADungeonPlayerController* Exception)
{
	CheckTrue(Exception && Exception->IsLocalController());
	APlayerCharacter* const myPawn = Cast<APlayerCharacter>(GetPawn());
	CheckNull(myPawn);
	myPawn->SetActorRotation(NewRotation);
}

void ADungeonPlayerController::Client_ReplicateRotation_Implementation(FRotator NewRotation)
{
	APlayerCharacter* const myPawn = Cast<APlayerCharacter>(GetPawn());
	CheckNull(myPawn);
	myPawn->SetActorRotation(NewRotation);
	Server_ReplicateRotation(NewRotation, this);
}

void ADungeonPlayerController::Server_Interaction_Implementation(AActor* InInteractable)
{
	IIInteractable* const interactable = Cast<IIInteractable>(InInteractable);
	CheckNull(interactable);
	interactable->Interact(this);
}

void ADungeonPlayerController::Server_SelectReply_Implementation(AActor* InInteractable, int32 NextPoint)
{
	CheckNull(InInteractable);
	UDialogComponent* dialog = CHelpers::GetComponent<UDialogComponent>(InInteractable);
	CheckNull(dialog);
	dialog->OnReply(this, NextPoint);
}

void ADungeonPlayerController::OnSetDestinationPressed()
{
	Target = nullptr;
	Item = nullptr;
	Iteractable = nullptr;
	bInputPressed = true;
	StopPawnImmediately();
}

void ADungeonPlayerController::OnSetDestinationReleased()
{
	// Player is no longer pressing the input
	bInputPressed = false;

	APlayerCharacter* const viewTarget = Cast<APlayerCharacter>(GetViewTarget());
	CheckNull(viewTarget);

	APlayerCharacter* const myPawn = Cast<APlayerCharacter>(GetPawn());
	CheckNull(myPawn);

	if(!myPawn->CanMove())return;

	if(FollowTime <= ShortPressThreshold)
	{
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, true, Hit);

		AEnemy* const other = Cast<AEnemy>(Hit.GetActor());
		AWeapon* const otherWeapon = Cast<AWeapon>(Hit.GetActor());
		IIInteractable* const otherInter = Cast<IIInteractable>(Hit.GetActor());

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
		else if (otherInter)
		{
			AActor* inter = Cast<AActor>(otherInter->_getUObject());
			UAIBlueprintHelperLibrary::SimpleMoveToActor(this, inter);
			Iteractable = otherInter;
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
	APlayerCharacter* const myPawn = Cast<APlayerCharacter>(GetPawn());
	CheckNull(myPawn);
	myPawn->UseRight();
}

void ADungeonPlayerController::OnQuickSlot0()
{
	APlayerCharacter* const myPawn = Cast<APlayerCharacter>(GetPawn());
	CheckNull(myPawn);
	myPawn->UseQ();
}

void ADungeonPlayerController::OnQuickSlot1()
{
	APlayerCharacter* const myPawn = Cast<APlayerCharacter>(GetPawn());
	CheckNull(myPawn);
	myPawn->UseW();
}

void ADungeonPlayerController::OnQuickSlot2()
{
	APlayerCharacter* const myPawn = Cast<APlayerCharacter>(GetPawn());
	CheckNull(myPawn);
	myPawn->UseE();
}

void ADungeonPlayerController::OnQuickSlot3()
{
	APlayerCharacter* const myPawn = Cast<APlayerCharacter>(GetPawn());
	CheckNull(myPawn);
	myPawn->UseR();
}

void ADungeonPlayerController::OnSkillTree()
{
	APlayerCharacter* const myPawn = Cast<APlayerCharacter>(GetPawn());
	CheckNull(myPawn);
	myPawn->ToggleSkillTree();
}

void ADungeonPlayerController::OnInventory()
{
	APlayerCharacter* const myPawn = Cast<APlayerCharacter>(GetPawn());
	CheckNull(myPawn);
	myPawn->ToggleInventory();
}

void ADungeonPlayerController::OnChat()
{
	CheckNull(MainWidget);
	MainWidget->OnChat();
}

void ADungeonPlayerController::Client_DialogInit_Implementation(ANPC* InNPC)
{
	if (!InNPC)
	{
		CLog::Print("ADungeonPlayerController::Client_DialogInit_Implementation InNPC is nullptr");
		return;
	}
	if (DialogWidgetClass && !DialogWidget)
	{
		DialogWidget = CreateWidget<UUW_Dialog, ADungeonPlayerController>(this, DialogWidgetClass);
		DialogWidget->AddToViewport();
	}
	if (DialogWidget)
	{
		DialogWidget->SetVisibility(ESlateVisibility::Visible);
		DialogWidget->SetInteractingNPC(InNPC);
		UDialogComponent* dialog = CHelpers::GetComponent<UDialogComponent>(InNPC);
		DialogWidget->Init(dialog->GetPortrait(), dialog->GetName());
		DialogWidget->OnSpeakFinished.AddUFunction(this, "Server_Interaction");
		DialogWidget->OnReplyFinished.AddUFunction(this, "Server_SelectReply");
	}
}

void ADungeonPlayerController::Client_DialogSpeak_Implementation(const FText& InText)
{
	if (!DialogWidget)
	{
		CLog::Print("ADungeonPlayerController::Client_DialogSpeak_Implementation DialogWidget is nullptr");
		return;
	}

	DialogWidget->Speak(InText);

}

void ADungeonPlayerController::Client_DialogReply_Implementation(const TArray<FText>& InReplies)
{
	if (!DialogWidget)
	{
		CLog::Print("ADungeonPlayerController::Client_DialogSpeak_Implementation DialogWidget is nullptr");
		return;
	}

	DialogWidget->Reply(InReplies);

}

void ADungeonPlayerController::Client_DialogExit_Implementation()
{
	if (!DialogWidget)
	{
		CLog::Print("ADungeonPlayerController::Client_DialogSpeak_Implementation DialogWidget is nullptr");
		return;
	}

	DialogWidget->Exit();

}

void ADungeonPlayerController::Server_SendChat_Implementation(const FText& InText)
{
	// send chat to every clients
	int32 max = UGameplayStatics::GetNumLocalPlayerControllers(GetWorld());

	for (int32 i = 0; i < max; ++i)
	{
		ADungeonPlayerController* player = Cast<ADungeonPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), i));
		if (!player)continue;
		player->Client_SendChat(InText);
	}	
}

void ADungeonPlayerController::Client_SendChat_Implementation(const FText& InText)
{
	// update chat

	CheckNull(MainWidget);
	MainWidget->OnMessageUpdated(InText, FColor::Red);
}

void ADungeonPlayerController::SetUIOnlyMode()
{
	FInputModeUIOnly InputMode;
	SetInputMode(InputMode);
}

void ADungeonPlayerController::SetGameAndUIMode()
{
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(0);
	SetInputMode(InputMode);
}

void ADungeonPlayerController::RevealMainWidget()
{
	// reveal main widget
	MainWidget->SetVisibility(ESlateVisibility::Visible);
}

void ADungeonPlayerController::HideWidget()
{
	// hide main widget
	MainWidget->SetVisibility(ESlateVisibility::Collapsed);

	HideAddtiveWidget();
}

void ADungeonPlayerController::HideAddtiveWidget()
{
	APlayerCharacter* const myPawn = Cast<APlayerCharacter>(GetPawn());
	CheckNull(myPawn);
	myPawn->HideSkillTree();
	myPawn->HideInventory();
}

void ADungeonPlayerController::StopPawnImmediately()
{
	APlayerCharacter* const myPawn = Cast<APlayerCharacter>(GetPawn());
	CheckNull(myPawn);
	myPawn->GetCharacterMovement()->StopMovementImmediately();
}
