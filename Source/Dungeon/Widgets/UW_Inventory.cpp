#include "Widgets/UW_Inventory.h"
#include "Global.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetLayoutLibrary.h"

#include "Components/InventoryComponent.h"
#include "Objects/Weapon.h"
#include "Objects/ItemObject.h"
#include "Widgets/UW_InventoryGrid.h"
#include "Widgets/UW_InventoryEquipmentSlot.h"
#include "Widgets/UW_InventoryItem.h"
#include "Widgets/UW_InventoryPopup.h"

bool UUW_Inventory::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool result = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	FVector2D mousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	mousePos /= BackgroundPanel->GetCachedGeometry().GetLocalSize();

	UItemObject* item = nullptr;
	if (InOperation)
	{
		UObject* payload = InOperation->Payload.Get();
		item = Cast<UItemObject>(payload);
		if (!item)return result;
	}

	//인벤토리 밖으로 드롭
	if (mousePos.X < 0.7)
	{
		UUW_InventoryItem* itemWidget = Cast<UUW_InventoryItem>(InOperation->DefaultDragVisual);
		if (itemWidget)itemWidget->DragDropEnd();
		item->GetEqquipment()->ChangeVisibility(EItemMode::Loot);
		Popup->Off();
		return result;
	}
	UCanvasPanelSlot* slot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Grid);
	FAnchors anc = slot->GetAnchors();
	
	//그리드 안에 드롭
	if (anc.Minimum.X < mousePos.X && mousePos.X < anc.Maximum.X &&
		anc.Minimum.Y < mousePos.Y && mousePos.Y < anc.Maximum.Y)
		return result;
	//그리드 바깥으로 드롭
	else
	{
		UUW_InventoryItem* itemWidget = Cast<UUW_InventoryItem>(InOperation->DefaultDragVisual);
		if (itemWidget)itemWidget->DragDropEnd();
		OwnerComponent->Server_TryAddItem(item->GetEqquipment());
	}	

	return result;
}

void UUW_Inventory::Init(UInventoryComponent* InComponent)
{
	OwnerComponent = InComponent;
	Grid->Init(OwnerComponent, Popup);
	WeaponSlot->Init(OwnerComponent, Popup, EItemType::Weapon, FVector2D(2, 4));
	HelmSlot->Init(OwnerComponent, Popup, EItemType::Helms, FVector2D(2, 2));
	UpperBodySlot->Init(OwnerComponent, Popup, EItemType::UpperBody, FVector2D(2, 2));
	LowerBodySlot->Init(OwnerComponent, Popup, EItemType::LowerBody, FVector2D(2, 2));
	GlovesSlot->Init(OwnerComponent, Popup, EItemType::Gloves, FVector2D(2, 2));
	BootsSlot->Init(OwnerComponent, Popup, EItemType::Boots, FVector2D(2, 2));
}
