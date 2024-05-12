#include "Widgets/UW_InventoryEquipmentSlot.h"
#include "Global.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/DragDropOperation.h"

#include "Components/InventoryComponent.h"
#include "Objects/ItemObject.h"
#include "Objects/Weapon.h"
#include "Widgets/UW_InventoryItem.h"
#include "Widgets/UW_InventoryPopup.h"

int32 UUW_InventoryEquipmentSlot::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 result = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	FPaintContext context = FPaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	if (UWidgetBlueprintLibrary::IsDragDropping() && DragDropEnter)
	{
		UDragDropOperation* operation = UWidgetBlueprintLibrary::GetDragDroppingContent();
		UObject* payload = operation->Payload.Get();
		UItemObject* itemobject = nullptr;
		if (operation) itemobject = Cast<UItemObject>(payload);

		// draw green or red box on widget
		bool green = 0;
		green = OwnerComponent->CanTakeOffEquipment(int32(SlotType)) && itemobject->GetType() == SlotType;

		FVector2D topleft = USlateBlueprintLibrary::GetLocalTopLeft(Background->GetCachedGeometry());
		FVector2D size = USlateBlueprintLibrary::GetLocalSize(Background->GetCachedGeometry());

		UWidgetBlueprintLibrary::DrawBox(
			context,
			topleft,
			topleft + size,
			SlateBrush,
			green ? FLinearColor(0, 1, 0, 0.5) : FLinearColor(1, 0, 0, 0.5)
		);
	}
	return result;
}

void UUW_InventoryEquipmentSlot::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);

	DragDropEnter = 1;
}

void UUW_InventoryEquipmentSlot::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	DragDropEnter = 0;
}

bool UUW_InventoryEquipmentSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	DragDropEnter = 0;

	// execute DragDropEnd events
	UUW_InventoryItem* itemWidget = Cast<UUW_InventoryItem>(InOperation->DefaultDragVisual);
	if (itemWidget)itemWidget->DragDropEnd();

	UItemObject* item = nullptr;
	if (InOperation)
	{
		UObject* payload = InOperation->Payload.Get();
		item = Cast<UItemObject>(payload);
		if (!item)return 1;

		// can take off equipment? same type?
		if (OwnerComponent->CanTakeOffEquipment(int32(SlotType)) && item->GetType() == SlotType)
			// equip
			ChangeEquippedData(int32(SlotType), item->GetEqquipment());		
		else // return to inv
			OwnerComponent->Server_TryAddItem(item->GetEqquipment());
	}

	return 1;
}

bool UUW_InventoryEquipmentSlot::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);

	return 1;
}

void UUW_InventoryEquipmentSlot::OnItemRemoved(UItemObject* InObject)
{
	// refresh
	Refresh();

	// reset component data
	OwnerComponent->Server_RemoveEquipped_Drag(int32(SlotType));
}

void UUW_InventoryEquipmentSlot::Refresh(UItemObject* InObject)
{
	// clear child
	if (InObject == nullptr)
	{
		Pannel->ClearChildren();
		return;
	}

	// create child widget
	UUW_InventoryItem* widget = CreateWidget<UUW_InventoryItem, APlayerController>(GetOwningPlayer(), ItemWidgetClass);
	if (widget)
	{
		int32 x, y;
		InObject->GetDimensions(x, y);

		widget->Init(FVector2D(0,0), InObject, OwnerComponent);
		widget->OnInventoryItemRemoved.AddUFunction(this, "OnItemRemoved");
		widget->OnInventoryItemMouseEnter.AddUFunction(this, "OnInfoPopup");
		widget->OnInventoryItemMouseLeave.AddUFunction(this, "OffInfoPopup");
		UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(Pannel->AddChild(widget));

		FAnchors anch;
		anch.Minimum = FVector2D(0, 0);
		anch.Maximum = FVector2D(1, 1);

		slot->SetAlignment(FVector2D(0.5));
		slot->SetAnchors(anch);
		slot->SetOffsets(FMargin(0));
	}
}

void UUW_InventoryEquipmentSlot::OnInfoPopup(UItemObject* InObject)
{
	Popup->On(InObject);
}

void UUW_InventoryEquipmentSlot::OffInfoPopup()
{
	Popup->Off();
}

void UUW_InventoryEquipmentSlot::Init(UInventoryComponent* InComponent, UUW_InventoryPopup* InPopup, EItemType NewType, FVector2D NewSize)
{
	OwnerComponent = InComponent;
	Popup = InPopup;
	SlotType = NewType;
	Size = NewSize;

	if (OwnerComponent)
	{
		if (!OwnerComponent->OnInventoryEquippedDataChanged.IsValidIndex(int32(SlotType)))
		{
			CLog::Print("UUW_InventoryEquipmentSlot::Init IsNOTValidIndex", -1, 10, FColor::Red);
			return;
		}
		OwnerComponent->OnInventoryEquippedDataChanged[int32(SlotType)].AddDynamic(this, &UUW_InventoryEquipmentSlot::Refresh);
	}

}

void UUW_InventoryEquipmentSlot::ChangeEquippedData(int32 InIdx, AEqquipment* InData)
{
	OwnerComponent->Server_ChangeEquippedData(InIdx, InData);
}
