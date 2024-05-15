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
DECLARE_MULTICAST_DELEGATE_OneParam(FInventoryItemMouseEnter, UItemObject*);
DECLARE_MULTICAST_DELEGATE(FInventoryItemMouseLeave);
DECLARE_MULTICAST_DELEGATE_OneParam(FInventoryItemClicked, UItemObject*);


UCLASS()
class DUNGEON_API UUW_InventoryItem : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized()override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent)override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)override;
public:

	//property
private:
	FVector2D WidgetSize;//maintain size while drag and drop
	UPROPERTY()UItemObject* ItemObject;
	UPROPERTY()UInventoryComponent* OwnerComponent;
	bool bDragDetected;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UBorder* BackgroundBorder;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* ItemImage;

public:
	FInventoryItemRemoved OnInventoryItemRemoved;
	FInventoryItemMouseEnter OnInventoryItemMouseEnter;
	FInventoryItemMouseLeave OnInventoryItemMouseLeave;
	FInventoryItemClicked OnLeftClicked;
	FInventoryItemClicked OnRightClicked;
	//function
private:
	UFUNCTION(BlueprintCallable, BlueprintPure) FSlateBrush GetIconImage();
protected:
public:
	void Init(FVector2D InSize, UItemObject* InObject, UInventoryComponent* InComponent);
	void Refresh();
	void DragDropEnd();
	FORCEINLINE bool CheckItem(UItemObject* InItemObject)const { return ItemObject == InItemObject; }
	FORCEINLINE FVector2D GetWidgetSize()const { return WidgetSize; }
};
