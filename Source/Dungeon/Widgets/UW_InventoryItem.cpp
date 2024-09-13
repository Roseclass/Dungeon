#include "Widgets/UW_InventoryItem.h"
#include "Global.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/SizeBox.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/DragDropOperation.h"

#include "Components/InventoryComponent.h"
#include "Objects/ItemObject.h"
#include "Objects/Weapon.h"

void UUW_InventoryItem::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

FReply UUW_InventoryItem::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (FKey("LeftMouseButton") == InMouseEvent.GetEffectingButton())
	{
		return 	UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, FKey("LeftMouseButton")).NativeReply;
	}
	else if (FKey("RightMouseButton") == InMouseEvent.GetEffectingButton())
	{
		//OwnerComponent->ChangePresetData(OwnerComponent->GetPresetIndex(), ItemObject);
		//if (OnInventoryItem_Removed.IsBound())
		//	OnInventoryItem_Removed.Broadcast(ItemObject);
		//RemoveFromParent();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UUW_InventoryItem::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (FKey("LeftMouseButton") == InMouseEvent.GetEffectingButton())
	{
		if (OnLeftClicked.IsBound())
			OnLeftClicked.Broadcast(ItemObject);
	}
	else if (FKey("RightMouseButton") == InMouseEvent.GetEffectingButton())
	{
		if (OnRightClicked.IsBound())
			OnRightClicked.Broadcast(ItemObject);
	}

	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UUW_InventoryItem::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	BackgroundBorder->SetBrushColor(FLinearColor(0.5, 0.5, 0.5, 0.2));
	if (OnInventoryItemMouseEnter.IsBound())
		OnInventoryItemMouseEnter.Broadcast(ItemObject);
}

void UUW_InventoryItem::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	BackgroundBorder->SetBrushColor(FLinearColor(0, 0, 0, 0.5));
	if (OnInventoryItemMouseLeave.IsBound())
		OnInventoryItemMouseLeave.Broadcast();
}

void UUW_InventoryItem::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	bDragDetected = 1;
	SetVisibility(ESlateVisibility::HitTestInvisible);

	UCanvasPanel* canvas = Cast<UCanvasPanel>(GetParent());

	WidgetSize = USlateBlueprintLibrary::GetLocalSize(ItemImage->GetCachedGeometry());

	OutOperation = NewObject<UDragDropOperation>(this);
	OutOperation->Payload = ItemObject;
	OutOperation->Pivot = EDragPivot::MouseDown;
	OutOperation->Offset = FVector2D(0, 0);
	OutOperation->DefaultDragVisual = this;

	if (ItemObject && OwnerComponent)
	{
		//TODO::CHECK
		//ItemObject->GetEqquipment()->ChangeVisibility(EItemMode::Inventory);
	}
}

FSlateBrush UUW_InventoryItem::GetIconImage()
{
	FSlateBrush result = UWidgetBlueprintLibrary::MakeBrushFromMaterial(ItemObject->GetIcon(), WidgetSize.X, WidgetSize.Y);
	float tint = bDragDetected ? 0.1 : 1;
	result.TintColor = FLinearColor(tint, tint, tint, 1);
	return result;
}

void UUW_InventoryItem::Init(FVector2D InSize, UItemObject* InObject, UInventoryComponent* InComponent)
{
	ItemObject = InObject;
	ItemImage->BrushDelegate.BindUFunction(this, "GetIconImage");
	ItemImage->SetBrushFromMaterial(ItemObject->GetIcon());

	OwnerComponent = InComponent;
	Refresh();
}

void UUW_InventoryItem::Refresh()
{
	//int32 X, Y;
	//CheckNull(ItemObject);
	//ItemObject->GetDimensions(X, Y);
	//Size = 
	//BackgroundSizeBox->SetWidthOverride(Size.X);
	//BackgroundSizeBox->SetHeightOverride(Size.Y);
	//UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(ItemImage->Slot);
	//slot->SetSize(Size);
}

void UUW_InventoryItem::DragDropEnd()
{
	if (OnInventoryItemRemoved.IsBound())
	{
		OnInventoryItemRemoved.Broadcast(ItemObject);
		OnInventoryItemRemoved.Clear();
	}
}
