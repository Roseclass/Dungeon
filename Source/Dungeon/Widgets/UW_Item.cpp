#include "Widgets/UW_Item.h"
#include "Global.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"

void UUW_Item::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UUW_Item::SetName(FString NewName)
{
	Name->SetText(FText::FromString(NewName));
}

void UUW_Item::StartCursorOver()
{
	Background->SetBrushColor(FLinearColor(1, 1, 1, 0.5));
	Name->SetColorAndOpacity(FLinearColor::Black);
}

void UUW_Item::EndCursorOver()
{
	Background->SetBrushColor(FLinearColor(0, 0, 0, 0.5));
	Name->SetColorAndOpacity(FLinearColor::White);
}
