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
	FVector2D TileSize;
	UItemObject* ItemObject;
	UInventoryComponent* OwnerComponent;
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
	FORCEINLINE bool CheckItem(UItemObject* InItemObject) { return ItemObject == InItemObject; }
};
