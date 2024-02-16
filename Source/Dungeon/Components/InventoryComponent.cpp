#include "Components/InventoryComponent.h"
#include "Global.h"

#include "Characters/DungeonCharacter.h"
#include "DungeonPlayerController.h"
#include "Objects/ItemObject.h"
#include "Objects/Weapon.h"
#include "Widgets/UW_Inventory.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	InitDefault();
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInventoryComponent::InitDefault()
{
	ADungeonCharacter* owner = Cast<ADungeonCharacter>(GetOwner());

	Items.Init(nullptr, Columns * Rows);

	if (WidgetClass)
	{
		ADungeonCharacter* character = Cast<ADungeonCharacter>(GetOwner());
		CheckNull(character);
		ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(character->GetController());
		CheckNull(controller);

		Widget = CreateWidget<UUW_Inventory, ADungeonPlayerController>(controller, WidgetClass);
		Widget->AddToViewport();
		Widget->Init(this);
	}

	if (DefaultWeapon)
	{
		CurrentWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeapon);
		CurrentWeapon->OffCollision();
		FAttachmentTransformRules f = { EAttachmentRule::SnapToTarget, 1 };
		CurrentWeapon->AttachToComponent(owner->GetMesh(), f, CurrentWeapon->GetSocketName());
		CurrentWeapon->SetOwner(owner);
		CurrentWeapon->SetTeamID(owner->GetGenericTeamId());
	}

	if (InvTestClass)
	{
		AWeapon* test = GetWorld()->SpawnActor<AWeapon>(InvTestClass);
		TryAddItem(test->GetItemObject());
	}
}

void UInventoryComponent::Reset()
{
	//TSet<UItemObject*> itemSet;

	////PresetData
	//for (int32 i = 0; i < 3; i++)
	//{
	//	if (!PresetItems[i])continue;
	//	itemSet.Add(PresetItems[i]);
	//	ChangePresetData(i, nullptr);
	//}

	//InventoryData
	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (!Items[i])continue;
		//if (!itemSet.Contains(Items[i]))
		//	itemSet.Add(PresetItems[i]);
		Items[i] = nullptr;
	}

	//for (auto i : itemSet)
	//	i->GetAttachment()->Destroy();
}

void UInventoryComponent::IndexToTile(int32 InIndex, int32& X, int32& Y)
{
	X = InIndex % Columns;
	Y = InIndex / Columns;
}

bool UInventoryComponent::GetItemAtIndex(int32 InIndex, UItemObject** OutObject)
{
	*OutObject = nullptr;
	if (!Items.IsValidIndex(InIndex))return 0;
	*OutObject = Items[InIndex];
	return 1;
}

void UInventoryComponent::OnCollision()
{
	if (CurrentWeapon)CurrentWeapon->OnCollision();
}

void UInventoryComponent::OffCollision()
{
	if (CurrentWeapon)CurrentWeapon->OffCollision();
}

void UInventoryComponent::ResetHittedActors()
{
	if (CurrentWeapon)CurrentWeapon->ResetHittedActors();
}

bool UInventoryComponent::IsRoomAvailable(UItemObject* InObject, int32 TopLeftIndex)
{
	int32 x, y, X, Y;
	IndexToTile(TopLeftIndex, x, y);
	InObject->GetDimensions(X, Y);
	X += x; Y += y;
	for (int32 i = x; i < X; i++)
	{
		for (int32 j = y; j < Y; j++)
		{
			if (i < 0 || i >= Columns)return 0;
			if (j < 0 || j >= Rows)return 0;
			int32 idx = TileToIndex(i, j);
			UItemObject* obj = nullptr;
			if (!GetItemAtIndex(idx, &obj))return 0;
			if (obj)return 0;
		}
	}
	return 1;
}

bool UInventoryComponent::IsRoomAvailable(UItemObject* InObject)
{
	if (!InObject)return 1;

	//히스토그램으로 사각형 찾기
	TArray<int32> arr; arr.Init(0, Columns * Rows);
	int32 cnt = 0; int32 rcnt = 0;
	int32 x = 0, y = 0, rx = 0, ry = 0;
	InObject->GetDimensions(x, y); InObject->Rotate();
	InObject->GetDimensions(rx, ry); InObject->Rotate();

	for (int32 i = 0; i < arr.Num(); i++)
	{
		if (Items[i])continue;
		arr[i] = 1;
		if (i / Columns)arr[i] += arr[i - Columns];
	}

	for (int32 i = 0; i < arr.Num(); i++)
	{
		cnt = 0; rcnt = 0;
		if (i % Columns == Columns - 1)
		{
			for (int32 j = (i / Columns) * Columns; j <= i; j++)
			{
				if (arr[j] >= y)cnt++;
				else cnt = 0;

				if (cnt >= x)
					return 1;

				if (arr[j] >= ry)rcnt++;
				else rcnt = 0;

				if (rcnt >= rx)
					return 1;
			}
		}
	}
	return 0;
}

bool UInventoryComponent::TryAddItem(UItemObject* InObject)
{
	if (!InObject)return 0;

	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (!IsRoomAvailable(InObject, i))continue;
		AddItemAt(InObject, i);
		return 1;
	}

	InObject->Rotate();

	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (!IsRoomAvailable(InObject, i))continue;
		AddItemAt(InObject, i);
		return 1;
	}

	InObject->Rotate();
	return 0;

}

void UInventoryComponent::AddItemAt(UItemObject* InObject, int32 TopLeftIndex)
{
	int32 x, y, X, Y;
	IndexToTile(TopLeftIndex, x, y);
	InObject->GetDimensions(X, Y);
	X += x; Y += y;
	for (int32 i = x; i < X; i++)
	{
		for (int32 j = y; j < Y; j++)
		{
			if (i < 0 || i >= Columns)continue;
			if (j < 0 || j >= Rows)continue;
			int32 idx = TileToIndex(i, j);
			Items[idx] = InObject;
		}
	}

	//InObject->GetAttachment()->SetInventoryMode();
	//InObject->SetInventoryComp(this);

	if (OnInventoryChanged.IsBound())
		OnInventoryChanged.Broadcast();
}

void UInventoryComponent::RemoveItem(UItemObject* InObject)
{
	CheckNull(InObject);
	for (int32 i = 0; i < Items.Num(); i++)
		if (Items[i] == InObject)Items[i] = nullptr;

	if (OnInventoryChanged.IsBound())
		OnInventoryChanged.Broadcast();
}

void UInventoryComponent::GetAllItems(TMap<UItemObject*, TTuple<int32, int32>>& Map)
{
	for (int32 i = 0; i < Items.Num(); i++)
	{
		UItemObject* cur = Items[i];
		if (!cur)continue;
		if (Map.Contains(cur))continue;
		int32 x, y;
		IndexToTile(i, x, y);
		Map.Add(cur, TTuple < int32, int32>(x, y));
	}
}
