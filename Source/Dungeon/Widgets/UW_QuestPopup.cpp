#include "Widgets/UW_QuestPopup.h"
#include "Global.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UUW_QuestPopup::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ConfirmButton->OnClicked.AddDynamic(this, &UUW_QuestPopup::OnConfirmButtonClicked);
	CancelButton->OnClicked.AddDynamic(this, &UUW_QuestPopup::OnCancelButtonClicked);
}

void UUW_QuestPopup::OnConfirmButtonClicked()
{
	OnConfirm.ExecuteIfBound();
}

void UUW_QuestPopup::OnCancelButtonClicked()
{
	OnCancel.ExecuteIfBound();
}

void UUW_QuestPopup::Init(FString InDescription)
{
	Description->SetText(FText::FromString(InDescription));
}
