#include "Widgets/UW_Lobby.h"
#include "Global.h"

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
	CharacterWidget->SetVisibility(ESlateVisibility::Visible);
}

void UUW_Lobby::OnCharacterWidgetConfirmButtonClicked()
{
	SessionWidget->SetVisibility(ESlateVisibility::Visible);
}

void UUW_Lobby::OnCharacterWidgetCancelButtonClicked()
{
	SessionWidget->SetVisibility(ESlateVisibility::Visible);
}
