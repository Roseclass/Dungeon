#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_InventoryEquipmentSlot.generated.h"

/**
 * 
 */

class UBorder;
class UCanvasPanel;
class USlateBrushAsset;
class UInventoryComponent;
class UItemObject;
class UUW_InventoryItem;

UCLASS()
class DUNGEON_API UUW_InventoryEquipmentSlot : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;
	//virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)override;
public:

	//property
private:
	UInventoryComponent* OwnerComponent;
	int32 SlotIndex;
	bool DragDropEnter;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UBorder* Background;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UCanvasPanel* Pannel;

	UPROPERTY(EditDefaultsOnly, Category = "DragDrop")
		USlateBrushAsset* SlateBrush;

	UPROPERTY(EditDefaultsOnly, Category = "DragDrop")
		TSubclassOf<UUW_InventoryItem> ItemWidgetClass;

public:

	//function
private:
	UFUNCTION()void Refresh();
	UFUNCTION()void OnItemRemoved(UItemObject* InObject);
protected:
public:
	void Init(UInventoryComponent* InComponent, int32 InSlotIndex);
	void EquipPreset(int32 InIdx);
	void ChangePresetIndex(int32 InIdx);
	void ChangePresetData(int32 InIdx, UItemObject* InData);
};
