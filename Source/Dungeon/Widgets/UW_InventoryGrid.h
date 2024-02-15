#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_InventoryGrid.generated.h"

/**
 * 
 */

class UBorder;
class USlateBrushAsset;
class UInventoryComponent;

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
	UInventoryComponent* OwnerComponent;
	bool DrawBox;
	int32 BoxLeft = 0;
	int32 BoxTop = 0;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UBorder* Background;

	UPROPERTY(EditDefaultsOnly, Category = "Size", meta = (ClampMin = 1.00))
		int32 RowSize = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Size", meta = (ClampMin = 1.00))
		int32 ColumnSize = 1;

	UPROPERTY(EditDefaultsOnly, Category = "DragDrop")
		USlateBrushAsset* SlateBrush;

public:

	//function
private:
protected:
public:

};
