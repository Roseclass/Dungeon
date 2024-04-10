#include "Components/InventoryComponent.h"
#include "Global.h"

#include "Characters/PlayerCharacter.h"
#include "Characters/Enemy.h"
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

	ADungeonCharacterBase* owner = Cast<ADungeonCharacterBase>(GetOwner());
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
	DOREPLIFETIME_CONDITION(UInventoryComponent, EquippedItems, COND_None);
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
	EquippedItems.Init(nullptr, int32(EItemType::Max));

	if (DefaultWeapon)
	{
		int32 idx = int32(EItemType::Weapon);
		if (EquippedItems.IsValidIndex(idx))
		{
			AWeapon* weapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeapon);
			if (weapon)
			{
				weapon->SetManager(manager);
				weapon->OffCollision();
				FAttachmentTransformRules f = { EAttachmentRule::SnapToTarget, 1 };
				weapon->AttachToComponent(owner->GetMesh(), f, weapon->GetSocketName());
				weapon->SetOwner(owner);
				weapon->SetTeamID(owner->GetGenericTeamId());
				EquippedItems[idx] = weapon;
			}
		}
	}

	if (InvTestClass)
	{
		AEqquipment* test = GetWorld()->SpawnActor<AEqquipment>(InvTestClass);
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

void UInventoryComponent::OnRep_EquippedItems()
{
	// TODO::Change appearance
}

void UInventoryComponent::Reset()
{
	//TSet<AEqquipment*> itemSet;

	////EquippedData
	//for (int32 i = 0; i < 3; i++)
	//{
	//	if (!EquippedItems[i])continue;
	//	itemSet.Add(EquippedItems[i]);
	//	ChangeEquippedData(i, nullptr);
	//}

	//InventoryData
	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (!Items[i])continue;
		//if (!itemSet.Contains(Items[i]))
		//	itemSet.Add(EquippedItems[i]);
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

bool UInventoryComponent::GetItemAtIndex(int32 InIndex, AEqquipment** OutObject)
{
	*OutObject = nullptr;
	if (!Items.IsValidIndex(InIndex))return 0;
	*OutObject = Items[InIndex];
	return 1;
}

void UInventoryComponent::OnCollision()
{
	int32 idx = int32(EItemType::Weapon);
	if (!EquippedItems.IsValidIndex(idx))
	{
		CLog::Print("UInventoryComponent::OnCollision IsNotValidIndex", -1, 10, FColor::Red);
		return;
	}
	AWeapon* weapon = Cast<AWeapon>(EquippedItems[idx]);
	if (weapon)weapon->OnCollision();
}

void UInventoryComponent::OffCollision()
{
	int32 idx = int32(EItemType::Weapon);
	if (!EquippedItems.IsValidIndex(idx))
	{
		CLog::Print("UInventoryComponent::OffCollision IsNotValidIndex", -1, 10, FColor::Red);
		return;
	}
	AWeapon* weapon = Cast<AWeapon>(EquippedItems[idx]);
	if (weapon)weapon->OffCollision();
}

void UInventoryComponent::ResetHittedActors()
{
	int32 idx = int32(EItemType::Weapon);
	if (!EquippedItems.IsValidIndex(idx))
	{
		CLog::Print("UInventoryComponent::ResetHittedActors IsNotValidIndex", -1, 10, FColor::Red);
		return;
	}
	AWeapon* weapon = Cast<AWeapon>(EquippedItems[idx]);
	if (weapon)weapon->ResetHittedActors();
}

bool UInventoryComponent::IsRoomAvailable(AEqquipment* InObject, int32 TopLeftIndex)
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
			AEqquipment* obj = nullptr;
			if (!GetItemAtIndex(idx, &obj))return 0;
			if (obj)return 0;
		}
	}
	return 1;
}

bool UInventoryComponent::IsRoomAvailable(AEqquipment* InObject)
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

bool UInventoryComponent::IsRoomGreen(AEqquipment* InObject, int32 TopLeftIndex)
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
			AEqquipment* obj = nullptr;
			if (!GetItemAtIndex(idx, &obj))return 0;
			if (obj && obj != InObject)return 0;
		}
	}
	return 1;
}

void UInventoryComponent::Server_TryAddItem_Implementation(AEqquipment* InObject)
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

void UInventoryComponent::Server_AddItemAt_Implementation(AEqquipment* InObject, int32 TopLeftIndex)
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

void UInventoryComponent::Server_RemoveItem_Implementation(AEqquipment* InObject)
{
	CheckNull(InObject);
	for (int32 i = 0; i < Items.Num(); i++)
		if (Items[i] == InObject)Items[i] = nullptr;

	if (OnInventoryChanged.IsBound())
		OnInventoryChanged.Broadcast();
}

void UInventoryComponent::GetAllItems(TMap<AEqquipment*, TTuple<int32, int32>>& Map)
{
	for (int32 i = 0; i < Items.Num(); i++)
	{
		AEqquipment* cur = Items[i];
		if (!Items[i])continue;
		if (Map.Contains(cur))continue;
		int32 x, y;
		IndexToTile(i, x, y);
		Map.Add(cur, TTuple < int32, int32>(x, y));
	}
}

