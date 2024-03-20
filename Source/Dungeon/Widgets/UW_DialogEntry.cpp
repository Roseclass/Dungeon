#include "Widgets/UW_DialogEntry.h"
#include "Global.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UUW_DialogEntry::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ReplyButton->OnHovered.AddDynamic(this, &UUW_DialogEntry::OnReplyButtonHover);
	ReplyButton->OnUnhovered.AddDynamic(this, &UUW_DialogEntry::OnReplyButtonUnhover);
	ReplyButton->OnClicked.AddDynamic(this, &UUW_DialogEntry::OnReplyButtonClicked);
}

void UUW_DialogEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UDialogReplyObject* temp = Cast<UDialogReplyObject>(ListItemObject);
	CheckNull(temp);
	DialogReplyObject = temp;
	ReplyText->SetText(DialogReplyObject->GetText());
}

void UUW_DialogEntry::OnReplyButtonHover()
{
	ReplyText->SetColorAndOpacity(HoveredColor);
}

void UUW_DialogEntry::OnReplyButtonUnhover()
{
	ReplyText->SetColorAndOpacity(UnhoveredColor);
}

void UUW_DialogEntry::OnReplyButtonClicked()
{
	if (DialogReplyObject->OnClicked.IsBound())
		DialogReplyObject->OnClicked.Broadcast(DialogReplyObject);
}
