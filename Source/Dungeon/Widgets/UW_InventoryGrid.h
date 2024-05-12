#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_InventoryGrid.generated.h"

/**
 * 
 */

class UBorder;
class UCanvasPanel;
class USlateBrushAsset;
class UInventoryComponent;
class UUW_InventoryItem;
class UUW_InventoryPopup;

UCLASS()
class DUNGEON_API UUW_InventoryGrid : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled)const override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)override;
public:

	//property
private:
	UPROPERTY()UInventoryComponent* OwnerComponent;
	UPROPERTY()UUW_InventoryPopup* Popup;

	bool DrawBox;
	int32 BoxLeft = 0;
	int32 BoxRight = 0;
	int32 BoxTop = 0;
	int32 BoxBottom = 0;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UBorder* Background;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UBorder* Grid;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UCanvasPanel* GridCanvasPanel;

	UPROPERTY(EditDefaultsOnly, Category = "Size", meta = (ClampMin = 1.00))
		int32 RowSize = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Size", meta = (ClampMin = 1.00))
		int32 ColumnSize = 1;

	UPROPERTY(EditDefaultsOnly, Category = "DragDrop")
		USlateBrushAsset* SlateBrush;

	UPROPERTY(EditDefaultsOnly, Category = "DragDrop")
		TSubclassOf<UUW_InventoryItem> ItemWidgetClass;

public:

	//function
private:
	void ChangeGridSize(int32 InRowSize = 1, int32 InColumnSize = 1);
	UFUNCTION()void Refresh();
	UFUNCTION()void OnItemRemoved(UItemObject* InObject);
	UFUNCTION()void OnInfoPopup(UItemObject* InObject);
	UFUNCTION()void OffInfoPopup();
	UFUNCTION()FEventReply OnGridBorderMouseButtonDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent);
	void DropItem(UItemObject* InObject);

protected:
public:
	void Init(UInventoryComponent* InComponent, UUW_InventoryPopup* InPopup);
	void ChangeOwnerComponent(UInventoryComponent* InComponent);
};
