#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_Inventory.generated.h"

/**
 * 
 */

class UBorder;
class UButton;
class UCanvasPanel;
class UUW_InventoryGrid;
class UUW_InventoryEquipmentSlot;
class UUW_InventoryPopup;
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
		UCanvasPanel* BackgroundPanel;

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

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_InventoryEquipmentSlot* HelmSlot;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_InventoryEquipmentSlot* UpperBodySlot;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_InventoryEquipmentSlot* LowerBodySlot;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_InventoryEquipmentSlot* GlovesSlot;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_InventoryEquipmentSlot* BootsSlot;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_InventoryPopup* Popup;

public:

	//function
private:
protected:
public:
	void Init(UInventoryComponent* InComponent);
};

/*
* ������ � Ÿ���� ������ �����������
* Ÿ���� ������ ��� �۵��� ���� �����ε� ���� �ּ¿����� �����ʿ�������
*/