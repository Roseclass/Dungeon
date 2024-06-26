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
#include "Widgets/UW_Trade.h"

#include "GameplayTagContainer.h"
#include "Components/SkillComponent.h"
#include "Abilities/AbilityTaskTypes.h"


UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(1);
	OnInventoryEquippedDataChanged.Init(FInventoryEquippedDataChanged(), int32(EItemType::Max));
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	ACharacter* owner = Cast<ACharacter>(GetOwner());
	if (owner && owner->HasAuthority())InitDefault();
	ADungeonPlayerController* pc = Cast<ADungeonPlayerController>(owner->GetController());
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
	ADungeonCharacterBase* owner = Cast<ADungeonCharacterBase>(GetOwner());
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

	for (auto i : DefaultItems)
	{
		FTransform transform;
		AEqquipment* item = GetWorld()->SpawnActorDeferred<AEqquipment>(i, transform);
		if (IsRoomAvailable(item))
		{
			UGameplayStatics::FinishSpawningActor(item, transform);
			Server_TryAddItem(item);
		}
		else item->Destroy();
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

void UInventoryComponent::Server_LoadData_Implementation(const TArray<TSubclassOf<AEqquipment>>& EquippedClasses, const TArray<FItemStatusData>& EquippedDatas,	const TArray<FVector2D>& Locations, const TArray<TSubclassOf<AEqquipment>>& InventoryClasses, const TArray<FItemStatusData>& InventoryDatas)
{
	AItemManager* manager = Cast<AItemManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AItemManager::StaticClass()));
	if (!manager)
	{
		CLog::Print("UInventoryComponent::Server_LoadData manager is nullptr", -1, 10, FColor::Red);
		return;
	}

	FTransform transform;
	// spawn equipped items		
	for (int32 i = 0; i < EquippedClasses.Num(); ++i)
	{
		if (!EquippedClasses[i])continue;
		AEqquipment* equipment = manager->SpawnItem(EquippedClasses[i], transform);
		if (!equipment)continue;
		equipment->Load(EquippedDatas[i]);

		// equip item
		Server_Equip(equipment);
	}

	// spawn inventory items
	for (int32 i = 0; i < InventoryClasses.Num(); ++i)
	{
		if (!InventoryClasses[i])continue;
		AEqquipment* equipment = manager->SpawnItem(InventoryClasses[i], transform);
		if (!equipment)continue;
		equipment->Load(InventoryDatas[i]);
		
		// add items to inventory
		Server_AddItemAt(equipment, TileToIndex(Locations[i].X, Locations[i].Y));
	}
}

void UInventoryComponent::Client_Trade_Implementation(AActor* InActor)
{
	UInventoryComponent* trade = CHelpers::GetComponent<UInventoryComponent>(InActor);
	CheckNull(trade);

	// TODO::reveal widget
	Widget->Trade(trade);
	Widget->SetVisibility(ESlateVisibility::Visible);
}

void UInventoryComponent::Server_Buy_Implementation(AEqquipment* InObject)
{
	// TODO::check gold condition

	//check room
	if (!IsRoomAvailable(InObject))
	{
		return;
	}

	// spawn,tryadd,reducegold->client:refresh
	FTransform transform;
	AEqquipment* item = GetWorld()->SpawnActorDeferred<AEqquipment>(InObject->GetClass(), transform);
	UGameplayStatics::FinishSpawningActor(item, transform);
	Server_TryAddItem(item);

	// TODO::send message
	//if(flag == 0) proceed
	//if(flag == 1) not enough gold
	//if(flag == 2) no rooms

	// TODO::system message
	// static
	// init in mainhud
	// 
	// USystemMessage::SetTextblock(utextblock* InText);
	// USystemMessage::Send("");
	//
}

