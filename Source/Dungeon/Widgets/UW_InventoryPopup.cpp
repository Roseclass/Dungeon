#include "Widgets/UW_InventoryPopup.h"
#include "Global.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/CanvasPanelSlot.h"

#include "Objects/ItemObject.h"

void UUW_InventoryPopup::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (IsVisible())
	{
		//find cursor loaction
		FGeometry geo = UWidgetLayoutLibrary::GetViewportWidgetGeometry(GetWorld());
		FVector2D mousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
		mousePos /= geo.GetLocalSize();
		UCanvasPanelSlot* slot = UWidgetLayoutLibrary::SlotAsCanvasSlot(this);
		FVector2D anchSize = slot->GetAnchors().Maximum - slot->GetAnchors().Minimum;
		slot->SetAnchors(FAnchors(mousePos.X, mousePos.Y - anchSize.Y, mousePos.X + anchSize.X, mousePos.Y));
	}
}

void UUW_InventoryPopup::On(UItemObject* InObject)
{
	const FItemStatusData& data = InObject->GetEqquipment()->GetItemStatus();

	//set name
	Name->SetText(FText::FromString(data.GetName()));

	int32 damage = data.GetFinalDamage();
	Damage->SetText(FText::FromString(
		FString::Printf(TEXT("데미지 : %i"), damage)
		));

	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UUW_InventoryPopup::Off()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
