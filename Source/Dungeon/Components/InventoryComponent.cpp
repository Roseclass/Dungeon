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
	PresetItems.Init(nullptr, 1);

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
		test->SetInventoryMode();
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

	InObject->GetWeapon()->SetInventoryMode();
	InObject->SetInventoryComp(this);

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

bool UInventoryComponent::CanTakeOffEquipment(int32 InIdx)
{
	if (!PresetItems.IsValidIndex(InIdx))return 0;
	UItemObject* item = PresetItems[InIdx];
	if (!item)return 1;
	return IsRoomAvailable(item);
}

bool UInventoryComponent::CanTakeOffCurrentEquipment()
{
	return CanTakeOffEquipment(PresetIndex);
}

UItemObject* UInventoryComponent::GetPresetItems(int32 InIdx)
{
	return PresetItems.IsValidIndex(InIdx) ? PresetItems[InIdx] : nullptr;
}

void UInventoryComponent::Equip(UItemObject* InData)
{
	//UStateComponent* state = CHelpers::GetComponent<UCStateComponent>(GetOwner());
	//if (!state || !state->IsIdleMode())return;

	if (!InData)
	{
		if (CurrentWeapon)CurrentWeapon->SetInventoryMode();
		CurrentWeapon = nullptr;
		return;
	}

	ADungeonCharacter* owner = Cast<ADungeonCharacter>(GetOwner());

	CurrentWeapon = InData->GetWeapon();
	CurrentWeapon->OffCollision();
	FAttachmentTransformRules f = { EAttachmentRule::SnapToTarget, 1 };
	CurrentWeapon->AttachToComponent(owner->GetMesh(), f, CurrentWeapon->GetSocketName());
	CurrentWeapon->SetOwner(owner);
	CurrentWeapon->SetTeamID(owner->GetGenericTeamId());
	CurrentWeapon->SetEquipMode();

	if (OnInventoryEquipWeapon.IsBound())
		OnInventoryEquipWeapon.Broadcast(InData == nullptr ? nullptr : InData->GetWeapon());
}

void UInventoryComponent::EquipPreset(int32 InIdx)
{
	CheckFalse(PresetItems.IsValidIndex(InIdx));

	//UCActionComponent* action = CHelpers::GetComponent<UCActionComponent>(GetOwner());
	//bool bUnarmed = action->IsUnarmedMode();
	//if ((PresetIndex == InIdx && !bUnarmed) || !PresetItems[InIdx])Select->PlaySelectDown();
	//else if (InIdx == 0)Select->PlaySelectLeft();
	//else if (InIdx == 1)Select->PlaySelectUp();
	//else if (InIdx == 2)Select->PlaySelectRight();

	PresetIndex = InIdx;
	Equip(PresetItems[InIdx]);
}

bool UInventoryComponent::ChangePresetData(int32 InIdx, UItemObject* InData)
{
	//UCStateComponent* state = CHelpers::GetComponent<UCStateComponent>(GetOwner());
	//if (!state || !state->IsIdleMode())return 0;

	if (!PresetItems.IsValidIndex(InIdx))return 0;
	if (!CanTakeOffEquipment(InIdx))return 0;
	if (PresetItems[InIdx] && !TryAddItem(PresetItems[InIdx]))return 0;
	PresetItems[InIdx] = InData;
	if (InIdx == PresetIndex)Equip(PresetItems[InIdx]);
	if (OnInventoryPresetChanged.IsBound())
		OnInventoryPresetChanged.Broadcast();

	//if (Select)
	//	Select->ChangeData(InIdx, InData);

	return 1;
}

void UInventoryComponent::ChangePresetIndex(int32 InIdx)
{
	// 기본적으로 플레이어가 키보드로 누르면 ui교체,장비교체
	// 위젯의 프리셋 변경버튼 눌러도 ui교체,장비교체
	// 둘을 묶는 함수가 필요할듯?
	//
	// 이미지를 프리셋에 맞게 변경
	// 현재 인덱스 변경

	CheckFalse(PresetItems.IsValidIndex(InIdx));
	PresetIndex = InIdx;
	Equip(PresetItems[InIdx]);

	//if (OnInventoryPresetChanged.IsBound())
	//	OnInventoryPresetChanged.Broadcast();
}

bool UInventoryComponent::RemovePreset(int32 InIdx)
{
	return ChangePresetData(InIdx, nullptr);
}

bool UInventoryComponent::RemovePreset_Drag(int32 InIdx)
{
	//UCStateComponent* state = CHelpers::GetComponent<UCStateComponent>(GetOwner());
	//if (!state || !state->IsIdleMode())return 0;
	 
	if (!PresetItems.IsValidIndex(InIdx))return 0;
	PresetItems[InIdx] = nullptr;
	if (InIdx == PresetIndex)Equip(PresetItems[InIdx]);
	if (OnInventoryPresetChanged.IsBound())
		OnInventoryPresetChanged.Broadcast();

	//if (Select)
	//	Select->ChangeData(InIdx, nullptr);

	return 1;
}