void UInventoryComponent::OnCollision(const FDamageEhancementData* InDamageData)
{
	int32 idx = int32(EItemType::Weapon);
	if (!EquippedItems.IsValidIndex(idx))
	{
		CLog::Print("UInventoryComponent::OnCollision IsNotValidIndex", -1, 10, FColor::Red);
		return;
	}
	AWeapon* weapon = Cast<AWeapon>(EquippedItems[idx]);
	if (weapon)weapon->OnCollision(InDamageData);
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
	int32 cnt = 0;
	int32 x = 0, y = 0;
	InObject->GetDimensions(x, y); 

	for (int32 i = 0; i < arr.Num(); i++)
	{
		if (Items[i])continue;
		arr[i] = 1;
		if (i / Columns)arr[i] += arr[i - Columns];
	}

	for (int32 i = 0; i < arr.Num(); i++)
	{
		cnt = 0;
		if (i % Columns == Columns - 1)
		{
			for (int32 j = (i / Columns) * Columns; j <= i; j++)
			{
				if (arr[j] >= y)cnt++;
				else cnt = 0;

				if (cnt >= x)
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
	if (InObject->HasOwnerCharacter())
	{
		ACharacter* ownerCharacter = Cast<ACharacter>(GetOwner());
		if (InObject->GetOwnerCharacter() != ownerCharacter)
		{
			CLog::Print("UInventoryComponent::Server_TryAddItem_Implementation owner Error", -1, 10, FColor::Red);
			return;
		}
	}
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

void UInventoryComponent::Trade(AActor* InActor)
{
	CheckFalse(InActor->GetIsReplicated());
	Client_Trade(InActor);
}

void UInventoryComponent::Server_Sell_Implementation(AEqquipment* InObject)
{
	// TODO::add gold
	
	//destroy item
	InObject->Destroy();

	OnRep_Items();
}

void UInventoryComponent::Buy(AEqquipment* InObject)
{
	// TODO::check gold condition

	Server_Buy(InObject);
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
	APlayerCharacter* owner = Cast<APlayerCharacter>(GetOwner());
	
	if (!InData)
	{
		CLog::Print("UInventoryComponent::Equip InData is nullptr", -1, 10, FColor::Red);
		return;
	
	}

	int32 idx = int32(InData->GetType());

	if (!EquippedItems.IsValidIndex(idx))return;
	if (!CanTakeOffEquipment(idx))return;
	if (EquippedItems[idx] && !IsRoomAvailable(EquippedItems[idx]))return;
	if (IsRoomAvailable(EquippedItems[idx]))Server_TryAddItem(EquippedItems[idx]);

	if(EquippedItems[idx])
	{
		const TArray<FSkillEnhancement>& enhancementDatas = EquippedItems[idx]->GetItemStatus().GetEnhancementDatas();
		USkillComponent* skill = CHelpers::GetComponent<USkillComponent>(owner);
		if (skill)skill->EnhanceAbility(enhancementDatas, -1);
	}

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

	{
		const TArray<FSkillEnhancement>& enhancementDatas = EquippedItems[idx]->GetItemStatus().GetEnhancementDatas();
		USkillComponent* skill = CHelpers::GetComponent<USkillComponent>(owner);
		if (skill)skill->EnhanceAbility(enhancementDatas);
	}
}

void UInventoryComponent::Server_RemoveEquipped_Drag_Implementation(int32 InIdx)
{
	if (!EquippedItems.IsValidIndex(InIdx))return;

	APlayerCharacter* owner = Cast<APlayerCharacter>(GetOwner());

	if (EquippedItems[InIdx])
	{
		const TArray<FSkillEnhancement>& enhancementDatas = EquippedItems[InIdx]->GetItemStatus().GetEnhancementDatas();
		USkillComponent* skill = CHelpers::GetComponent<USkillComponent>(owner);
		if (skill)skill->EnhanceAbility(enhancementDatas, -1);
	}

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
		SaveData->PlayerData.EquippedDatas.Add(i->GetItemStatus());
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
		SaveData->PlayerData.InventoryDatas.Add(eqquipment->GetItemStatus());
	}
}

void UInventoryComponent::LoadData(USaveGameData* const ReadData)
{
	TArray<TSubclassOf<AEqquipment>> equippedClasses;
	TArray<FItemStatusData> equippedDatas;
	TArray<FVector2D> locations;
	TArray<TSubclassOf<AEqquipment>> inventoryClasses;
	TArray<FItemStatusData> inventoryDatas;

	for (auto i : ReadData->PlayerData.EquippedClasses)
		equippedClasses.Add(i);

	for (auto i : ReadData->PlayerData.EquippedDatas)
		equippedDatas.Add(i);

	for (auto i : ReadData->PlayerData.InventoryClasses)
	{
		locations.Add(i.Key);
		inventoryClasses.Add(i.Value);
	}

	for (auto i : ReadData->PlayerData.InventoryDatas)
		inventoryDatas.Add(i);

	Server_LoadData(equippedClasses, equippedDatas, locations, inventoryClasses, inventoryDatas);
}
