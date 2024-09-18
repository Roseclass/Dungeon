#include "Components/InventoryComponent.h"
#include "Global.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

#include "SaveManager.h"
#include "Characters/PlayerCharacter.h"
#include "Characters/Enemy.h"
#include "DungeonPlayerController.h"
#include "Objects/ItemObject.h"
#include "Objects/Weapon.h"
#include "Widgets/UW_Inventory.h"
#include "Widgets/UW_Trade.h"

#include "GameplayTagContainer.h"
#include "Components/SkillComponent.h"
#include "Components/EquipmentManagementComponent.h"
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

	Items.Init(FString(), Columns * Rows);
	EquippedItems.Init(FString(), int32(EItemType::Max));

	//TODO::TEST
	if (DefaultWeapon)
	{
		int32 idx = int32(EItemType::Weapon);
		if (EquippedItems.IsValidIndex(idx))
		{
			FEquipmentStateUpdateParameters state;
			state.State = EItemMode::Equip;
			state.NewOwner = owner;
			AEqquipment* equipment = UEquipmentManagementComponent::SpawnEquipment(GetWorld(), DefaultWeapon, state);
			if (equipment)Server_Equip(equipment->GetUniqueID());
		}
	}

	//TODO::TEST
	for (auto i : DefaultItems)
	{
		FEquipmentStateUpdateParameters state;
		state.State = EItemMode::Inventory;
		state.NewOwner = owner;
		AEqquipment* equipment = UEquipmentManagementComponent::SpawnEquipment(GetWorld(), i, state);
		if (!equipment)continue;
		Server_TryAddItem(equipment->GetUniqueID());
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

	//TODO::TEST
	for (int32 i = 0; i < t; ++i)
	{
		if (EquippedItems[i] == FString())continue;
		AEqquipment* equipment = UEquipmentManagementComponent::GetEquipmentFromUniqueID(GetWorld(), EquippedItems[i]);
		if (!equipment)continue;
		UItemObject* data = equipment->GetItemObject();
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
		if (Items[i] == FString())continue;
		//if (!itemSet.Contains(Items[i]))
		//	itemSet.Add(EquippedItems[i]);
		Items[i] = FString();
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

	*OutObject = UEquipmentManagementComponent::GetEquipmentFromUniqueID(GetWorld(), Items[InIndex]);
	return *OutObject != nullptr;
}

void UInventoryComponent::Server_LoadData_Implementation(const TArray<TSubclassOf<AEqquipment>>& EquippedClasses, const TArray<FItemStatusData>& EquippedDatas,	const TArray<FVector2D>& Locations, const TArray<TSubclassOf<AEqquipment>>& InventoryClasses, const TArray<FItemStatusData>& InventoryDatas)
{
	ADungeonCharacterBase* owner = Cast<ADungeonCharacterBase>(GetOwner());

	//TODO::TEST
	// spawn equipped items		
	for (int32 i = 0; i < EquippedClasses.Num(); ++i)
	{
		if (!EquippedClasses[i])continue;
		FEquipmentStateUpdateParameters state;
		state.State = EItemMode::Loot;
		AEqquipment* equipment = UEquipmentManagementComponent::SpawnEquipment(GetWorld(), EquippedClasses[i], state);
		if (!equipment)continue;
		UEquipmentManagementComponent::UpadteStatus(GetWorld(), equipment->GetUniqueID(), EquippedDatas[i]);
		Server_Equip(equipment->GetUniqueID());
	}

	// spawn inventory items
	for (int32 i = 0; i < InventoryClasses.Num(); ++i)
	{
		if (!InventoryClasses[i])continue;
		FEquipmentStateUpdateParameters state;
		state.State = EItemMode::Loot;
		AEqquipment* equipment = UEquipmentManagementComponent::SpawnEquipment(GetWorld(), EquippedClasses[i], state);
		if (!equipment)continue;
		UEquipmentManagementComponent::UpadteStatus(GetWorld(), equipment->GetUniqueID(), EquippedDatas[i]);
		Server_TryAddItem(equipment->GetUniqueID());
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

void UInventoryComponent::Server_Buy_Implementation(const FString& InObject)
{
	// TODO::check gold condition

	ADungeonCharacterBase* owner = Cast<ADungeonCharacterBase>(GetOwner());
	AEqquipment* equipment = UEquipmentManagementComponent::GetEquipmentFromUniqueID(GetWorld(), InObject);
	CheckNull(equipment);

	//check room
	CheckFalse(IsRoomAvailable(equipment));

	//TODO::TEST
	// spawn,tryadd,reducegold->client:refresh
	FEquipmentStateUpdateParameters state;
	state.State = EItemMode::Loot;
	AEqquipment* buy = UEquipmentManagementComponent::SpawnEquipment(GetWorld(), equipment->GetClass(), state);
	UEquipmentManagementComponent::PickUp(GetWorld(), buy->GetUniqueID(), owner);

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

	AEqquipment* equipment = UEquipmentManagementComponent::GetEquipmentFromUniqueID(GetWorld(), EquippedItems[idx]);
	CheckNull(equipment);

	AWeapon* weapon = Cast<AWeapon>(equipment);
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
	AEqquipment* equipment = UEquipmentManagementComponent::GetEquipmentFromUniqueID(GetWorld(), EquippedItems[idx]);
	CheckNull(equipment);

	AWeapon* weapon = Cast<AWeapon>(equipment);
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
	AEqquipment* equipment = UEquipmentManagementComponent::GetEquipmentFromUniqueID(GetWorld(), EquippedItems[idx]);
	CheckNull(equipment);

	AWeapon* weapon = Cast<AWeapon>(equipment);
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
			if (GetItemAtIndex(idx, &obj))return 0;
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
		if (Items[i] != FString())continue;
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
			if (GetItemAtIndex(idx, &obj))return 0;
			if (obj && obj != InObject)return 0;
		}
	}
	return 1;
}

void UInventoryComponent::Server_TryAddItem_Implementation(const FString& InObject)
{
	CLog::Print(InObject);
	if (InObject == FString())
	{
		CLog::Print("UInventoryComponent::Server_TryAddItem_Implementation uniqueID Error", -1, 10, FColor::Red);
		return;
	}
	AEqquipment* equipment = UEquipmentManagementComponent::GetEquipmentFromUniqueID(GetWorld(), InObject);
	if (!equipment)
	{
		CLog::Print("UInventoryComponent::Server_TryAddItem_Implementation equipment Error", -1, 10, FColor::Red);
		return;
	}

	if (equipment->HasOwnerCharacter())
	{
		ACharacter* ownerCharacter = Cast<ACharacter>(GetOwner());
		if (equipment->GetOwnerCharacter() != ownerCharacter)
		{
			CLog::Print("UInventoryComponent::Server_TryAddItem_Implementation owner Error", -1, 10, FColor::Red);
			return;
		}
	}
	if (!IsRoomAvailable(equipment))
	{
		//TODO::TEST
		ADungeonCharacterBase* ownerCharacter = Cast<ADungeonCharacterBase>(GetOwner());
		// check location
		if (!ownerCharacter)
		{
			CLog::Print("UInventoryComponent::Server_TryAddItem_Implementation OwnerCharacter is nullptr", -1, 10, FColor::Red);
			return;
		}

		// find drop location
		float halfSize = ownerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		FVector start = ownerCharacter->GetActorLocation();
		start.Z -= halfSize;
		FVector end = UNavigationSystemV1::GetRandomPointInNavigableRadius(GetWorld(), start, 200);
		start.Z += halfSize;

		UEquipmentManagementComponent::Drop(GetWorld(), InObject, start, end);
		return;
	}
	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (!IsRoomAvailable(equipment, i))continue;
		Server_AddItemAt(InObject, i);
		return;
	}
}

