#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_InventoryItem.generated.h"

/**
 * 
 */

class UBorder;
class USizeBox;
class UImage;
class UItemObject;
class UInventoryComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FInventoryItemRemoved, UItemObject*);

UCLASS()
class DUNGEON_API UUW_InventoryItem : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized()override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent)override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)override;
public:

	//property
private:
	FVector2D WidgetSize;//드래그 드롭때 크기를 유지하기 위함
	UItemObject* ItemObject;
	UInventoryComponent* OwnerComponent;
	bool bDragDetected;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UBorder* BackgroundBorder;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* ItemImage;

public:
	FInventoryItemRemoved OnInventoryItemRemoved;
	//function
private:
	UFUNCTION(BlueprintCallable, BlueprintPure) FSlateBrush GetIconImage();
protected:
public:
	void Init(FVector2D InSize, UItemObject* InObject, UInventoryComponent* InComponent);
	void Refresh();
	void DragDropEnd();
	FORCEINLINE bool CheckItem(UItemObject* InItemObject) { return ItemObject == InItemObject; }
};
