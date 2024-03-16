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

	//�κ��丮 ������ ���
	if (mousePos.X < 0.7)
	{
		UUW_InventoryItem* itemWidget = Cast<UUW_InventoryItem>(InOperation->DefaultDragVisual);
		if (itemWidget)itemWidget->DragDropEnd();
		AActor* actor = OwnerComponent->GetOwner();
		FVector start = actor->GetActorLocation() + actor->GetActorForwardVector() * 150.0f;
		TArray<AActor*> arr; FHitResult hit;
		if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), start, start + FVector(0, 0, -1000), ETraceTypeQuery::TraceTypeQuery1, 0, arr, EDrawDebugTrace::None, hit, 1))
		{
			item->GetWeapon()->SetItemLocation(hit.Location);
			item->GetWeapon()->SetItemRotation(FRotator());
			item->GetWeapon()->ChangeVisibility(EItemMode::Loot);
		}		
		return result;
	}
	UCanvasPanelSlot* slot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Grid);
	FAnchors anc = slot->GetAnchors();
	
	//�׸��� �ȿ� ���
	if (anc.Minimum.X < mousePos.X && mousePos.X < anc.Maximum.X &&
		anc.Minimum.Y < mousePos.Y && mousePos.Y < anc.Maximum.Y)
		return result;
	//�׸��� �ٱ����� ���
	else
	{
		UUW_InventoryItem* itemWidget = Cast<UUW_InventoryItem>(InOperation->DefaultDragVisual);
		if (itemWidget)itemWidget->DragDropEnd();
		OwnerComponent->Server_TryAddItem(item->GetWeapon());
	}	

	return result;
}

void UUW_Inventory::Init(UInventoryComponent* InComponent)
{
	OwnerComponent = InComponent;
	Grid->Init(OwnerComponent);
	WeaponSlot->Init(OwnerComponent, 0);
}