void UInventoryComponent::Server_AddItemAt_Implementation(const FString& InObject, int32 TopLeftIndex)
{
	CheckTrue(InObject == FString());
	AEqquipment* equipment = UEquipmentManagementComponent::GetEquipmentFromUniqueID(GetWorld(), InObject);

	int32 x, y, X, Y;
	IndexToTile(TopLeftIndex, x, y);
	equipment->GetItemObject()->GetDimensions(X, Y);
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

	//TODO::TEST
	ADungeonCharacterBase* ownerCharacter = Cast<ADungeonCharacterBase>(GetOwner());
	UEquipmentManagementComponent::PickUp(GetWorld(), InObject, ownerCharacter);
	equipment->GetItemObject()->SetInventoryComp(this);

	OnRep_Items();
}

void UInventoryComponent::Server_RemoveItem_Implementation(const FString& InObject)
{
	CheckTrue(InObject == FString());
	for (int32 i = 0; i < Items.Num(); i++)
		if (Items[i] == InObject)Items[i] = nullptr;

	//TODO::TEST
	ADungeonCharacterBase* ownerCharacter = Cast<ADungeonCharacterBase>(GetOwner());
	// check location
	if (!ownerCharacter)
	{
		CLog::Print("UInventoryComponent::Server_RemoveItem_Implementation OwnerCharacter is nullptr", -1, 10, FColor::Red);
		return;
	}

	// find drop location
	float halfSize = ownerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector start = ownerCharacter->GetActorLocation();
	start.Z -= halfSize;
	FVector end = UNavigationSystemV1::GetRandomPointInNavigableRadius(GetWorld(), start, 200);
	start.Z += halfSize;

	UEquipmentManagementComponent::Drop(GetWorld(), InObject, start, end);

	OnRep_Items();
}

