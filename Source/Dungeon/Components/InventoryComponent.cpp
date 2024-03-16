#include "Components/InventoryComponent.h"
#include "Global.h"

#include "Characters/PlayerCharacter.h"
#include "DungeonPlayerController.h"
#include "Objects/ItemManager.h"
#include "Objects/ItemObject.h"
#include "Objects/Weapon.h"
#include "Widgets/UW_Inventory.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(1);
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	APlayerCharacter* owner = Cast<APlayerCharacter>(GetOwner());
	ADungeonPlayerController* pc = Cast<ADungeonPlayerController>(owner->GetController());
	if (pc && pc->IsLocalController())InitWidget();
	if (owner->HasAuthority())InitDefault();
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicated 변수를 여기에 추가
	DOREPLIFETIME_CONDITION(UInventoryComponent, Items, COND_None);
	DOREPLIFETIME_CONDITION(UInventoryComponent, PresetItems, COND_None);
}

void UInventoryComponent::InitDefault()
{
	APlayerCharacter* owner = Cast<APlayerCharacter>(GetOwner());
	int32 num = UGameplayStatics::GetNumLocalPlayerControllers(GetWorld());
	AItemManager* manager = nullptr;
	for (int32 i = 0; i < num; i++)
	{
		ADungeonPlayerController* pc = Cast<ADungeonPlayerController>(UGameplayStatics::GetPlayerControllerFromID(GetWorld(), i));
		if (pc && pc->IsLocalController())
			manager = pc->GetItemManager();
	}

	Items.Init(nullptr, Columns * Rows);
	PresetItems.Init(nullptr, 1);

	if (DefaultWeapon)
	{
		CurrentWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeapon);
		CurrentWeapon->SetManager(manager);
		CurrentWeapon->OffCollision();
		FAttachmentTransformRules f = { EAttachmentRule::SnapToTarget, 1 };
		CurrentWeapon->AttachToComponent(owner->GetMesh(), f, CurrentWeapon->GetSocketName());
		CurrentWeapon->SetOwner(owner);
		CurrentWeapon->SetTeamID(owner->GetGenericTeamId());
	}

	if (InvTestClass)
	{
		AWeapon* test = GetWorld()->SpawnActor<AWeapon>(InvTestClass);
		test->SetManager(manager);
		Server_TryAddItem(test);
	}
}

void UInventoryComponent::InitWidget()
{
	if (WidgetClass)
	{
		APlayerCharacter* character = Cast<APlayerCharacter>(GetOwner());
		CheckNull(character);
		ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(character->GetController());
		CheckNull(controller);

		Widget = CreateWidget<UUW_Inventory, ADungeonPlayerController>(controller, WidgetClass);
		Widget->AddToViewport();
		Widget->Init(this);
	}
}

void UInventoryComponent::OnRep_Items()
{
	//서버 아이템 목록이 갱신됨
	if (OnInventoryChanged.IsBound())
		OnInventoryChanged.Broadcast();
}

void UInventoryComponent::OnRep_PresetItems()
{

}

