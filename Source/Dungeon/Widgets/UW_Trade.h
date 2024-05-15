#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_Trade.generated.h"

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
class DUNGEON_API UUW_Trade : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled)const override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)override;
public:

	//property
private:
	UPROPERTY()UInventoryComponent* OwnerComponent;
	UPROPERTY()UInventoryComponent* TradeComponent;
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

	UPROPERTY(EditDefaultsOnly, Category = "Item")
		TSubclassOf<UUW_InventoryItem> ItemWidgetClass;

public:

	//function
private:
	void ChangeGridSize(int32 InRowSize = 1, int32 InColumnSize = 1);
	UFUNCTION()void Refresh();
	UFUNCTION()void Buy(UItemObject* InObject);
	UFUNCTION()void OnInfoPopup(UItemObject* InObject);
	UFUNCTION()void OffInfoPopup();

protected:
public:
	void Init(UInventoryComponent* InComponent, UUW_InventoryPopup* InPopup);
	void Trade(UInventoryComponent* InComponent);
};

/*
* drag and drop(X)
* 
* click items 
* ->try add
*	->|need more gold|
*	->|no rooms|
* ->add
* 
* drop item 
* ->sell item
*/