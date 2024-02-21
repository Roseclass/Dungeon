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
		if (!item)return 1;
	}

	//인벤토리 밖으로 드롭
	if (mousePos.X < 0.7)
	{
		AActor* actor = OwnerComponent->GetOwner();
		FVector start = actor->GetActorLocation() + actor->GetActorForwardVector() * 150.0f;
		TArray<AActor*> arr; FHitResult hit;
		if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), start, start + FVector(0, 0, -1000), ETraceTypeQuery::TraceTypeQuery1, 0, arr, EDrawDebugTrace::None, hit, 1))
		{
			item->GetWeapon()->SetActorLocation(hit.Location);
			item->GetWeapon()->SetPickableMode();
			item->GetWeapon()->SetActorRotation(FRotator());
		}		
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
		OwnerComponent->TryAddItem(item);
	}	

	return result;
}

void UUW_Inventory::Init(UInventoryComponent* InComponent)
{
	OwnerComponent = InComponent;
	Grid->Init(OwnerComponent);
	WeaponSlot->Init(OwnerComponent, 0);
}
