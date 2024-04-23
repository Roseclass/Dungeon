#include "Widgets/UW_Lobby.h"
#include "Global.h"

#include "LobbyPlayerController.h"
#include "Widgets/UW_LobbyCharacter.h"
#include "Widgets/UW_LobbySession.h"

void UUW_Lobby::NativeConstruct()
{
	Super::NativeConstruct();
	SessionWidget->OnNewButtonClickedDelegate.BindUFunction(this, "OnSessionWidgetNewButtonClicked");
	CharacterWidget->OnConfirmButtonClickedDelegate.BindUFunction(this, "OnCharacterWidgetConfirmButtonClicked");
	CharacterWidget->OnCancelButtonClickedDelegate.BindUFunction(this, "OnCharacterWidgetCancelButtonClicked");
}

void UUW_Lobby::OnSessionWidgetNewButtonClicked()
{
	ALobbyPlayerController* controller = Cast<ALobbyPlayerController>(GetOwningPlayer());
	CheckNull(controller);
	CheckNull(controller->GetCurrentCharacter());

	CharacterWidget->SetVisibility(ESlateVisibility::Visible);
	CharacterWidget->SetTarget(controller->GetCurrentCharacter());
}

void UUW_Lobby::OnCharacterWidgetConfirmButtonClicked()
{
	SessionWidget->SetVisibility(ESlateVisibility::Visible);
	SessionWidget->Refresh();
}

void UUW_Lobby::OnCharacterWidgetCancelButtonClicked()
{
	SessionWidget->SetVisibility(ESlateVisibility::Visible);
	SessionWidget->Refresh();
}
