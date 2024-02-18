#include "Widgets/UW_Inventory.h"
#include "Global.h"
#include "Components/Border.h"
#include "Components/Button.h"

#include "Widgets/UW_InventoryGrid.h"
#include "Widgets/UW_InventoryEquipmentSlot.h"

void UUW_Inventory::Init(UInventoryComponent* InComponent)
{
	OwnerComponent = InComponent;
	Grid->Init(OwnerComponent);
	WeaponSlot->Init(OwnerComponent, 0);
}
