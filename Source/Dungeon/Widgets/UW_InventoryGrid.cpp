#include "Widgets/UW_InventoryGrid.h"
#include "Global.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"

int32 UUW_InventoryGrid::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled)const
{
	int32 result = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	FPaintContext context = FPaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	FVector2D size = USlateBlueprintLibrary::GetLocalSize(Background->GetCachedGeometry());
	FVector2D topleft = USlateBlueprintLibrary::GetLocalTopLeft(Background->GetCachedGeometry());
	FVector2D topright = topleft + FVector2D(size.X, 0);
	FVector2D bottomleft = topleft + FVector2D(0, size.Y);
	FVector2D bottomright = topleft + size;
	FVector2D gap = FVector2D(size.X / RowSize, size.Y / ColumnSize);

	//테두리
	{
		UWidgetBlueprintLibrary::DrawLine(context, topleft, topright, FLinearColor(1, 1, 1, 1));
		UWidgetBlueprintLibrary::DrawLine(context, topleft, bottomleft, FLinearColor(1, 1, 1, 1));
		UWidgetBlueprintLibrary::DrawLine(context, topright, bottomright, FLinearColor(1, 1, 1, 1));
		UWidgetBlueprintLibrary::DrawLine(context, bottomleft, bottomright, FLinearColor(1, 1, 1, 1));
	}

	//격자
	{
		for (int32 i = 1; i < RowSize; ++i)
		{
			FVector2D top = topleft;
			top.X += gap.X * i;
			FVector2D bottom = bottomright;
			bottom.X = top.X;
			UWidgetBlueprintLibrary::DrawLine(context, top, bottom, FLinearColor(1, 1, 1, 1));
		}

		for (int32 i = 1; i < ColumnSize; ++i)
		{
			FVector2D left = topleft;
			left.Y += gap.Y * i;
			FVector2D right = bottomright;
			right.Y = left.Y;
			UWidgetBlueprintLibrary::DrawLine(context, left, right, FLinearColor(1, 1, 1, 1));
		}
	}

	//드래그 감지
	if (UWidgetBlueprintLibrary::IsDragDropping() && DrawBox && OwnerComponent)
	{
		//UDragDropOperation* operation = UWidgetBlueprintLibrary::GetDragDroppingContent();
		//UCItemObject* itemobject = nullptr;
		//if (operation) itemobject = Cast<UCItemObject>(operation->Payload.Get());
		bool green = 0;
		//int32 idx = -1;
		//idx = OwnerComponent->TileToIndex(DraggedItemTopLeftTile.Key, DraggedItemTopLeftTile.Value);
		//green = OwnerComponent->IsRoomAvailable(itemobject, idx);
		//
		//int32 X, Y;
		//itemobject->GetDimensions(X, Y);
		//UCanvasPanelSlot* text = UWidgetLayoutLibrary::SlotAsCanvasSlot(TextBorder);

		FVector2D start = FVector2D(topleft.X + (gap.X * BoxLeft), topleft.Y + (gap.Y * BoxTop));
		FVector2D end = start + FVector2D(5) * gap;//오브젝트 사이즈로 변경 필요

		UWidgetBlueprintLibrary::DrawBox(
			context,
			start,
			end,
			SlateBrush,
			green ? FLinearColor(0.25, 1, 0, 0.25) : FLinearColor(1, 0, 0, 0.25)
		);
	}

	return result;
}

void UUW_InventoryGrid::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
}

void UUW_InventoryGrid::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
}

bool UUW_InventoryGrid::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool result = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	return result;
}

bool UUW_InventoryGrid::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool result = Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
	return result;
}
