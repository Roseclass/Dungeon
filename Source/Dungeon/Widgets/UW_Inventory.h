#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_Inventory.generated.h"

/**
 * 
 */

class UBorder;
class UButton;
class UUW_InventoryGrid;
class UUW_InventoryEquipmentSlot;
class UInventoryComponent;

UCLASS()
class DUNGEON_API UUW_Inventory : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)override;
public:

	//property
private:
	UInventoryComponent* OwnerComponent;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UBorder* Background;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Preset0;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Preset1;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_InventoryGrid* Grid;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_InventoryEquipmentSlot* WeaponSlot;

public:

	//function
private:
protected:
public:
	void Init(UInventoryComponent* InComponent);
};
