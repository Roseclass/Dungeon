#include "Components/ConfirmPopupComponent.h"
#include "Global.h"

#include "DungeonPlayerController.h"
#include "Widgets/UW_ConfirmPopup.h"

UConfirmPopupComponent::UConfirmPopupComponent()
{
	SetIsReplicatedByDefault(1);
	PrimaryComponentTick.bCanEverTick = true;
}

void UConfirmPopupComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UConfirmPopupComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UConfirmPopupComponent::Server_Confirm_Implementation()
{
	// find server host's confirm comp
	UConfirmPopupComponent* confirm = CHelpers::GetComponent<UConfirmPopupComponent>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	// upadte owning player's state
	if (confirm)
	{
		ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(GetOwner());
		confirm->UpdateState(controller, EPopupState::Confirm);
		confirm->UpdateSign(controller, EPopupState::Confirm);
	}
	else CLog::Print("UConfirmPopupComponent::Server_Confirm_Implementation", -1, 10, FColor::Red);
}

void UConfirmPopupComponent::Server_Cancel_Implementation()
{
	// find server host's confirm comp
	UConfirmPopupComponent* confirm = CHelpers::GetComponent<UConfirmPopupComponent>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	// upadte owning player's state
	if (confirm)
	{
		ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(GetOwner());
		confirm->UpdateState(controller, EPopupState::Cancel);		
		confirm->UpdateSign(controller, EPopupState::Cancel);		
	}
	else CLog::Print("UConfirmPopupComponent::Server_Confirm_Implementation", -1, 10, FColor::Red);
}

void UConfirmPopupComponent::Client_CreatePopup_Implementation(const FString& InString, const TArray<AActor*>& NewPortraitActors, float NewTimeLimit)
{
	ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(GetOwner());
	PopupWidget = CreateWidget<UUW_ConfirmPopup, ADungeonPlayerController>(controller, PopupClass);
	PopupWidget->OnConfirm.BindUFunction(this, "Server_Confirm");
	PopupWidget->OnCancel.BindUFunction(this, "Server_Cancel");
	PopupWidget->Init(InString, NewPortraitActors, NewTimeLimit);
	PopupWidget->AddToViewport();
}

void UConfirmPopupComponent::Client_UpdateSign_Implementation(AActor* PortraitActor, bool NewState)
{
	// update sign
	if (PopupWidget)
		PopupWidget->UpdateSign(PortraitActor, NewState);
	else CLog::Print("UConfirmPopupComponent::Client_UpdateSign_Implementation PopupWidget is nullptr", -1, 10, FColor::Red);
}

void UConfirmPopupComponent::Client_ConfirmedSequence_Implementation()
{
	// update sign
	if (PopupWidget)
		PopupWidget->ConfirmedSequence();
	else CLog::Print("UConfirmPopupComponent::Client_UpdateSign_Implementation PopupWidget is nullptr", -1, 10, FColor::Red);
}

void UConfirmPopupComponent::SendPopupAllPlayers(FString InString, TFunction<bool()> FinishedEvent, float NewTimeLimit)
{
	// send popup every clients
	int32 numOfPlayers = UGameplayStatics::GetNumPlayerControllers(GetWorld());
	
	TArray<AActor*> arr;

	for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PlayerController = It->Get();
		if (PlayerController)
		{
			APawn* ControlledPawn = PlayerController->GetPawn();
			if (ControlledPawn)	arr.Add(ControlledPawn);
		}
	}

	for (int32 i = 0; i < numOfPlayers; ++i)
	{
		ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), i));
		States.FindOrAdd(controller) = EPopupState::Wait;
		UConfirmPopupComponent* confirm = CHelpers::GetComponent<UConfirmPopupComponent>(controller);
		if (confirm)confirm->Client_CreatePopup(InString, arr, NewTimeLimit);
		else CLog::Print("nullptr");
	}
	ReserveFinishedEvent = FinishedEvent;
}

void UConfirmPopupComponent::UpdateState(ADungeonPlayerController* InPlayer, EPopupState NewState)
{
	EPopupState& state = States.FindOrAdd(InPlayer);
	state = NewState;

	int32 size = States.Num();
	for (auto i : States)
		if (i.Value == EPopupState::Confirm)--size;

	if (!size)
	{
		for (auto i : States)
		{
			UConfirmPopupComponent* confirm = CHelpers::GetComponent<UConfirmPopupComponent>(i.Key);
			if (confirm)
			{
				confirm->Client_ConfirmedSequence();
			}
			else CLog::Print("UConfirmPopupComponent::UpdateSign can't find confirm", -1, 10, FColor::Red);
		}

		// execute function;
		ReserveFinishedEvent();
		ReserveFinishedEvent = nullptr;

	}
}

void UConfirmPopupComponent::UpdateSign(ADungeonPlayerController* InPlayer, EPopupState NewState)
{
	bool find = 0;
	int32 Index = 0;
	for (auto i : States)
	{
		if (i.Key == InPlayer) 
		{
			find = 1;
			break;
		}
		++Index;
	}
	if (!find)
	{
		CLog::Print("UConfirmPopupComponent::UpdateSign can't find player", -1, 10, FColor::Red);
		return;
	}
	for (auto i : States)
	{
		UConfirmPopupComponent* confirm = CHelpers::GetComponent<UConfirmPopupComponent>(i.Key);
		if (confirm)
		{
			if (NewState == EPopupState::Confirm)confirm->Client_UpdateSign(InPlayer->GetPawn(), 1);
			else if(NewState == EPopupState::Cancel)confirm->Client_UpdateSign(InPlayer->GetPawn(), 0);
		}
		else CLog::Print("UConfirmPopupComponent::UpdateSign can't find confirm", -1, 10, FColor::Red);
	}
}
