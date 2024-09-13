#include "Widgets/UW_InventoryGrid.h"
#include "Global.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/DragDropOperation.h"
#include "Kismet/KismetMaterialLibrary.h" 
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "Components/InventoryComponent.h"
#include "Objects/ItemObject.h"
#include "Widgets/UW_InventoryItem.h"
#include "Widgets/UW_InventoryPopup.h"

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

	//�׵θ�
	{
		UWidgetBlueprintLibrary::DrawLine(context, topleft, topright, FLinearColor(1, 1, 1, 1));
		UWidgetBlueprintLibrary::DrawLine(context, topleft, bottomleft, FLinearColor(1, 1, 1, 1));
		UWidgetBlueprintLibrary::DrawLine(context, topright, bottomright, FLinearColor(1, 1, 1, 1));
		UWidgetBlueprintLibrary::DrawLine(context, bottomleft, bottomright, FLinearColor(1, 1, 1, 1));
	}

	////����
	//{
	//	for (int32 i = 1; i < RowSize; ++i)
	//	{
	//		FVector2D top = topleft;
	//		top.X += gap.X * i;
	//		FVector2D bottom = bottomright;
	//		bottom.X = top.X;
	//		UWidgetBlueprintLibrary::DrawLine(context, top, bottom, FLinearColor(1, 1, 1, 1));
	//	}

	//	for (int32 i = 1; i < ColumnSize; ++i)
	//	{
	//		FVector2D left = topleft;
	//		left.Y += gap.Y * i;
	//		FVector2D right = bottomright;
	//		right.Y = left.Y;
	//		UWidgetBlueprintLibrary::DrawLine(context, left, right, FLinearColor(1, 1, 1, 1));
	//	}
	//}

	//�巡�� ����
	if (UWidgetBlueprintLibrary::IsDragDropping() && DrawBox && OwnerComponent)
	{
		UDragDropOperation* operation = UWidgetBlueprintLibrary::GetDragDroppingContent();
		UItemObject* itemObject = nullptr;
		if (operation) itemObject = Cast<UItemObject>(operation->Payload.Get());
		bool green = 0;

		int32 idx = OwnerComponent->TileToIndex(BoxLeft, BoxTop);
		green = OwnerComponent->IsRoomGreen(itemObject->GetEqquipment(), idx);

		FVector2D start = FVector2D(topleft.X + (gap.X * BoxLeft), topleft.Y + (gap.Y * BoxTop));
		FVector2D boxsize = FVector2D(gap.X * (BoxRight - BoxLeft), gap.Y * (BoxBottom - BoxTop));

		UWidgetBlueprintLibrary::DrawBox(
			context,
			start,
			boxsize,
			SlateBrush,
			green ? FLinearColor(0.25, 1, 0, 0.25) : FLinearColor(1, 0, 0, 0.25)
		);
	}

	return result;
}

void UUW_InventoryGrid::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
	DrawBox = 1;
}

void UUW_InventoryGrid::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	DrawBox = 0;
}

bool UUW_InventoryGrid::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool result = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UUW_InventoryItem* itemWidget = Cast<UUW_InventoryItem>(InOperation->DefaultDragVisual);
	if (itemWidget)itemWidget->DragDropEnd();

	UItemObject* item = nullptr;
	if (InOperation) item = Cast<UItemObject>(InOperation->Payload.Get());
	if (!item)return 1;

	if (OwnerComponent)
	{
		int32 idx = OwnerComponent->TileToIndex(BoxLeft, BoxTop);
		bool vacant = OwnerComponent->IsRoomGreen(item->GetEqquipment(), idx);
		if (vacant)
		{
			OwnerComponent->Server_AddItemAt(item->GetEqquipment()->GetUniqueID(), idx);
		}
		else
		{
			//TODO::TEST
			/*if (!OwnerComponent->IsRoomAvailable(item->GetEqquipment()))DropItem(item);
			else */OwnerComponent->Server_TryAddItem(item->GetEqquipment()->GetUniqueID());
		}
	}

	return result;
}

bool UUW_InventoryGrid::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool result = Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);

	FVector2D mousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());

	FVector2D widgetPos = Background->GetCachedGeometry().GetAbsolutePosition();
	FVector2D widgetSize = Background->GetCachedGeometry().GetAbsoluteSize();

	FVector2D topLeft, bottomRight, gap;
	USlateBlueprintLibrary::AbsoluteToViewport(GetWorld(), widgetPos, gap, topLeft);
	USlateBlueprintLibrary::AbsoluteToViewport(GetWorld(), widgetPos + widgetSize, gap, bottomRight);
	gap = bottomRight - topLeft;
	gap.X /= RowSize;
	gap.Y /= ColumnSize;

	int32 x, y;
	UItemObject* item = Cast<UItemObject>(InOperation->Payload);
	if (!item)return 1;
	item->GetDimensions(x, y);
	
	FVector2D loc = mousePos - topLeft;
	BoxLeft = loc.X / gap.X;
	BoxRight = BoxLeft + x;
	if (BoxRight > RowSize) BoxRight = RowSize;
	
	BoxTop = loc.Y / gap.Y;
	BoxBottom = BoxTop + y;
	if (BoxBottom > ColumnSize) BoxBottom = ColumnSize;

	return result;
}

