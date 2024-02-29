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

		bool green = 0;
		green = OwnerComponent->CanTakeOffEquipment(SlotIndex);

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
	//드롭이 됐을때
	//장비를 갈아입을수있는 상태면 그대로 진행
	//아니라면 드롭물체 제자리로

	DragDropEnter = 0;

	UItemObject* item = nullptr;
	if (InOperation)
	{
		UObject* payload = InOperation->Payload.Get();
		item = Cast<UItemObject>(payload);
		if (!item)return 1;
		if (OwnerComponent->CanTakeOffEquipment(SlotIndex))
		{
			ChangePresetData(SlotIndex, item->GetWeapon());
			Pannel->AddChild(InOperation->DefaultDragVisual.Get());

			UUW_InventoryItem* widget = Cast<UUW_InventoryItem>(InOperation->DefaultDragVisual.Get());
			if (widget)
			{
				widget->OnInventoryItemRemoved.AddUFunction(this, "OnItemRemoved");
			}

			int32 idx = Pannel->GetChildIndex(InOperation->DefaultDragVisual.Get());
			UCanvasPanelSlot* slot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Pannel->GetChildAt(idx));
			slot->SetAlignment(FVector2D(0.5));
			slot->SetAnchors(FAnchors(0.5));
			slot->SetOffsets(FMargin(0));
			slot->SetAutoSize(1);
		}
		else
			OwnerComponent->TryAddItem(item->GetWeapon());
	}

	return 1;
}

bool UUW_InventoryEquipmentSlot::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);

	return 1;
}

FReply UUW_InventoryEquipmentSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return 	UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, FKey("LeftMouseButton")).NativeReply;
}

void UUW_InventoryEquipmentSlot::Refresh()
{
	//UItemObject* item = OwnerComponent->GetPresetItems(SlotIndex);
	//if (item && item->IsRotated())item->Rotate();
	//UMaterialInterface* mat = item == nullptr ? nullptr : item->GetIcon();
	//if (!mat)Icon->SetVisibility(ESlateVisibility::Collapsed);
	//else
	//{
	//	Icon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	//	Icon->SetBrushFromMaterial(mat);
	//}
}

void UUW_InventoryEquipmentSlot::OnItemRemoved(UItemObject* InObject)
{
	OwnerComponent->RemovePreset_Drag(SlotIndex);
}

void UUW_InventoryEquipmentSlot::Init(UInventoryComponent* InComponent, int32 InSlotIndex)
{
	OwnerComponent = InComponent;
	OwnerComponent->OnInventoryPresetChanged.AddDynamic(this, &UUW_InventoryEquipmentSlot::Refresh);
	SlotIndex = InSlotIndex;
}

void UUW_InventoryEquipmentSlot::EquipPreset(int32 InIdx)
{
	OwnerComponent->EquipPreset(InIdx);
}

void UUW_InventoryEquipmentSlot::ChangePresetIndex(int32 InIdx)
{
	OwnerComponent->ChangePresetIndex(InIdx);
}

void UUW_InventoryEquipmentSlot::ChangePresetData(int32 InIdx, AWeapon* InData)
{
	OwnerComponent->ChangePresetData(InIdx, InData);
}
