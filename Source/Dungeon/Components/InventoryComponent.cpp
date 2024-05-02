#include "Components/InventoryComponent.h"
#include "Global.h"

#include "SaveManager.h"
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
	OnInventoryEquippedDataChanged.Init(FInventoryEquippedDataChanged(), int32(EItemType::Max));
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	ADungeonCharacterBase* owner = Cast<ADungeonCharacterBase>(GetOwner());
	ADungeonPlayerController* pc = Cast<ADungeonPlayerController>(owner->GetController());
	if (owner->HasAuthority())InitDefault();
	if (pc && pc->IsLocalController())InitWidget();
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
	// refresh inv grid list
	if (OnInventoryChanged.IsBound())
		OnInventoryChanged.Broadcast();
}

void UInventoryComponent::OnRep_EquippedItems()
{
	// refresh inv slot
	int32 t = EquippedItems.Num() > OnInventoryEquippedDataChanged.Num() ? OnInventoryEquippedDataChanged.Num() : EquippedItems.Num();

	for (int32 i = 0; i < t; ++i)
	{
		UItemObject* data = nullptr;
		if (EquippedItems[i])
			data = EquippedItems[i]->GetItemObject();
		if (OnInventoryEquippedDataChanged[i].IsBound())
			OnInventoryEquippedDataChanged[i].Broadcast(data);
	}
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

void UInventoryComponent::Server_LoadData_Implementation(const TArray<TSubclassOf<AEqquipment>>& EquippedClasses, const TArray<FVector2D>& Locations, const TArray<TSubclassOf<AEqquipment>>& InventoryClasses)
{
	AItemManager* manager = Cast<AItemManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AItemManager::StaticClass()));
	if (!manager)
	{
		CLog::Print("UInventoryComponent::Server_LoadData manager is nullptr", -1, 10, FColor::Red);
		return;
	}

	FTransform transform;
	// spawn equipped items		
	for (auto i : EquippedClasses)
	{
		if (!i)continue;
		AEqquipment* equipment = manager->SpawnItem(i, transform);
		if (!equipment)continue;

		// equip item
		Server_Equip(equipment);
	}

	// spawn inventory items
	for (int32 i = 0; i < InventoryClasses.Num(); ++i)
	{
		if (!InventoryClasses[i])continue;
		AEqquipment* equipment = manager->SpawnItem(InventoryClasses[i], transform);
		if (!equipment)continue;

		// add items to inventory
		Server_AddItemAt(equipment, TileToIndex(Locations[i].X, Locations[i].Y));
	}
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

void UInventoryComponent::ResetHitActors()
{
	int32 idx = int32(EItemType::Weapon);
	if (!EquippedItems.IsValidIndex(idx))
	{
		CLog::Print("UInventoryComponent::ResetHitActors IsNotValidIndex", -1, 10, FColor::Red);
		return;
	}
	AWeapon* weapon = Cast<AWeapon>(EquippedItems[idx]);
	if (weapon)weapon->ResetHitActors();
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
	if (InObject->HasOwnerCharacter())return;
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
	InObject->SetOwnerCharacter(Cast<APlayerCharacter>(GetOwner()));
	InObject->GetItemObject()->SetInventoryComp(this);

	OnRep_Items();
}

void UInventoryComponent::Server_RemoveItem_Implementation(AEqquipment* InObject)
{
	CheckNull(InObject);
	for (int32 i = 0; i < Items.Num(); i++)
		if (Items[i] == InObject)Items[i] = nullptr;

	OnRep_Items();
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

AEqquipment* UInventoryComponent::GetEquippedItems(int32 InIdx)
{
	return EquippedItems.IsValidIndex(InIdx) ? EquippedItems[InIdx] : nullptr;
}

void UInventoryComponent::Server_Equip_Implementation(AEqquipment* InData)
{
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
}

void UInventoryComponent::Server_ChangeEquippedData_Implementation(int32 InIdx, AEqquipment* InData)
{
	int32 idx = int32(InData->GetType());

	if (!EquippedItems.IsValidIndex(idx))return;
	if (!CanTakeOffEquipment(idx))return;
	if (EquippedItems[idx] && !IsRoomAvailable(EquippedItems[idx]))return;
	if (IsRoomAvailable(EquippedItems[idx]))Server_TryAddItem(EquippedItems[idx]);
	EquippedItems[idx] = InData;

	if (OnInventoryEquippedChanged.IsBound() && EquippedItems[idx]->GetType() != EItemType::Weapon)
	{
		const TArray<FItemAppearanceData>& datas = EquippedItems[idx]->GetAppearanceDatas();
		for (auto i : datas)	OnInventoryEquippedChanged.Broadcast(i.PartType, i.Index);
	}

	if (OnChangeHairVisiblity.IsBound() && EquippedItems[idx]->GetType() == EItemType::Helms)
		OnChangeHairVisiblity.Broadcast(0);

	if (OnInventoryEquippedItemsChanged.IsBound())
		OnInventoryEquippedItemsChanged.Broadcast();

	OnRep_EquippedItems();
}

void UInventoryComponent::Server_RemoveEquipped_Drag_Implementation(int32 InIdx)
{
	if (!EquippedItems.IsValidIndex(InIdx))return;

	// reset to default
	if (OnInventoryEquippedChanged.IsBound() && EquippedItems[InIdx]->GetType() != EItemType::Weapon)
	{
		const TArray<FItemAppearanceData>& datas = EquippedItems[InIdx]->GetAppearanceDatas();
		for (auto i : datas)	
			OnInventoryEquippedChanged.Broadcast(i.PartType, 0);
	}
	
	if (OnChangeHairVisiblity.IsBound() && EquippedItems[InIdx]->GetType() == EItemType::Helms)
		OnChangeHairVisiblity.Broadcast(1);

	EquippedItems[InIdx] = nullptr;

	if (OnInventoryEquippedItemsChanged.IsBound())
		OnInventoryEquippedItemsChanged.Broadcast();

	OnRep_EquippedItems();
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

void UInventoryComponent::SaveData(USaveGameData* SaveData)
{
	//save EquippedData
	SaveData->PlayerData.EquippedClasses.Empty();
	for (auto i : EquippedItems)
	{
		if (!i)continue;
		SaveData->PlayerData.EquippedClasses.Add(i->GetClass());
	}	

	//save InventoryData
	SaveData->PlayerData.InventoryClasses.Empty();
	TMap<AEqquipment*, TTuple<int32, int32>> m;
	GetAllItems(m);
	for (auto i : m)
	{
		if (!i.Key)continue;
		AEqquipment* eqquipment = i.Key;
		if (!eqquipment)continue;
		SaveData->PlayerData.InventoryClasses.Add(FVector2D(i.Value.Key, i.Value.Value), eqquipment->GetClass());
	}
}

void UInventoryComponent::LoadData(USaveGameData* const ReadData)
{
	TArray<TSubclassOf<AEqquipment>> equippedClasses;
	TArray<FVector2D> locations;
	TArray<TSubclassOf<AEqquipment>> inventoryClasses;

	for (auto i : ReadData->PlayerData.EquippedClasses)
		equippedClasses.Add(i);

	for (auto i : ReadData->PlayerData.InventoryClasses)
	{
		locations.Add(i.Key);
		inventoryClasses.Add(i.Value);
	}

	Server_LoadData(equippedClasses, locations, inventoryClasses);
}
