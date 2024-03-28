#include "Widgets/UW_ConfirmPopup.h"
#include "Global.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UUW_ConfirmPopup::NativeConstruct()
{
	Super::NativeConstruct();

	ConfirmButton->OnClicked.AddDynamic(this, &UUW_ConfirmPopup::OnConfirmButtonClicked);
	CancelButton->OnClicked.AddDynamic(this, &UUW_ConfirmPopup::OnCancelButtonClicked);
}

void UUW_ConfirmPopup::OnConfirmButtonClicked()
{
	OnConfirm.ExecuteIfBound();
	RemoveFromParent();
}

void UUW_ConfirmPopup::OnCancelButtonClicked()
{
	OnCancel.ExecuteIfBound();
	RemoveFromParent();
}

void UUW_ConfirmPopup::Init(FString InDescription)
{
	Description->SetText(FText::FromString(InDescription));
}
