#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Characters/CharcterAppearanceData.h"
#include "InventoryComponent.generated.h"

/**
 * 아이템 장착 및 보관
 */

class AWeapon;
class UUW_Inventory;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryEquipWeapon, AWeapon*, InAttachment);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryPresetChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryItemChanged, EAppearancePart, InMeshPart, int32, InIndex);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//property
private:
	UPROPERTY(ReplicatedUsing = "OnRep_Items")TArray<AWeapon*> Items;
	UPROPERTY(ReplicatedUsing = "OnRep_PresetItems")TArray<AWeapon*> PresetItems;
	int32 PresetIndex;

	UPROPERTY()UUW_Inventory* Widget;

	AWeapon* CurrentWeapon;

protected:
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AWeapon> DefaultWeapon;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AWeapon> InvTestClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widget", meta = (ClampMin = 1.00))
		int32 Columns = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Widget", meta = (ClampMin = 1.00))
		int32 Rows = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
		TSubclassOf<UUW_Inventory> WidgetClass;
public:
	FInventoryChanged OnInventoryChanged;
	FInventoryEquipWeapon OnInventoryEquipWeapon;
	FInventoryPresetChanged OnInventoryPresetChanged;
	FInventoryItemChanged OnInventoryItemChanged;

	//function
private:
	void InitDefault();
	void InitWidget();
	UFUNCTION() void OnRep_Items();
	UFUNCTION() void OnRep_PresetItems();

	void Reset();
	void IndexToTile(int32 InIndex, int32& X, int32& Y);
	bool GetItemAtIndex(int32 InIndex, AWeapon** OutObject);
protected:
public:
	void OnCollision();
	void OffCollision();
	void ResetHittedActors();

	bool IsRoomAvailable(AWeapon* InObject, int32 TopLeftIndex);
	bool IsRoomAvailable(AWeapon* InObject);
	bool IsRoomGreen(AWeapon* InObject, int32 TopLeftIndex);

	UFUNCTION(Reliable, Server)void Server_TryAddItem(AWeapon* InObject);
	UFUNCTION(Reliable, Server)void Server_AddItemAt(AWeapon* InObject, int32 TopLeftIndex);

	UFUNCTION(Reliable, Server)void Server_RemoveItem(AWeapon* InObject);

	void GetAllItems(TMap<AWeapon*, TTuple<int32, int32>>& Map);

	FORCEINLINE int32 TileToIndex(int32 X, int32 Y) {return X + (Y * Columns);}
	FORCEINLINE int32 GetRows() { return Rows; }
	FORCEINLINE int32 GetColumns() { return Columns; }

	//for EquipmentSlot
	bool CanTakeOffEquipment(int32 InIdx);
	bool CanTakeOffCurrentEquipment();
	AWeapon* GetPresetItems(int32 InIdx);
	FORCEINLINE AWeapon* GetCurrentPresetItem() { return PresetItems[PresetIndex]; };
	void Equip(AWeapon* InData);
	void EquipPreset(int32 InIdx);
	bool ChangePresetData(int32 InIdx, AWeapon* InData);
	void ChangePresetIndex(int32 InIdx);
	bool RemovePreset(int32 InIdx);
	bool RemovePreset_Drag(int32 InIdx);

	//Widget
	bool IsWidgetVisible();
	void OnWidget();
	void OffWidget();

	//
	// 드래그 드롭 - 이동 및 장착, 해제
	// 우클릭 - 장착 및 해제
	// 장착전에 조건 확인
	// 장착시 외형 변경, 효과 적용
	// 드롭시 등급에 맞는 이펙트
	// 격자무늬 인벤토리 탭3개
	// 프리셋 탭 2개
	//
	
	//ui
	//void LoadData(UCSaveGameData* const InData);

};
