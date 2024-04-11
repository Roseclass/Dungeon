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
class AEqquipment;
class UItemObject;
class UUW_InventoryItem;

enum class EItemType : uint8;

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

public:

	//property
private:
	UPROPERTY()UInventoryComponent* OwnerComponent;
	bool DragDropEnter;

	FVector2D Size = FVector2D(1, 1);

	EItemType SlotType;
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
	UFUNCTION()void OnItemRemoved(UItemObject* InObject);
	UFUNCTION()void Refresh(UItemObject* InObject = nullptr);
protected:
public:
	void Init(UInventoryComponent* InComponent, EItemType NewType, FVector2D NewSize);
	void ChangeEquippedData(int32 InIdx, AEqquipment* InData);
};
/*
* ������ ���� ��� ���������� ���� ������ �ʿ���
* � ������ � �κ��丮 ������Ʈ �����͸� ��������� �����������
* �����ƴٸ� ������ ����ÿ����� �ܰ� �����Ͱ� ���ŵǰ� ���ε�����
*/