void UInventoryComponent::Reset()
{
	//TSet<AWeapon*> itemSet;

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

bool UInventoryComponent::GetItemAtIndex(int32 InIndex, AWeapon** OutObject)
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

bool UInventoryComponent::IsRoomAvailable(AWeapon* InObject, int32 TopLeftIndex)
{
	int32 x, y, X, Y;
	IndexToTile(TopLeftIndex, x, y);
	InObject->GetItemObject()->GetDimensions(X, Y);
	X += x; Y += y;
	for (int32 i = x; i < X; i++)
	{
		for (int32 j = y; j < Y; j++)
		{
			if (i < 0 || i >= Columns)return 0;
			if (j < 0 || j >= Rows)return 0;
			int32 idx = TileToIndex(i, j);
			AWeapon* obj = nullptr;
			if (!GetItemAtIndex(idx, &obj))return 0;
			if (obj)return 0;
		}
	}
	return 1;
}

bool UInventoryComponent::IsRoomAvailable(AWeapon* InObject)
{
	if (!InObject)return 1;

	//히스토그램으로 사각형 찾기
	TArray<int32> arr; arr.Init(0, Columns * Rows);
	int32 cnt = 0; int32 rcnt = 0;
	int32 x = 0, y = 0, rx = 0, ry = 0;
	InObject->GetItemObject()->GetDimensions(x, y); InObject->GetItemObject()->Rotate();
	InObject->GetItemObject()->GetDimensions(rx, ry); InObject->GetItemObject()->Rotate();

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

bool UInventoryComponent::IsRoomGreen(AWeapon* InObject, int32 TopLeftIndex)
{
	int32 x, y, X, Y;
	IndexToTile(TopLeftIndex, x, y);
	InObject->GetItemObject()->GetDimensions(X, Y);
	X += x; Y += y;
	for (int32 i = x; i < X; i++)
	{
		for (int32 j = y; j < Y; j++)
		{
			if (i < 0 || i >= Columns)return 0;
			if (j < 0 || j >= Rows)return 0;
			int32 idx = TileToIndex(i, j);
			AWeapon* obj = nullptr;
			if (!GetItemAtIndex(idx, &obj))return 0;
			if (obj && obj != InObject)return 0;
		}
	}
	return 1;
}

void UInventoryComponent::Server_TryAddItem_Implementation(AWeapon* InObject)
{
	if (!InObject)return;
	if (!IsRoomAvailable(InObject))return;
	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (!IsRoomAvailable(InObject, i))continue;
		Server_AddItemAt(InObject, i);
		return;
	}
}

void UInventoryComponent::Server_AddItemAt_Implementation(AWeapon* InObject, int32 TopLeftIndex)
{
	int32 x, y, X, Y;
	IndexToTile(TopLeftIndex, x, y);
	InObject->GetItemObject()->GetDimensions(X, Y);
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

	InObject->ChangeVisibility(EItemMode::Inventory);
	InObject->GetItemObject()->SetInventoryComp(this);

	if (OnInventoryChanged.IsBound())
		OnInventoryChanged.Broadcast();
}

void UInventoryComponent::Server_RemoveItem_Implementation(AWeapon* InObject)
{
	CheckNull(InObject);
	for (int32 i = 0; i < Items.Num(); i++)
		if (Items[i] == InObject)Items[i] = nullptr;

	if (OnInventoryChanged.IsBound())
		OnInventoryChanged.Broadcast();
}

void UInventoryComponent::GetAllItems(TMap<AWeapon*, TTuple<int32, int32>>& Map)
{
	for (int32 i = 0; i < Items.Num(); i++)
	{
		AWeapon* cur = Items[i];
		if (!Items[i])continue;
		if (Map.Contains(cur))continue;
		int32 x, y;
		IndexToTile(i, x, y);
		Map.Add(cur, TTuple < int32, int32>(x, y));
	}
}

bool UInventoryComponent::CanTakeOffEquipment(int32 InIdx)
{
	if (!PresetItems.IsValidIndex(InIdx))return 0;
	AWeapon* item = PresetItems[InIdx];
	if (!item)return 1;
	return IsRoomAvailable(item);
}

bool UInventoryComponent::CanTakeOffCurrentEquipment()
{
	return CanTakeOffEquipment(PresetIndex);
}

AWeapon* UInventoryComponent::GetPresetItems(int32 InIdx)
{
	return PresetItems.IsValidIndex(InIdx) ? PresetItems[InIdx] : nullptr;
}

void UInventoryComponent::Equip(AWeapon* InData)
{
	//UStateComponent* state = CHelpers::GetComponent<UCStateComponent>(GetOwner());
	//if (!state || !state->IsIdleMode())return;

	APlayerCharacter* owner = Cast<APlayerCharacter>(GetOwner());

	if (!InData)
	{
		if (CurrentWeapon)CurrentWeapon->ChangeVisibility(EItemMode::Inventory);

		CurrentWeapon = nullptr;
		return;
	}


	CurrentWeapon = InData;
	CurrentWeapon->OffCollision();
	FAttachmentTransformRules f = { EAttachmentRule::SnapToTarget, 1 };
	CurrentWeapon->SetOwner(owner);
	CurrentWeapon->SetTeamID(owner->GetGenericTeamId());
	CurrentWeapon->AttachItemToComponent(owner->GetMesh(), f, CurrentWeapon->GetSocketName());
	CurrentWeapon->ChangeVisibility(EItemMode::Equip);

	if (OnInventoryEquipWeapon.IsBound())
		OnInventoryEquipWeapon.Broadcast(InData == nullptr ? nullptr : InData);
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

bool UInventoryComponent::ChangePresetData(int32 InIdx, AWeapon* InData)
{
	//UCStateComponent* state = CHelpers::GetComponent<UCStateComponent>(GetOwner());
	//if (!state || !state->IsIdleMode())return 0;

	if (!PresetItems.IsValidIndex(InIdx))return 0;
	if (!CanTakeOffEquipment(InIdx))return 0;
	if (PresetItems[InIdx] && !IsRoomAvailable(PresetItems[InIdx]))return 0;
	if (IsRoomAvailable(PresetItems[InIdx]))Server_TryAddItem(PresetItems[InIdx]);
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

bool UInventoryComponent::IsWidgetVisible()
{
	return Widget->IsVisible();
}

void UInventoryComponent::OnWidget()
{
	Widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UInventoryComponent::OffWidget()
{
	Widget->SetVisibility(ESlateVisibility::Collapsed);
}