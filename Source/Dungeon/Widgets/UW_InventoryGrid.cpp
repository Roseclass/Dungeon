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

#include "Components/InventoryComponent.h"
#include "Objects/ItemObject.h"
#include "Widgets/UW_InventoryItem.h"

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

	////격자
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

	//드래그 감지
	if (UWidgetBlueprintLibrary::IsDragDropping() && DrawBox && OwnerComponent)
	{
		UDragDropOperation* operation = UWidgetBlueprintLibrary::GetDragDroppingContent();
		UItemObject* itemObject = nullptr;
		if (operation) itemObject = Cast<UItemObject>(operation->Payload.Get());
		bool green = 0;

		green = OwnerComponent->IsRoomAvailable(itemObject);

		FVector2D start = FVector2D(topleft.X + (gap.X * BoxLeft), topleft.Y + (gap.Y * BoxTop));
		FVector2D end = start + (FVector2D(BoxRight, BoxBottom) * gap);

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
	return result;
}

bool UUW_InventoryGrid::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool result = Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);

	FVector2D mousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());

	FVector2D size = USlateBlueprintLibrary::GetLocalSize(Background->GetCachedGeometry());
	FVector2D gap = FVector2D(size.X / RowSize, size.Y / ColumnSize);
	FVector2D topleft = USlateBlueprintLibrary::GetLocalTopLeft(Background->GetCachedGeometry());
	
	int32 x, y;
	UItemObject* item = Cast<UItemObject>(InOperation->Payload);
	if (!item)return 1;
	item->GetDimensions(x, y);

	FVector2D loc = mousePos - topleft;
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
}

void UUW_InventoryGrid::Refresh()
{
	TMap<UItemObject*, TTuple<int32, int32>> map;
	GridCanvasPanel->ClearChildren();
	CheckNull(OwnerComponent);
	OwnerComponent->GetAllItems(map);

	FVector2D size = USlateBlueprintLibrary::GetLocalSize(Background->GetCachedGeometry());
	FVector2D gap = FVector2D(size.X / RowSize, size.Y / ColumnSize);

	for (auto i : map)
	{
		UUW_InventoryItem* widget = CreateWidget<UUW_InventoryItem, APlayerController>(GetOwningPlayer(), ItemWidgetClass);
		if (!widget)continue;
		widget->Init(gap, i.Key, OwnerComponent);
		widget->OnInventoryItemRemoved.AddUFunction(this, "OnItemRemoved");
		UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(GridCanvasPanel->AddChild(widget));
		if (!slot)continue;

		//앵커 조절
		int32 x, y;
		i.Key->GetDimensions(x, y);
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
	OwnerComponent->RemoveItem(InObject);
	TArray<UWidget*>arr = GridCanvasPanel->GetAllChildren();
	for (auto i : arr)
	{
		UUW_InventoryItem* widget = Cast<UUW_InventoryItem>(i);
		if (!widget)continue;
		if (widget->CheckItem(InObject))widget->RemoveFromParent();
	}
}

FEventReply UUW_InventoryGrid::OnGridBorderMouseButtonDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	return UWidgetBlueprintLibrary::Handled();
}

void UUW_InventoryGrid::DropItem(UItemObject* InObject)
{
	CheckNull(InObject);

	//이부분은 컴포넌트에서
	AActor* actor = OwnerComponent->GetOwner();
	FVector start = actor->GetActorLocation() + actor->GetActorForwardVector() * 150.0f;
	TArray<AActor*> arr; FHitResult hit;
	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), start, start + FVector(0, 0, -1000), ETraceTypeQuery::TraceTypeQuery1, 0, arr, EDrawDebugTrace::None, hit, 1))
	{
		//InObject->GetAttachment()->SetActorLocation(hit.Location);
		//InObject->GetAttachment()->SetPickableMode();
		//InObject->GetAttachment()->SetActorRotation(FRotator());
	}
}

void UUW_InventoryGrid::Init(UInventoryComponent* InComponent)
{
	OwnerComponent = InComponent;
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
	//바인딩해제
	//if (OwnerComponent)OwnerComponent->OnInventroyChanged.Remove(this, "Refresh");
	//오너 변경
	OwnerComponent = InComponent;

	//격자초기화
	if (!OwnerComponent)
	{
		CLog::Print("UUW_InventoryGrid::Init InComponent is nullptr");
		ChangeGridSize();
		return;
	}
	ChangeGridSize(InComponent->GetRows(), InComponent->GetColumns());

	//갱신
	Refresh();
	OwnerComponent->OnInventoryChanged.AddDynamic(this, &UUW_InventoryGrid::Refresh);
}