void UUW_InventoryGrid::ChangeGridSize(int32 InRowSize, int32 InColumnSize)
{
	RowSize = InRowSize;
	ColumnSize = InColumnSize;
	Grid->GetDynamicMaterial()->SetScalarParameterValue("Row", RowSize);
	Grid->GetDynamicMaterial()->SetScalarParameterValue("Column", ColumnSize);
}

void UUW_InventoryGrid::Refresh()
{
	TMap<AEqquipment*, TTuple<int32, int32>> map;
	GridCanvasPanel->ClearChildren();
	CheckNull(OwnerComponent);
	OwnerComponent->GetAllItems(map);

	FVector2D size = USlateBlueprintLibrary::GetLocalSize(Background->GetCachedGeometry());
	FVector2D gap = FVector2D(size.X / RowSize, size.Y / ColumnSize);

	for (auto i : map)
	{
		UUW_InventoryItem* widget = CreateWidget<UUW_InventoryItem, APlayerController>(GetOwningPlayer(), ItemWidgetClass);
		if (!widget)continue;
		UItemObject* obj = i.Key->GetItemObject();
		widget->Init(gap, obj, OwnerComponent);
		widget->OnInventoryItemRemoved.AddUFunction(this, "OnItemRemoved");
		widget->OnInventoryItemMouseEnter.AddUFunction(this, "OnInfoPopup");
		widget->OnInventoryItemMouseLeave.AddUFunction(this, "OffInfoPopup");
		UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(GridCanvasPanel->AddChild(widget));
		if (!slot)continue;

		//��Ŀ ����
		int32 x, y;
		obj->GetDimensions(x, y);
		FAnchors anch; 
		anch.Minimum = FVector2D(double(i.Value.Key) / double(RowSize), double(i.Value.Value) / double(ColumnSize));
		anch.Maximum = anch.Minimum + FVector2D(double(x) / double(RowSize), double(y) / double(ColumnSize));
		slot->SetAnchors(anch);
		slot->SetOffsets(FMargin(0));
	}
}

void UUW_InventoryGrid::OnItemRemoved(UItemObject* InObject)
{
	CheckNull(OwnerComponent);
	AEqquipment* weapon = nullptr;
	if (InObject)weapon = InObject->GetEqquipment();
	OwnerComponent->Server_RemoveItem(weapon->GetUniqueID());
	Refresh();
	// ������ ������ ��ȭ�� ���� ���ŵǰ� �����س���
	// �巡�׵���� ������ ��ü�� ���ְ� ���� ������ ��ٷȴµ�
	// �׷����ϸ� ������ ��ȭ�� ���� ��쿡�� ���ΰ�ħ�� �ȵǴ� ��쵵 �־�
	// ���� ���ΰ�ħ

}

void UUW_InventoryGrid::OnInfoPopup(UItemObject* InObject)
{
	Popup->On(InObject);
}

void UUW_InventoryGrid::OffInfoPopup()
{
	Popup->Off();
}

FEventReply UUW_InventoryGrid::OnGridBorderMouseButtonDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	return UWidgetBlueprintLibrary::Handled();
}

void UUW_InventoryGrid::Init(UInventoryComponent* InComponent, UUW_InventoryPopup* InPopup)
{
	OwnerComponent = InComponent;
	Popup = InPopup;

	Background->OnMouseButtonDownEvent.BindUFunction(this, "OnGridBorderMouseButtonDown");

	if (!OwnerComponent)
	{
		CLog::Print("UUW_InventoryGrid::Init InComponent is nullptr");
		ChangeGridSize();
		return;
	}
	ChangeGridSize(InComponent->GetRows(), InComponent->GetColumns());

	UCanvasPanelSlot* slot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Background);
	Refresh();
	OwnerComponent->OnInventoryChanged.AddDynamic(this, &UUW_InventoryGrid::Refresh);
}

void UUW_InventoryGrid::ChangeOwnerComponent(UInventoryComponent* InComponent)
{
	//���ε�����

	//���� ����
	OwnerComponent = InComponent;

	//�����ʱ�ȭ
	if (!OwnerComponent)
	{
		CLog::Print("UUW_InventoryGrid::Init InComponent is nullptr");
		ChangeGridSize();
		return;
	}
	ChangeGridSize(InComponent->GetRows(), InComponent->GetColumns());

	//����
	Refresh();
	OwnerComponent->OnInventoryChanged.AddDynamic(this, &UUW_InventoryGrid::Refresh);
}
