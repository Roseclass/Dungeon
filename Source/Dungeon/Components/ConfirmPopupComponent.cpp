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
	UConfirmPopupComponent* confirm = CHelpers::GetComponent<UConfirmPopupComponent>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (confirm)
	{
		ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(GetOwner());
		confirm->UpdateState(controller, EPopupState::Confirm);
	}
	else CLog::Print("UConfirmPopupComponent::Server_Confirm_Implementation", -1, 10, FColor::Red);
}

void UConfirmPopupComponent::Server_Cancel_Implementation()
{
	UConfirmPopupComponent* confirm = CHelpers::GetComponent<UConfirmPopupComponent>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (confirm)
	{
		ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(GetOwner());
		confirm->UpdateState(controller, EPopupState::Cancel);
	}
	else CLog::Print("UConfirmPopupComponent::Server_Confirm_Implementation", -1, 10, FColor::Red);
}

void UConfirmPopupComponent::Client_CreatePopup_Implementation(const FString& InString)
{
	ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(GetOwner());
	PopupWidget = CreateWidget<UUW_ConfirmPopup, ADungeonPlayerController>(controller, PopupClass);
	PopupWidget->OnConfirm.BindUFunction(this, "Server_Confirm");
	PopupWidget->OnCancel.BindUFunction(this, "Server_Cancel");
	PopupWidget->Init(InString);
	PopupWidget->AddToViewport();
}

void UConfirmPopupComponent::SendPopupAllPlayers(FString InString, TFunction<bool()> FinishedEvent)
{
	// send popup every clients
	int32 max = UGameplayStatics::GetNumPlayerControllers(GetWorld());
	for (int32 i = 0; i < max; ++i)
	{
		ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), i));
		States.FindOrAdd(controller) = EPopupState::Wait;
		UConfirmPopupComponent* confirm = CHelpers::GetComponent<UConfirmPopupComponent>(controller);
		if (confirm)confirm->Client_CreatePopup(InString);
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
		//executefunction;
		ReserveFinishedEvent();
		ReserveFinishedEvent = nullptr;
	}
}