void UInventoryComponent::GetAllItems(TMap<AEqquipment*, TTuple<int32, int32>>& Map)
{
	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (Items[i] == FString())continue;
		AEqquipment* cur = UEquipmentManagementComponent::GetEquipmentFromUniqueID(GetWorld(), Items[i]);
		if (!cur)continue;
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

void UInventoryComponent::Server_Sell_Implementation(const FString& InObject)
{
	// TODO::add gold

	// TODO::TEST	
	//destroy item
	UEquipmentManagementComponent::DestroyEquipment(GetWorld(), InObject);

	OnRep_Items();
}

void UInventoryComponent::Buy(AEqquipment* InObject)
{
	// TODO::check gold condition

	Server_Buy(InObject->GetUniqueID());
}

bool UInventoryComponent::CanTakeOffEquipment(int32 InIdx)
{
	if (!EquippedItems.IsValidIndex(InIdx))return 0;
	AEqquipment* item = UEquipmentManagementComponent::GetEquipmentFromUniqueID(GetWorld(), EquippedItems[InIdx]);
	if (!item)return 1;
	return IsRoomAvailable(item);
}

AEqquipment* UInventoryComponent::GetEquippedItems(int32 InIdx)
{
	if (!EquippedItems.IsValidIndex(InIdx))return nullptr;
	return  UEquipmentManagementComponent::GetEquipmentFromUniqueID(GetWorld(), EquippedItems[InIdx]);
}

void UInventoryComponent::Server_Equip_Implementation(const FString& InData)
{
	AEqquipment* item = UEquipmentManagementComponent::GetEquipmentFromUniqueID(GetWorld(), InData);

	if (InData == FString() || !item)
	{
		CLog::Print("UInventoryComponent::Equip InData is nullptr", -1, 10, FColor::Red);
		return;
	}

	APlayerCharacter* owner = Cast<APlayerCharacter>(GetOwner());

	int32 idx = int32(item->GetType());

	EquippedItems[idx] = InData;
	//EquippedItems[idx]->SetOwner(owner);
	//EquippedItems[idx]->ChangeVisibility(EItemMode::Equip);

	//TODO::TEST
	ADungeonCharacterBase* ownerCharacter = Cast<ADungeonCharacterBase>(GetOwner());
	UEquipmentManagementComponent::Equip(GetWorld(), InData, ownerCharacter);

	if (item->GetType() == EItemType::Weapon)
	{
		AWeapon* weapon = Cast<AWeapon>(item);
		if (weapon)
		{
			weapon->OffCollision();
			FAttachmentTransformRules f = { EAttachmentRule::SnapToTarget, 1 };
			weapon->SetTeamID(owner->GetGenericTeamId());
			//weapon->AttachItemToComponent(owner->GetMesh(), f, weapon->GetSocketName());
		}
	}
}

void UInventoryComponent::Server_ChangeEquippedData_Implementation(int32 InIdx, const FString& InData)
{
	AEqquipment* item = UEquipmentManagementComponent::GetEquipmentFromUniqueID(GetWorld(), InData);
	
	if (InData == FString() || !item)
	{
		CLog::Print("UInventoryComponent::Equip InData is nullptr", -1, 10, FColor::Red);
		return;	
	}

	APlayerCharacter* owner = Cast<APlayerCharacter>(GetOwner());

	int32 idx = int32(item->GetType());

	AEqquipment* equipped = UEquipmentManagementComponent::GetEquipmentFromUniqueID(GetWorld(), EquippedItems[idx]);

	if (!EquippedItems.IsValidIndex(idx))return;
	if (!CanTakeOffEquipment(idx))return;
	if (EquippedItems[idx] != FString() && !IsRoomAvailable(equipped))return;
	if (IsRoomAvailable(equipped))Server_TryAddItem(EquippedItems[idx]);

	// remove item effect
	if(EquippedItems[idx] != FString())
	{
		const TArray<FSkillEnhancement>& enhancementDatas = equipped->GetItemStatus().GetEnhancementDatas();
		USkillComponent* skill = CHelpers::GetComponent<USkillComponent>(owner);
		if (skill)
		{
			skill->EnhanceAbility(enhancementDatas, -1);
			for (auto i : equipped->GetTargetAttributes())
				skill->ApplyModToAttribute(i.Key, EGameplayModOp::Additive, -i.Value);
		}
	}

	//TODO::TEST
	ADungeonCharacterBase* ownerCharacter = Cast<ADungeonCharacterBase>(GetOwner());
	UEquipmentManagementComponent::Equip(GetWorld(), InData, ownerCharacter);
	EquippedItems[idx] = InData;
	//EquippedItems[idx]->SetOwner(owner);
	//EquippedItems[idx]->ChangeVisibility(EItemMode::Equip);

	if (item->GetType() == EItemType::Weapon)
	{
		AWeapon* weapon = Cast<AWeapon>(item);
		if (weapon)
		{
			weapon->OffCollision();
			FAttachmentTransformRules f = { EAttachmentRule::SnapToTarget, 1 };
			weapon->SetTeamID(owner->GetGenericTeamId());
			//weapon->AttachItemToComponent(owner->GetMesh(), f, weapon->GetSocketName());
		}
	}

	if (OnInventoryEquippedChanged.IsBound() && item->GetType() != EItemType::Weapon)
	{
		const TArray<FItemAppearanceData>& datas = item->GetAppearanceDatas();
		for (auto i : datas)	OnInventoryEquippedChanged.Broadcast(i.PartType, i.Index);
	}

	if (OnChangeHairVisiblity.IsBound() && item->GetType() == EItemType::Helms)
		OnChangeHairVisiblity.Broadcast(0);

	if (OnInventoryEquippedItemsChanged.IsBound())
		OnInventoryEquippedItemsChanged.Broadcast();

	OnRep_EquippedItems();

	// add item effect
	if (item)
	{
		const TArray<FSkillEnhancement>& enhancementDatas = item->GetItemStatus().GetEnhancementDatas();
		USkillComponent* skill = CHelpers::GetComponent<USkillComponent>(owner);
		if (skill)
		{
			skill->EnhanceAbility(enhancementDatas);
			for (auto i : item->GetTargetAttributes())
				skill->ApplyModToAttribute(i.Key, EGameplayModOp::Additive, i.Value);
		}
	}
}

void UInventoryComponent::Server_RemoveEquipped_Drag_Implementation(int32 InIdx)
{
	AEqquipment* item = UEquipmentManagementComponent::GetEquipmentFromUniqueID(GetWorld(), EquippedItems[InIdx]);

	if (EquippedItems[InIdx] == FString() || !item)
	{
		CLog::Print("UInventoryComponent::Equip InData is nullptr", -1, 10, FColor::Red);
		return;
	}

	if (!EquippedItems.IsValidIndex(InIdx))return;

	APlayerCharacter* owner = Cast<APlayerCharacter>(GetOwner());

	// remove item effect
	if(EquippedItems[InIdx] != FString())
	{
		const TArray<FSkillEnhancement>& enhancementDatas = item->GetItemStatus().GetEnhancementDatas();
		USkillComponent* skill = CHelpers::GetComponent<USkillComponent>(owner);
		if (skill)
		{
			skill->EnhanceAbility(enhancementDatas, -1);
			for (auto i : item->GetTargetAttributes())
				skill->ApplyModToAttribute(i.Key, EGameplayModOp::Additive, -i.Value);
		}
	}

	// reset to default
	if (OnInventoryEquippedChanged.IsBound() && item->GetType() != EItemType::Weapon)
	{
		const TArray<FItemAppearanceData>& datas = item->GetAppearanceDatas();
		for (auto i : datas)	
			OnInventoryEquippedChanged.Broadcast(i.PartType, 0);
	}
	
	if (OnChangeHairVisiblity.IsBound() && item->GetType() == EItemType::Helms)
		OnChangeHairVisiblity.Broadcast(1);

	EquippedItems[InIdx] = FString();

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

void UInventoryComponent::RefreshGrid()
{
	// refresh inv grid list
	if (OnInventoryChanged.IsBound())
		OnInventoryChanged.Broadcast();
}

void UInventoryComponent::SaveData(USaveGameData* SaveData)
{
	//save EquippedData
	SaveData->PlayerData.EquippedClasses.Empty();
	for (auto i : EquippedItems)
	{
		if (i == FString())continue;
		AEqquipment* item = UEquipmentManagementComponent::GetEquipmentFromUniqueID(GetWorld(), i);
		if (!item)continue;
		SaveData->PlayerData.EquippedClasses.Add(item->GetClass());
		SaveData->PlayerData.EquippedDatas.Add(item->GetItemStatus());
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

void UInventoryComponent::GetEquipmentEffectClasses(TArray<TSubclassOf<UGameplayEffect>>& Classes)const
{
	for (int32 i = int(EItemType::Helms); i<int(EItemType::Max); ++i)
	{
		if (EquippedItems[i] == FString())continue;
		AEqquipment* item = UEquipmentManagementComponent::GetEquipmentFromUniqueID(GetWorld(), EquippedItems[i]);
		if (!item)continue;
		item->GetAllEffectClasses(Classes);
	}
}
