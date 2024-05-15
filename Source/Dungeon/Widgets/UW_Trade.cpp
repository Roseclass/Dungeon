#include "Widgets/UW_Trade.h"
#include "Global.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/DragDropOperation.h"
#include "Kismet/KismetMaterialLibrary.h" 
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "Components/InventoryComponent.h"
#include "Objects/ItemObject.h"
#include "Widgets/UW_InventoryItem.h"
#include "Widgets/UW_InventoryPopup.h"

int32 UUW_Trade::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled)const
{
	int32 result = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	FPaintContext context = FPaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	FVector2D size = USlateBlueprintLibrary::GetLocalSize(Background->GetCachedGeometry());
	FVector2D topleft = USlateBlueprintLibrary::GetLocalTopLeft(Background->GetCachedGeometry());
	FVector2D topright = topleft + FVector2D(size.X, 0);
	FVector2D bottomleft = topleft + FVector2D(0, size.Y);
	FVector2D bottomright = topleft + size;
	FVector2D gap = FVector2D(size.X / RowSize, size.Y / ColumnSize);

	//테두리
	{
		UWidgetBlueprintLibrary::DrawLine(context, topleft, topright, FLinearColor(1, 1, 1, 1));
		UWidgetBlueprintLibrary::DrawLine(context, topleft, bottomleft, FLinearColor(1, 1, 1, 1));
		UWidgetBlueprintLibrary::DrawLine(context, topright, bottomright, FLinearColor(1, 1, 1, 1));
		UWidgetBlueprintLibrary::DrawLine(context, bottomleft, bottomright, FLinearColor(1, 1, 1, 1));
	}

	return result;
}

bool UUW_Trade::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool result = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UUW_InventoryItem* itemWidget = Cast<UUW_InventoryItem>(InOperation->DefaultDragVisual);
	if (itemWidget)itemWidget->DragDropEnd();

	UItemObject* item = nullptr;
	if (InOperation) item = Cast<UItemObject>(InOperation->Payload.Get());
	if (!item)return 1;

	//sell items
	//OwnerComponent->Server_Sell(item->GetEquipment());

	return result;
}

void UUW_Trade::ChangeGridSize(int32 InRowSize, int32 InColumnSize)
{
	RowSize = InRowSize;
	ColumnSize = InColumnSize;
	Grid->GetDynamicMaterial()->SetScalarParameterValue("Row", RowSize);
	Grid->GetDynamicMaterial()->SetScalarParameterValue("Column", ColumnSize);
}

void UUW_Trade::Refresh()
{
	TMap<AEqquipment*, TTuple<int32, int32>> map;
	GridCanvasPanel->ClearChildren();
	CheckNull(TradeComponent);
	TradeComponent->GetAllItems(map);

	FVector2D size = USlateBlueprintLibrary::GetLocalSize(Background->GetCachedGeometry());
	FVector2D gap = FVector2D(size.X / RowSize, size.Y / ColumnSize);

	for (auto i : map)
	{
		UUW_InventoryItem* widget = CreateWidget<UUW_InventoryItem, APlayerController>(GetOwningPlayer(), ItemWidgetClass);
		if (!widget)continue;
		UItemObject* obj = i.Key->GetItemObject();
		widget->Init(gap, obj, OwnerComponent);
		widget->OnLeftClicked.AddUFunction(this, "Buy");
		widget->OnRightClicked.AddUFunction(this, "Buy");
		widget->OnInventoryItemMouseEnter.AddUFunction(this, "OnInfoPopup");
		widget->OnInventoryItemMouseLeave.AddUFunction(this, "OffInfoPopup");
		UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(GridCanvasPanel->AddChild(widget));
		if (!slot)continue;

		//앵커 조절
		int32 x, y;
		obj->GetDimensions(x, y);
		FAnchors anch;
		anch.Minimum = FVector2D(double(i.Value.Key) / double(RowSize), double(i.Value.Value) / double(ColumnSize));
		anch.Maximum = anch.Minimum + FVector2D(double(x) / double(RowSize), double(y) / double(ColumnSize));
		slot->SetAnchors(anch);
		slot->SetOffsets(FMargin(0));
	}
}

void UUW_Trade::Buy(UItemObject* InObject)
{
	//check condition

	//send message

	//add to inv
}

void UUW_Trade::OnInfoPopup(UItemObject* InObject)
{
	Popup->On(InObject);
}

void UUW_Trade::OffInfoPopup()
{
	Popup->Off();
}

void UUW_Trade::Init(UInventoryComponent* InComponent, UUW_InventoryPopup* InPopup)
{
	OwnerComponent = InComponent;
	Popup = InPopup;
}

void UUW_Trade::Trade(UInventoryComponent* InComponent)
{
	TradeComponent = InComponent;
	ChangeGridSize(TradeComponent->GetRows(), TradeComponent->GetColumns());
	Refresh();
}