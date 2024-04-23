#include "Widgets/UW_Chat.h"
#include "Global.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

#include "DungeonPlayerController.h"

void UUW_Chat::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ChatBox->OnTextCommitted.AddDynamic(this, &UUW_Chat::OnTextCommitted);
}

void UUW_Chat::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod != ETextCommit::Type::OnEnter)return;

	ChatBox->SetVisibility(ESlateVisibility::Collapsed);
	ChatBox->SetText(FText());

	ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(GetOwningPlayer());
	if (controller)
	{
		controller->Server_SendChat(Text);

		// release focus
		FInputModeGameOnly game;
		controller->SetInputMode(game);

		// for OnSetDestinationPressed
		FInputModeGameAndUI f; f.SetHideCursorDuringCapture(0);
		controller->SetInputMode(f);
	}
}

void UUW_Chat::OnMessageUpdated(const FText& Text, const FLinearColor& Color)
{
	UTextBlock* message = NewObject<UTextBlock>(GetOwningPlayer(), UTextBlock::StaticClass());
	if (!message)
	{
		CLog::Print("UUW_Chat::OnMessageUpdated");
		return;
	}

	message->SetText(Text);
	message->SetColorAndOpacity(Color);
	MessageBox->AddChild(message);
}

void UUW_Chat::OnChat()
{
	ChatBox->SetVisibility(ESlateVisibility::Visible);
	ChatBox->SetFocus();
}