bool UInventoryComponent::CanTakeOffEquipment(int32 InIdx)
{
	if (!EquippedItems.IsValidIndex(InIdx))return 0;
	AEqquipment* item = EquippedItems[InIdx];
	if (!item)return 1;
	return IsRoomAvailable(item);
}

bool UInventoryComponent::CanTakeOffCurrentEquipment()
{
	return CanTakeOffEquipment(EquippedIndex);
}

AEqquipment* UInventoryComponent::GetEquippedItems(int32 InIdx)
{
	return EquippedItems.IsValidIndex(InIdx) ? EquippedItems[InIdx] : nullptr;
}

void UInventoryComponent::Equip(AEqquipment* InData)
{
	//UStateComponent* state = CHelpers::GetComponent<UCStateComponent>(GetOwner());
	//if (!state || !state->IsIdleMode())return;

	APlayerCharacter* owner = Cast<APlayerCharacter>(GetOwner());

	if (!InData)
	{
		CLog::Print("UInventoryComponent::Equip InData is nullptr", -1, 10, FColor::Red);
		return;
	}

	int32 idx = int32(InData->GetType());

	EquippedItems[idx] = InData;
	EquippedItems[idx]->SetOwner(owner);
	EquippedItems[idx]->ChangeVisibility(EItemMode::Equip);

	if (InData->GetType() == EItemType::Weapon)
	{
		AWeapon* weapon = Cast<AWeapon>(EquippedItems[idx]);
		if (weapon)
		{
			weapon->OffCollision();
			FAttachmentTransformRules f = { EAttachmentRule::SnapToTarget, 1 };
			weapon->SetTeamID(owner->GetGenericTeamId());
			weapon->AttachItemToComponent(owner->GetMesh(), f, weapon->GetSocketName());
		}
	}


	if (OnInventoryEquipWeapon.IsBound())
		OnInventoryEquipWeapon.Broadcast(InData == nullptr ? nullptr : InData);
}

void UInventoryComponent::EquipEquipped(int32 InIdx)
{
	CheckFalse(EquippedItems.IsValidIndex(InIdx));

	//UCActionComponent* action = CHelpers::GetComponent<UCActionComponent>(GetOwner());
	//bool bUnarmed = action->IsUnarmedMode();
	//if ((EquippedIndex == InIdx && !bUnarmed) || !EquippedItems[InIdx])Select->PlaySelectDown();
	//else if (InIdx == 0)Select->PlaySelectLeft();
	//else if (InIdx == 1)Select->PlaySelectUp();
	//else if (InIdx == 2)Select->PlaySelectRight();

	EquippedIndex = InIdx;
	Equip(EquippedItems[InIdx]);
}

bool UInventoryComponent::ChangeEquippedData(int32 InIdx, AEqquipment* InData)
{
	//UCStateComponent* state = CHelpers::GetComponent<UCStateComponent>(GetOwner());
	//if (!state || !state->IsIdleMode())return 0;

	if (!EquippedItems.IsValidIndex(InIdx))return 0;
	if (!CanTakeOffEquipment(InIdx))return 0;
	if (EquippedItems[InIdx] && !IsRoomAvailable(EquippedItems[InIdx]))return 0;
	if (IsRoomAvailable(EquippedItems[InIdx]))Server_TryAddItem(EquippedItems[InIdx]);
	EquippedItems[InIdx] = InData;
	if (InIdx == EquippedIndex)Equip(EquippedItems[InIdx]);
	//if (OnInventoryEquippedChanged.IsBound())
	//	OnInventoryEquippedChanged.Broadcast(EAppearancePart::ChestAttachment,);

	//if (Select)
	//	Select->ChangeData(InIdx, InData);

	return 1;
}

void UInventoryComponent::ChangeEquippedIndex(int32 InIdx)
{
	// 기본적으로 플레이어가 키보드로 누르면 ui교체,장비교체
	// 위젯의 프리셋 변경버튼 눌러도 ui교체,장비교체
	// 둘을 묶는 함수가 필요할듯?
	//
	// 이미지를 프리셋에 맞게 변경
	// 현재 인덱스 변경

	CheckFalse(EquippedItems.IsValidIndex(InIdx));
	EquippedIndex = InIdx;
	Equip(EquippedItems[InIdx]);

	//if (OnInventoryEquippedChanged.IsBound())
	//	OnInventoryEquippedChanged.Broadcast();
}

bool UInventoryComponent::RemoveEquipped_Drag(int32 InIdx)
{
	//UCStateComponent* state = CHelpers::GetComponent<UCStateComponent>(GetOwner());
	//if (!state || !state->IsIdleMode())return 0;
	 
	if (!EquippedItems.IsValidIndex(InIdx))return 0;
	EquippedItems[InIdx] = nullptr;
	if (InIdx == EquippedIndex)Equip(EquippedItems[InIdx]);
	//if (OnInventoryEquippedChanged.IsBound())
	//	OnInventoryEquippedChanged.Broadcast();

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